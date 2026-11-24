#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#define MAX_LIVROS        500
#define MAX_USUARIOS      500
#define MAX_EMPRESTIMOS  2000


#define TAM_TITULO    101   
#define TAM_AUTOR      81   
#define TAM_EDITORA    61   
#define TAM_NOME      101   
#define TAM_CURSO      51   
#define TAM_TELEFONE   16  
#define TAM_LINHA     512   
#define TAM_DATA       11  

#define MAX_CODIGO_LIVRO   999999
#define MAX_MATRICULA    99999999
#define MAX_EXEMPLARES        999
#define ANO_MINIMO_LIVRO     1450  

#define PRAZO_EMPRESTIMO          7   
#define MAX_RENOVACOES            2   
#define MAX_EMPRESTIMOS_USUARIO   3   
#define TOP_RELATORIO            10   

#define LIVRO_EMPRESTADO 0
#define LIVRO_DISPONIVEL 1

#define EMP_DEVOLVIDO 0
#define EMP_ATIVO     1

#define ARQ_LIVROS          "livros.txt"
#define ARQ_USUARIOS        "usuarios.txt"
#define ARQ_EMPRESTIMOS     "emprestimos.txt"

#define ARQ_BKP_LIVROS      "backup_livros.txt"
#define ARQ_BKP_USUARIOS    "backup_usuarios.txt"
#define ARQ_BKP_EMPRESTIMOS "backup_emprestimos.txt"
#define ARQ_BKP_INFO        "backup_info.txt"

typedef struct {
    int dia;
    int mes;
    int ano;
} Data;

typedef struct {
    int  codigo;                    
    char titulo[TAM_TITULO];        
    char autor[TAM_AUTOR];          
    char editora[TAM_EDITORA];      
    int  anoPublicacao;             
    int  totalExemplares;          
    int  exemplaresDisponiveis;    
    int  status;                    
} Livro;

typedef struct {
    int  matricula;                
    char nome[TAM_NOME];            
    char curso[TAM_CURSO];          
    char telefone[TAM_TELEFONE];    
    Data dataCadastro;              
} Usuario;


typedef struct {
    int  codigo;                  
    int  matricula;                
    int  codigoLivro;              
    Data dataEmprestimo;           
    Data dataPrevista;              
    Data dataDevolucao;             
    int  renovacoes;                
    int  status;                    
} Emprestimo;

typedef struct {
    Livro      livros[MAX_LIVROS];
    int        qtdLivros;
    Usuario    usuarios[MAX_USUARIOS];
    int        qtdUsuarios;
    Emprestimo emprestimos[MAX_EMPRESTIMOS];
    int        qtdEmprestimos;
} Biblioteca;

int  lerLinha(char *destino, int tamanho);
void aparar(char *texto);
void sanitizar(char *texto);
void removerQuebraLinha(char *texto);
int  lerInteiro(const char *mensagem, int minimo, int maximo);
int  lerInteiroOpcional(const char *mensagem, int minimo, int maximo, int valorPadrao);
void lerTexto(const char *mensagem, char *destino, int tamanho, int obrigatorio);
int  lerConfirmacao(const char *mensagem);
Data lerData(const char *mensagem, Data padrao);
void pausar(void);
void imprimirLinha(char caractere, int quantidade);
void imprimirTitulo(const char *titulo);
int  contemTexto(const char *texto, const char *trecho);
int  telefoneValido(const char *telefone);

int  ehBissexto(int ano);
int  diasNoMes(int mes, int ano);
int  dataValida(Data d);
int  dataVazia(Data d);
Data obterDataAtual(void);
Data adicionarDias(Data d, int dias);
long dataParaDias(Data d);
int  diferencaDias(Data inicio, Data fim);
void formatarData(Data d, char *destino);

void inicializarData(Data *d);
void inicializarLivro(Livro *livro);
void inicializarUsuario(Usuario *usuario);
void inicializarEmprestimo(Emprestimo *emprestimo);
void inicializarBiblioteca(Biblioteca *b);

int  buscarLivroPorCodigo(const Livro livros[], int qtd, int codigo);
int  buscarUsuarioPorMatricula(const Usuario usuarios[], int qtd, int matricula);
int  buscarEmprestimoPorCodigo(const Emprestimo emprestimos[], int qtd, int codigo);
int  proximoCodigoEmprestimo(const Emprestimo emprestimos[], int qtd);
int  contarAtivosDoUsuario(const Biblioteca *b, int matricula);
int  usuarioPossuiAtraso(const Biblioteca *b, int matricula, Data hoje);
int  usuarioPossuiLivroAtivo(const Biblioteca *b, int matricula, int codigoLivro);
int  emprestimoAtrasado(const Emprestimo *e, Data hoje);
void atualizarStatusLivro(Livro *livro);
void recalcularDisponibilidade(Biblioteca *b);
int  contarEmprestimosAtrasados(const Biblioteca *b, Data hoje);
int  contarEmprestimosAtivos(const Biblioteca *b);

const char *textoStatusLivro(int status);
void imprimirCabecalhoLivros(void);
void imprimirLivroLinha(const Livro *livro);
void imprimirLivroDetalhado(const Livro *livro);
void imprimirCabecalhoUsuarios(void);
void imprimirUsuarioLinha(const Usuario *usuario);
void imprimirUsuarioDetalhado(const Usuario *usuario);
void imprimirCabecalhoEmprestimos(void);
void imprimirEmprestimoLinha(const Biblioteca *b, const Emprestimo *e, Data hoje);
void imprimirComprovante(const Biblioteca *b, const Emprestimo *e, const char *tipo);

int  cadastrarLivro(Livro livros[], int *qtdLivros);
int  cadastrarUsuario(Usuario usuarios[], int *qtdUsuarios);
void listarLivros(const Livro livros[], int qtd);
void listarUsuarios(const Usuario usuarios[], int qtd);

int  realizarEmprestimo(Biblioteca *b);
int  selecionarEmprestimoAtivo(const Biblioteca *b);
int  realizarDevolucao(Biblioteca *b);
int  renovarEmprestimo(Biblioteca *b);
void listarEmprestimosAtivos(const Biblioteca *b);
void consultarEmprestimosUsuario(const Biblioteca *b);

void pesquisarLivros(const Livro livros[], int qtd);
void pesquisarUsuarios(const Usuario usuarios[], int qtd);

void relatorioLivrosMaisEmprestados(const Biblioteca *b);
void relatorioUsuariosEmAtraso(const Biblioteca *b);
void buscaAvancada(const Livro livros[], int qtd);

int  finalizarGravacao(FILE *arquivo, const char *temporario, const char *destino);
int  salvarLivros(const Livro livros[], int qtd, const char *nomeArquivo);
int  salvarUsuarios(const Usuario usuarios[], int qtd, const char *nomeArquivo);
int  salvarEmprestimos(const Emprestimo emprestimos[], int qtd, const char *nomeArquivo);
int  carregarLivros(Livro livros[], int *qtd, const char *nomeArquivo);
int  carregarUsuarios(Usuario usuarios[], int *qtd, const char *nomeArquivo);
int  carregarEmprestimos(Emprestimo emprestimos[], int *qtd, const char *nomeArquivo);
int  salvarDados(const Biblioteca *b);
int  carregarDados(Biblioteca *b);
int  arquivoExiste(const char *nomeArquivo);
int  copiarArquivo(const char *origem, const char *destino);
int  realizarBackup(void);
void lerInfoBackup(char *destino, int tamanho);
int  restaurarBackup(Biblioteca *b);

int  exibirMenuPrincipal(const Biblioteca *b);
void menuLivros(Biblioteca *b);
void menuUsuarios(Biblioteca *b);
void menuEmprestimos(Biblioteca *b);
void menuRelatorios(const Biblioteca *b);
void menuArquivos(Biblioteca *b);


int main(void)
{
    static Biblioteca biblioteca;
    int opcao;
    int invalidos;

    inicializarBiblioteca(&biblioteca);

    imprimirTitulo("SISTEMA DE GERENCIAMENTO DE BIBLIOTECA");
    printf("Carregando dados...\n");
    invalidos = carregarDados(&biblioteca);

    if (invalidos == 0) {
        if (realizarBackup() > 0) {
            printf("Backup automatico realizado com sucesso.\n");
        }
    } else {
        printf("\nATENCAO: %d registro(s) invalido(s) foram ignorados.\n", invalidos);
        printf("O backup automatico NAO foi feito, para preservar a copia anterior.\n");
        printf("Se necessario, use o menu 'Arquivos e backup' > 'Restaurar backup'.\n");
    }
    pausar();

    do {
        opcao = exibirMenuPrincipal(&biblioteca);
        switch (opcao) {
            case 1: menuLivros(&biblioteca);      break;
            case 2: menuUsuarios(&biblioteca);    break;
            case 3: menuEmprestimos(&biblioteca); break;
            case 4: menuRelatorios(&biblioteca);  break;
            case 5: menuArquivos(&biblioteca);    break;
            case 0:
                if (salvarDados(&biblioteca)) {
                    printf("\nDados salvos. Obrigado por utilizar o sistema!\n");
                }
                break;
            default:
                break;
        }
    } while (opcao != 0);

    return 0;
}

int lerLinha(char *destino, int tamanho)
{
    int c;
    int descartou = 0;
    size_t len;

    if (fgets(destino, tamanho, stdin) == NULL) {
        printf("\n\nEntrada encerrada. Programa finalizado.\n");
        exit(0);
    }

    len = strlen(destino);
    if (len > 0 && destino[len - 1] == '\n') {
        destino[len - 1] = '\0';
        return 1;
    }

    while ((c = getchar()) != '\n' && c != EOF) {
        descartou = 1;
    }
    return descartou ? 2 : 1;
}

void aparar(char *texto)
{
    char *inicio = texto;
    size_t len;

    while (*inicio != '\0' && isspace((unsigned char)*inicio)) {
        inicio++;
    }
    if (inicio != texto) {
        memmove(texto, inicio, strlen(inicio) + 1);
    }
    len = strlen(texto);
    while (len > 0 && isspace((unsigned char)texto[len - 1])) {
        texto[--len] = '\0';
    }
}

void sanitizar(char *texto)
{
    for (; *texto != '\0'; texto++) {
        if (*texto == '|') {
            *texto = '/';
        }
    }
}

void removerQuebraLinha(char *texto)
{
    size_t len = strlen(texto);
    while (len > 0 && (texto[len - 1] == '\n' || texto[len - 1] == '\r')) {
        texto[--len] = '\0';
    }
}

int lerInteiro(const char *mensagem, int minimo, int maximo)
{
    char buffer[32];
    char *fim;
    long valor;
    int  resultado;

    while (1) {
        printf("%s", mensagem);
        resultado = lerLinha(buffer, sizeof(buffer));
        aparar(buffer);

        if (buffer[0] == '\0') {
            printf("  >> Valor obrigatorio. Tente novamente.\n");
            continue;
        }
        valor = strtol(buffer, &fim, 10);
        if (resultado == 2 || *fim != '\0') {
            printf("  >> Digite apenas numeros inteiros.\n");
            continue;
        }
        if (valor < minimo || valor > maximo) {
            printf("  >> O valor deve estar entre %d e %d.\n", minimo, maximo);
            continue;
        }
        return (int)valor;
    }
}

int lerInteiroOpcional(const char *mensagem, int minimo, int maximo, int valorPadrao)
{
    char buffer[32];
    char *fim;
    long valor;
    int  resultado;

    while (1) {
        printf("%s", mensagem);
        resultado = lerLinha(buffer, sizeof(buffer));
        aparar(buffer);

        if (buffer[0] == '\0') {
            return valorPadrao;
        }
        valor = strtol(buffer, &fim, 10);
        if (resultado == 2 || *fim != '\0') {
            printf("  >> Digite apenas numeros inteiros (ou ENTER para ignorar).\n");
            continue;
        }
        if (valor < minimo || valor > maximo) {
            printf("  >> O valor deve estar entre %d e %d.\n", minimo, maximo);
            continue;
        }
        return (int)valor;
    }
}

void lerTexto(const char *mensagem, char *destino, int tamanho, int obrigatorio)
{
    int resultado;

    while (1) {
        printf("%s", mensagem);
        resultado = lerLinha(destino, tamanho);
        aparar(destino);
        sanitizar(destino);

        if (resultado == 2) {
            printf("  >> Aviso: o texto foi limitado a %d caracteres.\n", tamanho - 1);
        }
        if (obrigatorio && destino[0] == '\0') {
            printf("  >> Campo obrigatorio. Tente novamente.\n");
            continue;
        }
        return;
    }
}

int lerConfirmacao(const char *mensagem)
{
    char buffer[16];

    while (1) {
        printf("%s (S/N): ", mensagem);
        lerLinha(buffer, sizeof(buffer));
        aparar(buffer);

        if (buffer[0] == 'S' || buffer[0] == 's') {
            return 1;
        }
        if (buffer[0] == 'N' || buffer[0] == 'n') {
            return 0;
        }
        printf("  >> Responda S para sim ou N para nao.\n");
    }
}

Data lerData(const char *mensagem, Data padrao)
{
    char buffer[32];
    char textoPadrao[TAM_DATA];
    char sobra;
    Data d;

    while (1) {
        printf("%s", mensagem);
        if (!dataVazia(padrao)) {
            formatarData(padrao, textoPadrao);
            printf("[ENTER = %s]: ", textoPadrao);
        } else {
            printf(": ");
        }
        lerLinha(buffer, sizeof(buffer));
        aparar(buffer);

        if (buffer[0] == '\0') {
            if (!dataVazia(padrao)) {
                return padrao;
            }
            printf("  >> Data obrigatoria.\n");
            continue;
        }
        if (sscanf(buffer, "%d/%d/%d%c", &d.dia, &d.mes, &d.ano, &sobra) != 3
            || !dataValida(d)) {
            printf("  >> Data invalida. Use o formato dd/mm/aaaa (ex.: 05/03/2025).\n");
            continue;
        }
        return d;
    }
}

void pausar(void)
{
    char buffer[8];
    printf("\nPressione ENTER para continuar...");
    lerLinha(buffer, sizeof(buffer));
}

void imprimirLinha(char caractere, int quantidade)
{
    int i;
    for (i = 0; i < quantidade; i++) {
        putchar(caractere);
    }
    putchar('\n');
}

void imprimirTitulo(const char *titulo)
{
    int largura = 78;
    int espacos = (largura - (int)strlen(titulo)) / 2;

    printf("\n");
    imprimirLinha('=', largura);
    printf("%*s%s\n", espacos > 0 ? espacos : 0, "", titulo);
    imprimirLinha('=', largura);
}

int contemTexto(const char *texto, const char *trecho)
{
    size_t i, j;
    size_t tamTexto = strlen(texto);
    size_t tamTrecho = strlen(trecho);

    if (tamTrecho == 0) {
        return 1;
    }
    for (i = 0; i + tamTrecho <= tamTexto; i++) {
        for (j = 0; j < tamTrecho; j++) {
            if (tolower((unsigned char)texto[i + j]) !=
                tolower((unsigned char)trecho[j])) {
                break;
            }
        }
        if (j == tamTrecho) {
            return 1;
        }
    }
    return 0;
}

int telefoneValido(const char *telefone)
{
    int digitos = 0;

    for (; *telefone != '\0'; telefone++) {
        if (isdigit((unsigned char)*telefone)) {
            digitos++;
        } else if (strchr(" ()-+", *telefone) == NULL) {
            return 0;
        }
    }
    return digitos >= 8;
}

int ehBissexto(int ano)
{
    return (ano % 4 == 0 && ano % 100 != 0) || (ano % 400 == 0);
}

int diasNoMes(int mes, int ano)
{
    int dias[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    if (mes == 2 && ehBissexto(ano)) {
        return 29;
    }
    return dias[mes - 1];
}

int dataValida(Data d)
{
    if (d.ano < 1900 || d.ano > 2100) return 0;
    if (d.mes < 1 || d.mes > 12)      return 0;
    if (d.dia < 1 || d.dia > diasNoMes(d.mes, d.ano)) return 0;
    return 1;
}

int dataVazia(Data d)
{
    return d.dia == 0 && d.mes == 0 && d.ano == 0;
}

Data obterDataAtual(void)
{
    Data d;
    time_t agora = time(NULL);
    struct tm *t = localtime(&agora);

    if (t == NULL) {            
        d.dia = 1; d.mes = 1; d.ano = 2000;
        return d;
    }
    d.dia = t->tm_mday;
    d.mes = t->tm_mon + 1;      
    d.ano = t->tm_year + 1900;  
    return d;
}

Data adicionarDias(Data d, int dias)
{
    while (dias > 0) {
        d.dia++;
        if (d.dia > diasNoMes(d.mes, d.ano)) {
            d.dia = 1;
            d.mes++;
            if (d.mes > 12) {
                d.mes = 1;
                d.ano++;
            }
        }
        dias--;
    }
    return d;
}

long dataParaDias(Data d)
{
    long total = 0;
    int a, m;

    for (a = 1900; a < d.ano; a++) {
        total += ehBissexto(a) ? 366 : 365;
    }
    for (m = 1; m < d.mes; m++) {
        total += diasNoMes(m, d.ano);
    }
    return total + d.dia;
}

int diferencaDias(Data inicio, Data fim)
{
    return (int)(dataParaDias(fim) - dataParaDias(inicio));
}

void formatarData(Data d, char *destino)
{
    if (dataVazia(d)) {
        strcpy(destino, "    -     ");
    } else {
        sprintf(destino, "%02d/%02d/%04d", d.dia, d.mes, d.ano);
    }
}

void inicializarData(Data *d)
{
    d->dia = 0;
    d->mes = 0;
    d->ano = 0;
}

void inicializarLivro(Livro *livro)
{
    livro->codigo = 0;
    livro->titulo[0] = '\0';
    livro->autor[0] = '\0';
    livro->editora[0] = '\0';
    livro->anoPublicacao = 0;
    livro->totalExemplares = 0;
    livro->exemplaresDisponiveis = 0;
    livro->status = LIVRO_DISPONIVEL;
}

void inicializarUsuario(Usuario *usuario)
{
    usuario->matricula = 0;
    usuario->nome[0] = '\0';
    usuario->curso[0] = '\0';
    usuario->telefone[0] = '\0';
    inicializarData(&usuario->dataCadastro);
}

void inicializarEmprestimo(Emprestimo *emprestimo)
{
    emprestimo->codigo = 0;
    emprestimo->matricula = 0;
    emprestimo->codigoLivro = 0;
    inicializarData(&emprestimo->dataEmprestimo);
    inicializarData(&emprestimo->dataPrevista);
    inicializarData(&emprestimo->dataDevolucao);
    emprestimo->renovacoes = 0;
    emprestimo->status = EMP_ATIVO;
}

void inicializarBiblioteca(Biblioteca *b)
{
    int i;

    for (i = 0; i < MAX_LIVROS; i++) {
        inicializarLivro(&b->livros[i]);
    }
    for (i = 0; i < MAX_USUARIOS; i++) {
        inicializarUsuario(&b->usuarios[i]);
    }
    for (i = 0; i < MAX_EMPRESTIMOS; i++) {
        inicializarEmprestimo(&b->emprestimos[i]);
    }
    b->qtdLivros = 0;
    b->qtdUsuarios = 0;
    b->qtdEmprestimos = 0;
}

int buscarLivroPorCodigo(const Livro livros[], int qtd, int codigo)
{
    int i;
    for (i = 0; i < qtd; i++) {
        if (livros[i].codigo == codigo) {
            return i;
        }
    }
    return -1;
}

int buscarUsuarioPorMatricula(const Usuario usuarios[], int qtd, int matricula)
{
    int i;
    for (i = 0; i < qtd; i++) {
        if (usuarios[i].matricula == matricula) {
            return i;
        }
    }
    return -1;
}

int buscarEmprestimoPorCodigo(const Emprestimo emprestimos[], int qtd, int codigo)
{
    int i;
    for (i = 0; i < qtd; i++) {
        if (emprestimos[i].codigo == codigo) {
            return i;
        }
    }
    return -1;
}

int proximoCodigoEmprestimo(const Emprestimo emprestimos[], int qtd)
{
    int i, maior = 0;
    for (i = 0; i < qtd; i++) {
        if (emprestimos[i].codigo > maior) {
            maior = emprestimos[i].codigo;
        }
    }
    return maior + 1;
}

int contarAtivosDoUsuario(const Biblioteca *b, int matricula)
{
    int i, total = 0;
    for (i = 0; i < b->qtdEmprestimos; i++) {
        if (b->emprestimos[i].matricula == matricula &&
            b->emprestimos[i].status == EMP_ATIVO) {
            total++;
        }
    }
    return total;
}

int emprestimoAtrasado(const Emprestimo *e, Data hoje)
{
    return e->status == EMP_ATIVO && diferencaDias(e->dataPrevista, hoje) > 0;
}

int usuarioPossuiAtraso(const Biblioteca *b, int matricula, Data hoje)
{
    int i;
    for (i = 0; i < b->qtdEmprestimos; i++) {
        if (b->emprestimos[i].matricula == matricula &&
            emprestimoAtrasado(&b->emprestimos[i], hoje)) {
            return 1;
        }
    }
    return 0;
}

int usuarioPossuiLivroAtivo(const Biblioteca *b, int matricula, int codigoLivro)
{
    int i;
    for (i = 0; i < b->qtdEmprestimos; i++) {
        if (b->emprestimos[i].matricula == matricula &&
            b->emprestimos[i].codigoLivro == codigoLivro &&
            b->emprestimos[i].status == EMP_ATIVO) {
            return 1;
        }
    }
    return 0;
}

void atualizarStatusLivro(Livro *livro)
{
    livro->status = (livro->exemplaresDisponiveis > 0) ? LIVRO_DISPONIVEL
                                                       : LIVRO_EMPRESTADO;
}

void recalcularDisponibilidade(Biblioteca *b)
{
    int i, j, ativos;

    for (i = 0; i < b->qtdLivros; i++) {
        ativos = 0;
        for (j = 0; j < b->qtdEmprestimos; j++) {
            if (b->emprestimos[j].codigoLivro == b->livros[i].codigo &&
                b->emprestimos[j].status == EMP_ATIVO) {
                ativos++;
            }
        }
        b->livros[i].exemplaresDisponiveis = b->livros[i].totalExemplares - ativos;
        if (b->livros[i].exemplaresDisponiveis < 0) {
            b->livros[i].exemplaresDisponiveis = 0;
        }
        atualizarStatusLivro(&b->livros[i]);
    }
}

int contarEmprestimosAtivos(const Biblioteca *b)
{
    int i, total = 0;
    for (i = 0; i < b->qtdEmprestimos; i++) {
        if (b->emprestimos[i].status == EMP_ATIVO) {
            total++;
        }
    }
    return total;
}

int contarEmprestimosAtrasados(const Biblioteca *b, Data hoje)
{
    int i, total = 0;
    for (i = 0; i < b->qtdEmprestimos; i++) {
        if (emprestimoAtrasado(&b->emprestimos[i], hoje)) {
            total++;
        }
    }
    return total;
}

const char *textoStatusLivro(int status)
{
    return (status == LIVRO_DISPONIVEL) ? "Disponivel" : "Emprestado";
}

void imprimirCabecalhoLivros(void)
{
    printf("%-6s %-26s %-20s %-4s %-7s %s\n",
           "Codigo", "Titulo", "Autor", "Ano", "Disp.", "Status");
    imprimirLinha('-', 78);
}

void imprimirLivroLinha(const Livro *livro)
{
    printf("%-6d %-26.26s %-20.20s %4d %3d/%-3d %s\n",
           livro->codigo, livro->titulo, livro->autor, livro->anoPublicacao,
           livro->exemplaresDisponiveis, livro->totalExemplares,
           textoStatusLivro(livro->status));
}

void imprimirLivroDetalhado(const Livro *livro)
{
    printf("  Codigo ............: %d\n", livro->codigo);
    printf("  Titulo ............: %s\n", livro->titulo);
    printf("  Autor .............: %s\n", livro->autor);
    printf("  Editora ...........: %s\n", livro->editora);
    printf("  Ano de publicacao .: %d\n", livro->anoPublicacao);
    printf("  Exemplares ........: %d disponivel(is) de %d\n",
           livro->exemplaresDisponiveis, livro->totalExemplares);
    printf("  Status ............: %s\n", textoStatusLivro(livro->status));
}

void imprimirCabecalhoUsuarios(void)
{
    printf("%-8s %-26s %-16s %-15s %s\n",
           "Matric.", "Nome", "Curso", "Telefone", "Cadastro");
    imprimirLinha('-', 78);
}

void imprimirUsuarioLinha(const Usuario *usuario)
{
    char data[TAM_DATA];
    formatarData(usuario->dataCadastro, data);
    printf("%-8d %-26.26s %-16.16s %-15s %s\n",
           usuario->matricula, usuario->nome, usuario->curso,
           usuario->telefone, data);
}

void imprimirUsuarioDetalhado(const Usuario *usuario)
{
    char data[TAM_DATA];
    formatarData(usuario->dataCadastro, data);
    printf("  Matricula .........: %d\n", usuario->matricula);
    printf("  Nome ..............: %s\n", usuario->nome);
    printf("  Curso .............: %s\n", usuario->curso);
    printf("  Telefone ..........: %s\n", usuario->telefone);
    printf("  Data de cadastro ..: %s\n", data);
}

void imprimirCabecalhoEmprestimos(void)
{
    printf("%-5s %-16s %-20s %-10s %-10s %-3s %s\n",
           "Cod.", "Usuario", "Livro", "Emprestimo", "Prevista", "Ren", "Situacao");
    imprimirLinha('-', 78);
}

void imprimirEmprestimoLinha(const Biblioteca *b, const Emprestimo *e, Data hoje)
{
    char dataEmp[TAM_DATA], dataPrev[TAM_DATA], situacao[20];
    int iu = buscarUsuarioPorMatricula(b->usuarios, b->qtdUsuarios, e->matricula);
    int il = buscarLivroPorCodigo(b->livros, b->qtdLivros, e->codigoLivro);

    formatarData(e->dataEmprestimo, dataEmp);
    formatarData(e->dataPrevista, dataPrev);

    if (e->status == EMP_DEVOLVIDO) {
        strcpy(situacao, "Devolvido");
    } else if (emprestimoAtrasado(e, hoje)) {
        sprintf(situacao, "ATRASO %dd", diferencaDias(e->dataPrevista, hoje));
    } else {
        strcpy(situacao, "No prazo");
    }

    printf("%-5d %-16.16s %-20.20s %-10s %-10s %-3d %s\n",
           e->codigo,
           iu >= 0 ? b->usuarios[iu].nome : "(desconhecido)",
           il >= 0 ? b->livros[il].titulo : "(desconhecido)",
           dataEmp, dataPrev, e->renovacoes, situacao);
}

void imprimirComprovante(const Biblioteca *b, const Emprestimo *e, const char *tipo)
{
    char dataEmp[TAM_DATA], dataPrev[TAM_DATA], dataDev[TAM_DATA];
    int iu = buscarUsuarioPorMatricula(b->usuarios, b->qtdUsuarios, e->matricula);
    int il = buscarLivroPorCodigo(b->livros, b->qtdLivros, e->codigoLivro);

    formatarData(e->dataEmprestimo, dataEmp);
    formatarData(e->dataPrevista, dataPrev);
    formatarData(e->dataDevolucao, dataDev);

    printf("\n");
    imprimirLinha('-', 50);
    printf("  COMPROVANTE DE %s\n", tipo);
    imprimirLinha('-', 50);
    printf("  Emprestimo No ...: %d\n", e->codigo);
    printf("  Usuario .........: %d - %s\n", e->matricula,
           iu >= 0 ? b->usuarios[iu].nome : "(desconhecido)");
    printf("  Livro ...........: %d - %s\n", e->codigoLivro,
           il >= 0 ? b->livros[il].titulo : "(desconhecido)");
    printf("  Data emprestimo .: %s\n", dataEmp);
    printf("  Devolver ate ....: %s\n", dataPrev);
    if (e->status == EMP_DEVOLVIDO) {
        printf("  Devolvido em ....: %s\n", dataDev);
    }
    printf("  Renovacoes ......: %d de %d\n", e->renovacoes, MAX_RENOVACOES);
    imprimirLinha('-', 50);
}

int cadastrarLivro(Livro livros[], int *qtdLivros)
{
    Livro novo;
    char mensagem[80];
    int anoAtual = obterDataAtual().ano;

    imprimirTitulo("CADASTRO DE LIVRO");

    if (*qtdLivros >= MAX_LIVROS) {
        printf("Limite de %d livros atingido. Nao e possivel cadastrar.\n", MAX_LIVROS);
        return 0;
    }

    inicializarLivro(&novo);

    while (1) {
        novo.codigo = lerInteiro("Codigo do livro (1 a 999999, 0 = cancelar): ",
                                 0, MAX_CODIGO_LIVRO);
        if (novo.codigo == 0) {
            printf("Cadastro cancelado.\n");
            return 0;
        }
        if (buscarLivroPorCodigo(livros, *qtdLivros, novo.codigo) == -1) {
            break;
        }
        printf("  >> Ja existe um livro com o codigo %d.\n", novo.codigo);
    }

    lerTexto("Titulo (ate 100 caracteres): ", novo.titulo, TAM_TITULO, 1);
    lerTexto("Autor (ate 80 caracteres): ", novo.autor, TAM_AUTOR, 1);
    lerTexto("Editora (ate 60 caracteres): ", novo.editora, TAM_EDITORA, 1);

    sprintf(mensagem, "Ano de publicacao (%d a %d): ", ANO_MINIMO_LIVRO, anoAtual);
    novo.anoPublicacao = lerInteiro(mensagem, ANO_MINIMO_LIVRO, anoAtual);

    sprintf(mensagem, "Numero de exemplares (1 a %d): ", MAX_EXEMPLARES);
    novo.totalExemplares = lerInteiro(mensagem, 1, MAX_EXEMPLARES);

    novo.exemplaresDisponiveis = novo.totalExemplares;
    atualizarStatusLivro(&novo);

    printf("\nConfira os dados informados:\n");
    imprimirLivroDetalhado(&novo);

    if (!lerConfirmacao("\nConfirmar cadastro?")) {
        printf("Cadastro cancelado.\n");
        return 0;
    }

    livros[*qtdLivros] = novo;
    (*qtdLivros)++;
    printf("\nLivro cadastrado com sucesso!\n");
    return 1;
}

int cadastrarUsuario(Usuario usuarios[], int *qtdUsuarios)
{
    Usuario novo;
    Data hoje = obterDataAtual();

    imprimirTitulo("CADASTRO DE USUARIO");

    if (*qtdUsuarios >= MAX_USUARIOS) {
        printf("Limite de %d usuarios atingido. Nao e possivel cadastrar.\n", MAX_USUARIOS);
        return 0;
    }

    inicializarUsuario(&novo);

   
    while (1) {
        novo.matricula = lerInteiro("Matricula (1 a 99999999, 0 = cancelar): ",
                                    0, MAX_MATRICULA);
        if (novo.matricula == 0) {
            printf("Cadastro cancelado.\n");
            return 0;
        }
        if (buscarUsuarioPorMatricula(usuarios, *qtdUsuarios, novo.matricula) == -1) {
            break;
        }
        printf("  >> Ja existe um usuario com a matricula %d.\n", novo.matricula);
    }

    lerTexto("Nome completo (ate 100 caracteres): ", novo.nome, TAM_NOME, 1);
    lerTexto("Curso (ate 50 caracteres): ", novo.curso, TAM_CURSO, 1);

    while (1) {
        lerTexto("Telefone (ex.: (44) 99999-0000): ", novo.telefone, TAM_TELEFONE, 1);
        if (telefoneValido(novo.telefone)) {
            break;
        }
        printf("  >> Telefone invalido. Use apenas numeros, espacos, ( ) - + "
               "e pelo menos 8 digitos.\n");
    }

    while (1) {
        novo.dataCadastro = lerData("Data de cadastro (dd/mm/aaaa) ", hoje);
        if (diferencaDias(hoje, novo.dataCadastro) > 0) {
            printf("  >> A data de cadastro nao pode ser futura.\n");
            continue;
        }
        break;
    }

    printf("\nConfira os dados informados:\n");
    imprimirUsuarioDetalhado(&novo);

    if (!lerConfirmacao("\nConfirmar cadastro?")) {
        printf("Cadastro cancelado.\n");
        return 0;
    }

    usuarios[*qtdUsuarios] = novo;
    (*qtdUsuarios)++;
    printf("\nUsuario cadastrado com sucesso!\n");
    return 1;
}

void listarLivros(const Livro livros[], int qtd)
{
    int i;

    imprimirTitulo("ACERVO DE LIVROS");
    if (qtd == 0) {
        printf("Nenhum livro cadastrado.\n");
        return;
    }
    imprimirCabecalhoLivros();
    for (i = 0; i < qtd; i++) {
        imprimirLivroLinha(&livros[i]);
    }
    printf("\nTotal: %d livro(s).\n", qtd);
}

void listarUsuarios(const Usuario usuarios[], int qtd)
{
    int i;

    imprimirTitulo("USUARIOS CADASTRADOS");
    if (qtd == 0) {
        printf("Nenhum usuario cadastrado.\n");
        return;
    }
    imprimirCabecalhoUsuarios();
    for (i = 0; i < qtd; i++) {
        imprimirUsuarioLinha(&usuarios[i]);
    }
    printf("\nTotal: %d usuario(s).\n", qtd);
}

int realizarEmprestimo(Biblioteca *b)
{
    Emprestimo novo;
    Data hoje = obterDataAtual();
    int matricula, codigoLivro, iu, il;

    imprimirTitulo("REALIZAR EMPRESTIMO");

    if (b->qtdEmprestimos >= MAX_EMPRESTIMOS) {
        printf("Limite de registros de emprestimo atingido.\n");
        return 0;
    }
    if (b->qtdUsuarios == 0 || b->qtdLivros == 0) {
        printf("E preciso ter ao menos um usuario e um livro cadastrados.\n");
        return 0;
    }

    matricula = lerInteiro("Matricula do usuario (0 = cancelar): ", 0, MAX_MATRICULA);
    if (matricula == 0) {
        printf("Operacao cancelada.\n");
        return 0;
    }
    iu = buscarUsuarioPorMatricula(b->usuarios, b->qtdUsuarios, matricula);
    if (iu == -1) {
        printf("  >> Usuario nao encontrado. Cadastre-o antes do emprestimo.\n");
        return 0;
    }
    printf("  Usuario: %s (%s)\n", b->usuarios[iu].nome, b->usuarios[iu].curso);

    if (usuarioPossuiAtraso(b, matricula, hoje)) {
        printf("  >> Emprestimo bloqueado: o usuario possui devolucao em atraso.\n");
        return 0;
    }
    if (contarAtivosDoUsuario(b, matricula) >= MAX_EMPRESTIMOS_USUARIO) {
        printf("  >> Emprestimo bloqueado: o usuario ja possui %d livros emprestados "
               "(limite).\n", MAX_EMPRESTIMOS_USUARIO);
        return 0;
    }
    codigoLivro = lerInteiro("Codigo do livro (0 = cancelar): ", 0, MAX_CODIGO_LIVRO);
    if (codigoLivro == 0) {
        printf("Operacao cancelada.\n");
        return 0;
    }
    il = buscarLivroPorCodigo(b->livros, b->qtdLivros, codigoLivro);
    if (il == -1) {
        printf("  >> Livro nao encontrado. Use a pesquisa para descobrir o codigo.\n");
        return 0;
    }
    printf("  Livro: %s - %s\n", b->livros[il].titulo, b->livros[il].autor);

    if (b->livros[il].exemplaresDisponiveis <= 0) {
        printf("  >> Nao ha exemplares disponiveis deste livro no momento.\n");
        return 0;
    }
    if (usuarioPossuiLivroAtivo(b, matricula, codigoLivro)) {
        printf("  >> Este usuario ja esta com um exemplar deste livro.\n");
        return 0;
    }

    inicializarEmprestimo(&novo);
    novo.codigo = proximoCodigoEmprestimo(b->emprestimos, b->qtdEmprestimos);
    novo.matricula = matricula;
    novo.codigoLivro = codigoLivro;

    while (1) {
        novo.dataEmprestimo = lerData("Data do emprestimo (dd/mm/aaaa) ", hoje);
        if (diferencaDias(hoje, novo.dataEmprestimo) > 0) {
            printf("  >> A data do emprestimo nao pode ser futura.\n");
            continue;
        }
        if (diferencaDias(b->usuarios[iu].dataCadastro, novo.dataEmprestimo) < 0) {
            printf("  >> A data nao pode ser anterior ao cadastro do usuario.\n");
            continue;
        }
        break;
    }
    novo.dataPrevista = adicionarDias(novo.dataEmprestimo, PRAZO_EMPRESTIMO);
    novo.status = EMP_ATIVO;

    imprimirComprovante(b, &novo, "EMPRESTIMO (PREVIA)");
    if (!lerConfirmacao("Confirmar emprestimo?")) {
        printf("Emprestimo cancelado.\n");
        return 0;
    }

    b->emprestimos[b->qtdEmprestimos] = novo;
    b->qtdEmprestimos++;
    b->livros[il].exemplaresDisponiveis--;
    atualizarStatusLivro(&b->livros[il]);

    printf("\nEmprestimo No %d registrado com sucesso!\n", novo.codigo);
    if (emprestimoAtrasado(&novo, hoje)) {
        printf("Atencao: pela data informada, este emprestimo ja esta em atraso.\n");
    }
    return 1;
}

int selecionarEmprestimoAtivo(const Biblioteca *b)
{
    Data hoje = obterDataAtual();
    int matricula, iu, i, codigo, ie, encontrados = 0;

    matricula = lerInteiro("Matricula do usuario (0 = cancelar): ", 0, MAX_MATRICULA);
    if (matricula == 0) {
        printf("Operacao cancelada.\n");
        return -1;
    }
    iu = buscarUsuarioPorMatricula(b->usuarios, b->qtdUsuarios, matricula);
    if (iu == -1) {
        printf("  >> Usuario nao encontrado.\n");
        return -1;
    }

    printf("\nEmprestimos ativos de %s:\n", b->usuarios[iu].nome);
    imprimirCabecalhoEmprestimos();
    for (i = 0; i < b->qtdEmprestimos; i++) {
        if (b->emprestimos[i].matricula == matricula &&
            b->emprestimos[i].status == EMP_ATIVO) {
            imprimirEmprestimoLinha(b, &b->emprestimos[i], hoje);
            encontrados++;
        }
    }
    if (encontrados == 0) {
        printf("(nenhum emprestimo ativo)\n");
        return -1;
    }

    while (1) {
        codigo = lerInteiro("\nCodigo do emprestimo (0 = cancelar): ", 0, 2147483647);
        if (codigo == 0) {
            printf("Operacao cancelada.\n");
            return -1;
        }
        ie = buscarEmprestimoPorCodigo(b->emprestimos, b->qtdEmprestimos, codigo);
        if (ie == -1 || b->emprestimos[ie].matricula != matricula ||
            b->emprestimos[ie].status != EMP_ATIVO) {
            printf("  >> Codigo invalido. Escolha um dos emprestimos listados acima.\n");
            continue;
        }
        return ie;
    }
}

int realizarDevolucao(Biblioteca *b)
{
    Data hoje = obterDataAtual();
    Data dataDevolucao;
    Emprestimo *e;
    int ie, il, atraso;

    imprimirTitulo("REALIZAR DEVOLUCAO");

    ie = selecionarEmprestimoAtivo(b);
    if (ie == -1) {
        return 0;
    }
    e = &b->emprestimos[ie];  

    while (1) {
        dataDevolucao = lerData("Data da devolucao (dd/mm/aaaa) ", hoje);
        if (diferencaDias(hoje, dataDevolucao) > 0) {
            printf("  >> A data de devolucao nao pode ser futura.\n");
            continue;
        }
        if (diferencaDias(e->dataEmprestimo, dataDevolucao) < 0) {
            printf("  >> A devolucao nao pode ser anterior ao emprestimo.\n");
            continue;
        }
        break;
    }

    if (!lerConfirmacao("Confirmar devolucao?")) {
        printf("Devolucao cancelada.\n");
        return 0;
    }

    e->status = EMP_DEVOLVIDO;
    e->dataDevolucao = dataDevolucao;

    il = buscarLivroPorCodigo(b->livros, b->qtdLivros, e->codigoLivro);
    if (il != -1) {
        b->livros[il].exemplaresDisponiveis++;
        if (b->livros[il].exemplaresDisponiveis > b->livros[il].totalExemplares) {
            b->livros[il].exemplaresDisponiveis = b->livros[il].totalExemplares;
        }
        atualizarStatusLivro(&b->livros[il]);
    }

    imprimirComprovante(b, e, "DEVOLUCAO");
    atraso = diferencaDias(e->dataPrevista, dataDevolucao);
    if (atraso > 0) {
        printf("Devolucao realizada com %d dia(s) de atraso.\n", atraso);
    } else {
        printf("Devolucao realizada dentro do prazo. Obrigado!\n");
    }
    return 1;
}

int renovarEmprestimo(Biblioteca *b)
{
    Data hoje = obterDataAtual();
    Data novaData;
    char textoData[TAM_DATA];
    Emprestimo *e;
    int ie;

    imprimirTitulo("RENOVAR EMPRESTIMO");

    ie = selecionarEmprestimoAtivo(b);
    if (ie == -1) {
        return 0;
    }
    e = &b->emprestimos[ie];

    if (e->renovacoes >= MAX_RENOVACOES) {
        printf("  >> Renovacao negada: limite de %d renovacoes atingido.\n", MAX_RENOVACOES);
        return 0;
    }
    if (usuarioPossuiAtraso(b, e->matricula, hoje)) {
        printf("  >> Renovacao negada: o usuario possui emprestimo em atraso.\n");
        printf("     Realize a devolucao do(s) livro(s) atrasado(s).\n");
        return 0;
    }

    novaData = adicionarDias(e->dataPrevista, PRAZO_EMPRESTIMO);
    formatarData(novaData, textoData);
    printf("Nova data prevista de devolucao: %s\n", textoData);

    if (!lerConfirmacao("Confirmar renovacao?")) {
        printf("Renovacao cancelada.\n");
        return 0;
    }

    e->dataPrevista = novaData;
    e->renovacoes++;
    imprimirComprovante(b, e, "RENOVACAO");
    printf("Emprestimo renovado com sucesso!\n");
    return 1;
}

void listarEmprestimosAtivos(const Biblioteca *b)
{
    Data hoje = obterDataAtual();
    int i, total = 0;

    imprimirTitulo("EMPRESTIMOS ATIVOS");
    for (i = 0; i < b->qtdEmprestimos; i++) {
        if (b->emprestimos[i].status == EMP_ATIVO) {
            if (total == 0) {
                imprimirCabecalhoEmprestimos();
            }
            imprimirEmprestimoLinha(b, &b->emprestimos[i], hoje);
            total++;
        }
    }
    if (total == 0) {
        printf("Nenhum emprestimo ativo no momento.\n");
    } else {
        printf("\nTotal: %d emprestimo(s) ativo(s), %d em atraso.\n",
               total, contarEmprestimosAtrasados(b, hoje));
    }
}

void consultarEmprestimosUsuario(const Biblioteca *b)
{
    Data hoje = obterDataAtual();
    int matricula, iu, i, total = 0;

    imprimirTitulo("HISTORICO DE EMPRESTIMOS DO USUARIO");
    matricula = lerInteiro("Matricula do usuario (0 = cancelar): ", 0, MAX_MATRICULA);
    if (matricula == 0) {
        return;
    }
    iu = buscarUsuarioPorMatricula(b->usuarios, b->qtdUsuarios, matricula);
    if (iu == -1) {
        printf("  >> Usuario nao encontrado.\n");
        return;
    }

    printf("\nUsuario: %s\n\n", b->usuarios[iu].nome);
    for (i = 0; i < b->qtdEmprestimos; i++) {
        if (b->emprestimos[i].matricula == matricula) {
            if (total == 0) {
                imprimirCabecalhoEmprestimos();
            }
            imprimirEmprestimoLinha(b, &b->emprestimos[i], hoje);
            total++;
        }
    }
    if (total == 0) {
        printf("Este usuario ainda nao realizou emprestimos.\n");
    } else {
        printf("\nTotal: %d emprestimo(s), %d ativo(s).\n",
               total, contarAtivosDoUsuario(b, matricula));
    }
}

void pesquisarLivros(const Livro livros[], int qtd)
{
    char termo[TAM_TITULO];
    int opcao, codigo, indice, i, encontrados;

    do {
        imprimirTitulo("PESQUISAR LIVROS");
        printf("  1 - Por codigo\n");
        printf("  2 - Por titulo\n");
        printf("  3 - Por autor\n");
        printf("  0 - Voltar\n");
        opcao = lerInteiro("Opcao: ", 0, 3);

        if (opcao == 1) {
            codigo = lerInteiro("Codigo do livro: ", 1, MAX_CODIGO_LIVRO);
            indice = buscarLivroPorCodigo(livros, qtd, codigo);
            if (indice == -1) {
                printf("\nNenhum livro encontrado com o codigo %d.\n", codigo);
            } else {
                printf("\n");
                imprimirLivroDetalhado(&livros[indice]);
            }
            pausar();
        } else if (opcao == 2 || opcao == 3) {
            lerTexto(opcao == 2 ? "Parte do titulo: " : "Parte do nome do autor: ",
                     termo, sizeof(termo), 1);
            encontrados = 0;
            printf("\n");
            for (i = 0; i < qtd; i++) {
                if ((opcao == 2 && contemTexto(livros[i].titulo, termo)) ||
                    (opcao == 3 && contemTexto(livros[i].autor, termo))) {
                    if (encontrados == 0) {
                        imprimirCabecalhoLivros();
                    }
                    imprimirLivroLinha(&livros[i]);
                    encontrados++;
                }
            }
            if (encontrados == 0) {
                printf("Nenhum livro encontrado para \"%s\".\n", termo);
            } else {
                printf("\n%d livro(s) encontrado(s).\n", encontrados);
            }
            pausar();
        }
    } while (opcao != 0);
}

void pesquisarUsuarios(const Usuario usuarios[], int qtd)
{
    char termo[TAM_NOME];
    int opcao, matricula, indice, i, encontrados;

    do {
        imprimirTitulo("PESQUISAR USUARIOS");
        printf("  1 - Por matricula\n");
        printf("  2 - Por nome\n");
        printf("  0 - Voltar\n");
        opcao = lerInteiro("Opcao: ", 0, 2);

        if (opcao == 1) {
            matricula = lerInteiro("Matricula: ", 1, MAX_MATRICULA);
            indice = buscarUsuarioPorMatricula(usuarios, qtd, matricula);
            if (indice == -1) {
                printf("\nNenhum usuario encontrado com a matricula %d.\n", matricula);
            } else {
                printf("\n");
                imprimirUsuarioDetalhado(&usuarios[indice]);
            }
            pausar();
        } else if (opcao == 2) {
            lerTexto("Parte do nome: ", termo, sizeof(termo), 1);
            encontrados = 0;
            printf("\n");
            for (i = 0; i < qtd; i++) {
                if (contemTexto(usuarios[i].nome, termo)) {
                    if (encontrados == 0) {
                        imprimirCabecalhoUsuarios();
                    }
                    imprimirUsuarioLinha(&usuarios[i]);
                    encontrados++;
                }
            }
            if (encontrados == 0) {
                printf("Nenhum usuario encontrado para \"%s\".\n", termo);
            } else {
                printf("\n%d usuario(s) encontrado(s).\n", encontrados);
            }
            pausar();
        }
    } while (opcao != 0);
}

void relatorioLivrosMaisEmprestados(const Biblioteca *b)
{
    int contagem[MAX_LIVROS];
    int ordem[MAX_LIVROS];
    int i, j, maior, aux, il, posicao = 0;

    imprimirTitulo("RELATORIO - LIVROS MAIS EMPRESTADOS");

    if (b->qtdEmprestimos == 0) {
        printf("Ainda nao ha emprestimos registrados.\n");
        return;
    }

    for (i = 0; i < b->qtdLivros; i++) {
        contagem[i] = 0;
        ordem[i] = i;
    }
    for (i = 0; i < b->qtdEmprestimos; i++) {
        il = buscarLivroPorCodigo(b->livros, b->qtdLivros, b->emprestimos[i].codigoLivro);
        if (il != -1) {
            contagem[il]++;
        }
    }

    for (i = 0; i < b->qtdLivros - 1; i++) {
        maior = i;
        for (j = i + 1; j < b->qtdLivros; j++) {
            if (contagem[ordem[j]] > contagem[ordem[maior]]) {
                maior = j;
            }
        }
        if (maior != i) {
            aux = ordem[i];
            ordem[i] = ordem[maior];
            ordem[maior] = aux;
        }
    }

    printf("%-4s %-6s %-36s %-20s %s\n", "Pos.", "Codigo", "Titulo", "Autor", "Qtd.");
    imprimirLinha('-', 78);
    for (i = 0; i < b->qtdLivros && posicao < TOP_RELATORIO; i++) {
        il = ordem[i];
        if (contagem[il] == 0) {
            break;
        }
        posicao++;
        printf("%-4d %-6d %-36.36s %-20.20s %4d\n", posicao, b->livros[il].codigo,
               b->livros[il].titulo, b->livros[il].autor, contagem[il]);
    }
    printf("\nTotal de emprestimos registrados: %d\n", b->qtdEmprestimos);
}

void relatorioUsuariosEmAtraso(const Biblioteca *b)
{
    Data hoje = obterDataAtual();
    char dataPrev[TAM_DATA], dataHoje[TAM_DATA];
    int iu, i, il, primeiro, usuariosAtraso = 0, emprestimosAtraso = 0;
    const Emprestimo *e;

    imprimirTitulo("RELATORIO - USUARIOS COM EMPRESTIMOS EM ATRASO");
    formatarData(hoje, dataHoje);
    printf("Data de referencia: %s\n\n", dataHoje);

    for (iu = 0; iu < b->qtdUsuarios; iu++) {
        primeiro = 1;
        for (i = 0; i < b->qtdEmprestimos; i++) {
            e = &b->emprestimos[i];
            if (e->matricula != b->usuarios[iu].matricula || !emprestimoAtrasado(e, hoje)) {
                continue;
            }
            if (primeiro) {
                printf("%d - %s | Curso: %s | Tel.: %s\n",
                       b->usuarios[iu].matricula, b->usuarios[iu].nome,
                       b->usuarios[iu].curso, b->usuarios[iu].telefone);
                usuariosAtraso++;
                primeiro = 0;
            }
            il = buscarLivroPorCodigo(b->livros, b->qtdLivros, e->codigoLivro);
            formatarData(e->dataPrevista, dataPrev);
            printf("    Emp. %-5d %-32.32s prevista %s  atraso: %d dia(s)\n",
                   e->codigo, il != -1 ? b->livros[il].titulo : "(desconhecido)",
                   dataPrev, diferencaDias(e->dataPrevista, hoje));
            emprestimosAtraso++;
        }
        if (!primeiro) {
            printf("\n");
        }
    }

    if (usuariosAtraso == 0) {
        printf("Nenhum usuario com emprestimo em atraso. \n");
    } else {
        imprimirLinha('-', 78);
        printf("Usuarios em atraso: %d | Emprestimos em atraso: %d\n",
               usuariosAtraso, emprestimosAtraso);
    }
}

void buscaAvancada(const Livro livros[], int qtd)
{
    char titulo[TAM_TITULO], autor[TAM_AUTOR], editora[TAM_EDITORA];
    int anoMin, anoMax, aux, somenteDisponiveis, i, encontrados = 0;

    imprimirTitulo("BUSCA AVANCADA DE LIVROS");
    printf("Preencha os criterios desejados. Tecle ENTER para ignorar um criterio.\n\n");

    lerTexto("Titulo contem ..........: ", titulo, sizeof(titulo), 0);
    lerTexto("Autor contem ...........: ", autor, sizeof(autor), 0);
    lerTexto("Editora contem .........: ", editora, sizeof(editora), 0);
    anoMin = lerInteiroOpcional("Publicado a partir do ano: ", 0, 9999, 0);
    anoMax = lerInteiroOpcional("Publicado ate o ano .....: ", 0, 9999, 9999);
    somenteDisponiveis = lerInteiroOpcional(
        "Somente com exemplar disponivel? (1 = sim, 0 = nao) [ENTER = nao]: ", 0, 1, 0);

    if (anoMin > anoMax) {         
        aux = anoMin;
        anoMin = anoMax;
        anoMax = aux;
        printf("  (faixa de anos invertida foi corrigida para %d a %d)\n", anoMin, anoMax);
    }

    printf("\n");
    for (i = 0; i < qtd; i++) {
        if (!contemTexto(livros[i].titulo, titulo))       continue;
        if (!contemTexto(livros[i].autor, autor))         continue;
        if (!contemTexto(livros[i].editora, editora))     continue;
        if (livros[i].anoPublicacao < anoMin)             continue;
        if (livros[i].anoPublicacao > anoMax)             continue;
        if (somenteDisponiveis && livros[i].exemplaresDisponiveis <= 0) continue;

        if (encontrados == 0) {
            imprimirCabecalhoLivros();
        }
        imprimirLivroLinha(&livros[i]);
        encontrados++;
    }

    if (encontrados == 0) {
        printf("Nenhum livro atende a todos os criterios informados.\n");
    } else {
        printf("\n%d livro(s) encontrado(s).\n", encontrados);
    }
}

int finalizarGravacao(FILE *arquivo, const char *temporario, const char *destino)
{
    int erro = ferror(arquivo);

    if (fclose(arquivo) != 0) {
        erro = 1;
    }
    if (erro) {
        remove(temporario);
        return 0;
    }
    remove(destino);                     
    return rename(temporario, destino) == 0;
}

int salvarLivros(const Livro livros[], int qtd, const char *nomeArquivo)
{
    FILE *arquivo;
    char temporario[128];
    int i;

    sprintf(temporario, "%s.tmp", nomeArquivo);
    arquivo = fopen(temporario, "w");
    if (arquivo == NULL) {
        return 0;
    }
    fprintf(arquivo, "# codigo|titulo|autor|editora|ano|total_exemplares|"
                     "exemplares_disponiveis|status\n");
    for (i = 0; i < qtd; i++) {
        fprintf(arquivo, "%d|%s|%s|%s|%d|%d|%d|%s\n",
                livros[i].codigo, livros[i].titulo, livros[i].autor,
                livros[i].editora, livros[i].anoPublicacao,
                livros[i].totalExemplares, livros[i].exemplaresDisponiveis,
                livros[i].status == LIVRO_DISPONIVEL ? "DISPONIVEL" : "EMPRESTADO");
    }
    return finalizarGravacao(arquivo, temporario, nomeArquivo);
}

int salvarUsuarios(const Usuario usuarios[], int qtd, const char *nomeArquivo)
{
    FILE *arquivo;
    char temporario[128];
    char data[TAM_DATA];
    int i;

    sprintf(temporario, "%s.tmp", nomeArquivo);
    arquivo = fopen(temporario, "w");
    if (arquivo == NULL) {
        return 0;
    }
    fprintf(arquivo, "# matricula|nome|curso|telefone|data_cadastro\n");
    for (i = 0; i < qtd; i++) {
        formatarData(usuarios[i].dataCadastro, data);
        fprintf(arquivo, "%d|%s|%s|%s|%s\n",
                usuarios[i].matricula, usuarios[i].nome, usuarios[i].curso,
                usuarios[i].telefone, data);
    }
    return finalizarGravacao(arquivo, temporario, nomeArquivo);
}

int salvarEmprestimos(const Emprestimo emprestimos[], int qtd, const char *nomeArquivo)
{
    FILE *arquivo;
    char temporario[128];
    char dataEmp[TAM_DATA], dataPrev[TAM_DATA], dataDev[TAM_DATA];
    int i;

    sprintf(temporario, "%s.tmp", nomeArquivo);
    arquivo = fopen(temporario, "w");
    if (arquivo == NULL) {
        return 0;
    }
    fprintf(arquivo, "# codigo|matricula|codigo_livro|data_emprestimo|"
                     "data_prevista|data_devolucao|renovacoes|status\n");
    for (i = 0; i < qtd; i++) {
        formatarData(emprestimos[i].dataEmprestimo, dataEmp);
        formatarData(emprestimos[i].dataPrevista, dataPrev);
        if (dataVazia(emprestimos[i].dataDevolucao)) {
            strcpy(dataDev, "-");
        } else {
            formatarData(emprestimos[i].dataDevolucao, dataDev);
        }
        fprintf(arquivo, "%d|%d|%d|%s|%s|%s|%d|%s\n",
                emprestimos[i].codigo, emprestimos[i].matricula,
                emprestimos[i].codigoLivro, dataEmp, dataPrev, dataDev,
                emprestimos[i].renovacoes,
                emprestimos[i].status == EMP_ATIVO ? "ATIVO" : "DEVOLVIDO");
    }
    return finalizarGravacao(arquivo, temporario, nomeArquivo);
}

int carregarLivros(Livro livros[], int *qtd, const char *nomeArquivo)
{
    FILE *arquivo;
    char linha[TAM_LINHA];
    char status[20];
    Livro l;
    int invalidas = 0;

    *qtd = 0;
    arquivo = fopen(nomeArquivo, "r");
    if (arquivo == NULL) {
        return -1;
    }

    while (fgets(linha, sizeof(linha), arquivo) != NULL) {
        removerQuebraLinha(linha);
        if (linha[0] == '\0' || linha[0] == '#') {
            continue;
        }
        inicializarLivro(&l);
        if (*qtd >= MAX_LIVROS ||
            sscanf(linha, "%d|%100[^|]|%80[^|]|%60[^|]|%d|%d|%d|%19s",
                   &l.codigo, l.titulo, l.autor, l.editora, &l.anoPublicacao,
                   &l.totalExemplares, &l.exemplaresDisponiveis, status) != 8 ||
            l.codigo <= 0 || l.totalExemplares <= 0 ||
            buscarLivroPorCodigo(livros, *qtd, l.codigo) != -1) {
            invalidas++;
            continue;
        }
        atualizarStatusLivro(&l);
        livros[*qtd] = l;
        (*qtd)++;
    }
    fclose(arquivo);
    return invalidas;
}

int carregarUsuarios(Usuario usuarios[], int *qtd, const char *nomeArquivo)
{
    FILE *arquivo;
    char linha[TAM_LINHA];
    Usuario u;
    int invalidas = 0;

    *qtd = 0;
    arquivo = fopen(nomeArquivo, "r");
    if (arquivo == NULL) {
        return -1;
    }

    while (fgets(linha, sizeof(linha), arquivo) != NULL) {
        removerQuebraLinha(linha);
        if (linha[0] == '\0' || linha[0] == '#') {
            continue;
        }
        inicializarUsuario(&u);
        if (*qtd >= MAX_USUARIOS ||
            sscanf(linha, "%d|%100[^|]|%50[^|]|%15[^|]|%d/%d/%d",
                   &u.matricula, u.nome, u.curso, u.telefone,
                   &u.dataCadastro.dia, &u.dataCadastro.mes,
                   &u.dataCadastro.ano) != 7 ||
            u.matricula <= 0 || !dataValida(u.dataCadastro) ||
            buscarUsuarioPorMatricula(usuarios, *qtd, u.matricula) != -1) {
            invalidas++;
            continue;
        }
        usuarios[*qtd] = u;
        (*qtd)++;
    }
    fclose(arquivo);
    return invalidas;
}

int carregarEmprestimos(Emprestimo emprestimos[], int *qtd, const char *nomeArquivo)
{
    FILE *arquivo;
    char linha[TAM_LINHA];
    char devolucao[20], status[20];
    Emprestimo e;
    int invalidas = 0;
    int lidos;

    *qtd = 0;
    arquivo = fopen(nomeArquivo, "r");
    if (arquivo == NULL) {
        return -1;
    }

    while (fgets(linha, sizeof(linha), arquivo) != NULL) {
        removerQuebraLinha(linha);
        if (linha[0] == '\0' || linha[0] == '#') {
            continue;
        }
        inicializarEmprestimo(&e);
        lidos = sscanf(linha, "%d|%d|%d|%d/%d/%d|%d/%d/%d|%19[^|]|%d|%19s",
                       &e.codigo, &e.matricula, &e.codigoLivro,
                       &e.dataEmprestimo.dia, &e.dataEmprestimo.mes,
                       &e.dataEmprestimo.ano,
                       &e.dataPrevista.dia, &e.dataPrevista.mes,
                       &e.dataPrevista.ano,
                       devolucao, &e.renovacoes, status);

        if (*qtd >= MAX_EMPRESTIMOS || lidos != 12 || e.codigo <= 0 ||
            !dataValida(e.dataEmprestimo) || !dataValida(e.dataPrevista) ||
            buscarEmprestimoPorCodigo(emprestimos, *qtd, e.codigo) != -1) {
            invalidas++;
            continue;
        }

        e.status = (strcmp(status, "ATIVO") == 0) ? EMP_ATIVO : EMP_DEVOLVIDO;
        if (e.status == EMP_DEVOLVIDO) {
            if (sscanf(devolucao, "%d/%d/%d", &e.dataDevolucao.dia,
                       &e.dataDevolucao.mes, &e.dataDevolucao.ano) != 3 ||
                !dataValida(e.dataDevolucao)) {
                invalidas++;
                continue;
            }
        }
        emprestimos[*qtd] = e;
        (*qtd)++;
    }
    fclose(arquivo);
    return invalidas;
}

int salvarDados(const Biblioteca *b)
{
    int ok = 1;

    if (!salvarLivros(b->livros, b->qtdLivros, ARQ_LIVROS)) {
        printf("  >> ERRO ao salvar %s\n", ARQ_LIVROS);
        ok = 0;
    }
    if (!salvarUsuarios(b->usuarios, b->qtdUsuarios, ARQ_USUARIOS)) {
        printf("  >> ERRO ao salvar %s\n", ARQ_USUARIOS);
        ok = 0;
    }
    if (!salvarEmprestimos(b->emprestimos, b->qtdEmprestimos, ARQ_EMPRESTIMOS)) {
        printf("  >> ERRO ao salvar %s\n", ARQ_EMPRESTIMOS);
        ok = 0;
    }
    return ok;
}

int carregarDados(Biblioteca *b)
{
    int rLivros, rUsuarios, rEmprestimos, invalidos = 0;

    rLivros = carregarLivros(b->livros, &b->qtdLivros, ARQ_LIVROS);
    rUsuarios = carregarUsuarios(b->usuarios, &b->qtdUsuarios, ARQ_USUARIOS);
    rEmprestimos = carregarEmprestimos(b->emprestimos, &b->qtdEmprestimos, ARQ_EMPRESTIMOS);

    if (rLivros == -1 && rUsuarios == -1 && rEmprestimos == -1) {
        printf("Nenhum arquivo de dados encontrado: iniciando com a base vazia.\n");
        printf("Os arquivos serao criados automaticamente no primeiro cadastro.\n");
        return 0;
    }

    if (rLivros > 0)      invalidos += rLivros;
    if (rUsuarios > 0)    invalidos += rUsuarios;
    if (rEmprestimos > 0) invalidos += rEmprestimos;

    recalcularDisponibilidade(b);

    printf("Dados carregados: %d livro(s), %d usuario(s), %d emprestimo(s).\n",
           b->qtdLivros, b->qtdUsuarios, b->qtdEmprestimos);
    return invalidos;
}

int arquivoExiste(const char *nomeArquivo)
{
    FILE *arquivo = fopen(nomeArquivo, "r");
    if (arquivo == NULL) {
        return 0;
    }
    fclose(arquivo);
    return 1;
}

int copiarArquivo(const char *origem, const char *destino)
{
    FILE *entrada, *saida;
    char bloco[1024];
    size_t lidos;
    int ok = 1;

    entrada = fopen(origem, "rb");
    if (entrada == NULL) {
        return 0;
    }
    saida = fopen(destino, "wb");
    if (saida == NULL) {
        fclose(entrada);
        return 0;
    }
    while ((lidos = fread(bloco, 1, sizeof(bloco), entrada)) > 0) {
        if (fwrite(bloco, 1, lidos, saida) != lidos) {
            ok = 0;
            break;
        }
    }
    fclose(entrada);
    if (fclose(saida) != 0) {
        ok = 0;
    }
    return ok;
}

int realizarBackup(void)
{
    FILE *info;
    time_t agora;
    struct tm *t;
    int copiados = 0;

    if (arquivoExiste(ARQ_LIVROS) && copiarArquivo(ARQ_LIVROS, ARQ_BKP_LIVROS)) {
        copiados++;
    }
    if (arquivoExiste(ARQ_USUARIOS) && copiarArquivo(ARQ_USUARIOS, ARQ_BKP_USUARIOS)) {
        copiados++;
    }
    if (arquivoExiste(ARQ_EMPRESTIMOS) &&
        copiarArquivo(ARQ_EMPRESTIMOS, ARQ_BKP_EMPRESTIMOS)) {
        copiados++;
    }

    if (copiados > 0) {
        info = fopen(ARQ_BKP_INFO, "w");
        if (info != NULL) {
            agora = time(NULL);
            t = localtime(&agora);
            if (t != NULL) {
                fprintf(info, "%02d/%02d/%04d %02d:%02d\n", t->tm_mday, t->tm_mon + 1,
                        t->tm_year + 1900, t->tm_hour, t->tm_min);
            }
            fclose(info);
        }
    }
    return copiados;
}

void lerInfoBackup(char *destino, int tamanho)
{
    FILE *info = fopen(ARQ_BKP_INFO, "r");

    strcpy(destino, "nenhum");
    if (info == NULL) {
        return;
    }
    if (fgets(destino, tamanho, info) == NULL) {
        strcpy(destino, "nenhum");
    }
    removerQuebraLinha(destino);
    fclose(info);
}

int restaurarBackup(Biblioteca *b)
{
    char quando[40];

    imprimirTitulo("RESTAURAR BACKUP");

    if (!arquivoExiste(ARQ_BKP_LIVROS) && !arquivoExiste(ARQ_BKP_USUARIOS) &&
        !arquivoExiste(ARQ_BKP_EMPRESTIMOS)) {
        printf("Nenhum backup encontrado.\n");
        return 0;
    }

    lerInfoBackup(quando, sizeof(quando));
    printf("Backup disponivel de: %s\n", quando);
    printf("ATENCAO: todas as alteracoes feitas depois desse backup serao perdidas.\n");
    if (!lerConfirmacao("Deseja realmente restaurar o backup?")) {
        printf("Restauracao cancelada.\n");
        return 0;
    }

    if (arquivoExiste(ARQ_BKP_LIVROS))      copiarArquivo(ARQ_BKP_LIVROS, ARQ_LIVROS);
    if (arquivoExiste(ARQ_BKP_USUARIOS))    copiarArquivo(ARQ_BKP_USUARIOS, ARQ_USUARIOS);
    if (arquivoExiste(ARQ_BKP_EMPRESTIMOS)) copiarArquivo(ARQ_BKP_EMPRESTIMOS, ARQ_EMPRESTIMOS);

    inicializarBiblioteca(b);
    carregarDados(b);
    printf("Backup restaurado com sucesso.\n");
    return 1;
}

int exibirMenuPrincipal(const Biblioteca *b)
{
    Data hoje = obterDataAtual();
    char dataHoje[TAM_DATA];
    int atrasados = contarEmprestimosAtrasados(b, hoje);

    formatarData(hoje, dataHoje);
    imprimirTitulo("SISTEMA DE GERENCIAMENTO DE BIBLIOTECA");
    printf(" Hoje: %s | Livros: %d | Usuarios: %d | Emprestimos ativos: %d",
           dataHoje, b->qtdLivros, b->qtdUsuarios, contarEmprestimosAtivos(b));
    if (atrasados > 0) {
        printf(" (%d em atraso)", atrasados);
    }
    printf("\n");
    imprimirLinha('-', 78);
    printf("  1 - Livros\n");
    printf("  2 - Usuarios\n");
    printf("  3 - Emprestimos e devolucoes\n");
    printf("  4 - Relatorios e busca avancada\n");
    printf("  5 - Arquivos e backup\n");
    printf("  0 - Sair\n");
    imprimirLinha('-', 78);
    return lerInteiro("Escolha uma opcao: ", 0, 5);
}

void menuLivros(Biblioteca *b)
{
    int opcao;

    do {
        imprimirTitulo("MENU LIVROS");
        printf("  1 - Cadastrar livro\n");
        printf("  2 - Listar todos os livros\n");
        printf("  3 - Pesquisar livros (codigo, titulo ou autor)\n");
        printf("  0 - Voltar ao menu principal\n");
        opcao = lerInteiro("Opcao: ", 0, 3);

        switch (opcao) {
            case 1:
                if (cadastrarLivro(b->livros, &b->qtdLivros)) {
                    salvarDados(b);
                }
                pausar();
                break;
            case 2:
                listarLivros(b->livros, b->qtdLivros);
                pausar();
                break;
            case 3:
                pesquisarLivros(b->livros, b->qtdLivros);
                break;
            default:
                break;
        }
    } while (opcao != 0);
}

void menuUsuarios(Biblioteca *b)
{
    int opcao;

    do {
        imprimirTitulo("MENU USUARIOS");
        printf("  1 - Cadastrar usuario\n");
        printf("  2 - Listar todos os usuarios\n");
        printf("  3 - Pesquisar usuarios (matricula ou nome)\n");
        printf("  0 - Voltar ao menu principal\n");
        opcao = lerInteiro("Opcao: ", 0, 3);

        switch (opcao) {
            case 1:
                if (cadastrarUsuario(b->usuarios, &b->qtdUsuarios)) {
                    salvarDados(b);
                }
                pausar();
                break;
            case 2:
                listarUsuarios(b->usuarios, b->qtdUsuarios);
                pausar();
                break;
            case 3:
                pesquisarUsuarios(b->usuarios, b->qtdUsuarios);
                break;
            default:
                break;
        }
    } while (opcao != 0);
}

void menuEmprestimos(Biblioteca *b)
{
    int opcao;

    do {
        imprimirTitulo("MENU EMPRESTIMOS E DEVOLUCOES");
        printf("  1 - Realizar emprestimo\n");
        printf("  2 - Realizar devolucao\n");
        printf("  3 - Renovar emprestimo\n");
        printf("  4 - Listar emprestimos ativos\n");
        printf("  5 - Historico de emprestimos de um usuario\n");
        printf("  0 - Voltar ao menu principal\n");
        opcao = lerInteiro("Opcao: ", 0, 5);

        switch (opcao) {
            case 1:
                if (realizarEmprestimo(b)) {
                    salvarDados(b);
                }
                pausar();
                break;
            case 2:
                if (realizarDevolucao(b)) {
                    salvarDados(b);
                }
                pausar();
                break;
            case 3:
                if (renovarEmprestimo(b)) {
                    salvarDados(b);
                }
                pausar();
                break;
            case 4:
                listarEmprestimosAtivos(b);
                pausar();
                break;
            case 5:
                consultarEmprestimosUsuario(b);
                pausar();
                break;
            default:
                break;
        }
    } while (opcao != 0);
}

void menuRelatorios(const Biblioteca *b)
{
    int opcao;

    do {
        imprimirTitulo("MENU RELATORIOS E BUSCA AVANCADA");
        printf("  1 - Livros mais emprestados\n");
        printf("  2 - Usuarios com emprestimos em atraso\n");
        printf("  3 - Busca avancada de livros (multiplos criterios)\n");
        printf("  0 - Voltar ao menu principal\n");
        opcao = lerInteiro("Opcao: ", 0, 3);

        switch (opcao) {
            case 1:
                relatorioLivrosMaisEmprestados(b);
                pausar();
                break;
            case 2:
                relatorioUsuariosEmAtraso(b);
                pausar();
                break;
            case 3:
                buscaAvancada(b->livros, b->qtdLivros);
                pausar();
                break;
            default:
                break;
        }
    } while (opcao != 0);
}

void menuArquivos(Biblioteca *b)
{
    char quando[40];
    int opcao, copiados;

    do {
        lerInfoBackup(quando, sizeof(quando));
        imprimirTitulo("MENU ARQUIVOS E BACKUP");
        printf(" Ultimo backup: %s\n\n", quando);
        printf("  1 - Salvar dados agora\n");
        printf("  2 - Fazer backup agora\n");
        printf("  3 - Restaurar ultimo backup\n");
        printf("  0 - Voltar ao menu principal\n");
        opcao = lerInteiro("Opcao: ", 0, 3);

        switch (opcao) {
            case 1:
                if (salvarDados(b)) {
                    printf("\nDados salvos em %s, %s e %s.\n",
                           ARQ_LIVROS, ARQ_USUARIOS, ARQ_EMPRESTIMOS);
                }
                pausar();
                break;
            case 2:
                salvarDados(b);
                copiados = realizarBackup();
                printf("\nBackup concluido: %d arquivo(s) copiado(s).\n", copiados);
                pausar();
                break;
            case 3:
                restaurarBackup(b);
                pausar();
                break;
            default:
                break;
        }
    } while (opcao != 0);
}
