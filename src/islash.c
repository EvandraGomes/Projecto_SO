/*
 * islash.c - Shell interpretador de comandos (UEsh).
 * Sistemas Operativos - IADE 2025/2026
 *
 * Comandos internos (correm DENTRO deste processo):
 *   cd DIR     - mudar diretorio corrente
 *   pwd        - mostrar diretorio corrente
 *   echo TEXTO - imprimir texto
 *   sleep NUM  - pausar NUM segundos
 *   exit       - sair do shell
 *
 * Qualquer outro comando -> fork() + execvp() + waitpid()
 */

#define _POSIX_C_SOURCE 200809L

#include "utilitarios_comuns.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>         /* chdir, fork, execvp, getcwd, gethostname, sleep */
#include <sys/wait.h>       /* waitpid */
#include <sys/types.h>
#include <errno.h>
#include <signal.h>

#define MAX_TOKENS    64
#define HOSTNAME_LEN  256
#define PATH_LEN      4096

static void mostrar_prompt(void);
static int  tokenizar(char *linha, char **tokens, int max_tokens);
static int  eh_builtin(const char *cmd);
static void executar_builtin(int n, char **tokens);
static void executar_externo(char **tokens);


int main(int argc, char *argv[]) {

    const char *ajuda_islash =
        "Uso: islash\n"
        "Shell interpretador de comandos.\n"
        "\n"
        "Comandos internos:\n"
        "  cd DIR      altera o diretorio corrente\n"
        "  pwd         mostra o diretorio corrente\n"
        "  echo TEXTO  escreve no ecra\n"
        "  sleep NUM   pausa NUM segundos\n"
        "  exit        sai do shell\n"
        "\n"
        "Qualquer outro comando e' executado como processo externo.\n"
        "\n"
        "Opcoes:\n"
        "  -h          mostra esta ajuda";

    verificar_ajuda(argc, argv, ajuda_islash);

    /*
     * O shell IGNORA Ctrl+C - se o utilizador pressionar Ctrl+C
     * enquanto um comando externo corre, o sinal vai para esse
     * comando, nao para o shell.
     */
    signal(SIGINT, SIG_IGN);

    char  *linha = NULL;
    size_t cap   = 0;

    while (1) {
        mostrar_prompt();
        fflush(stdout);

        ssize_t lidos = getline(&linha, &cap, stdin);
        if (lidos == -1) {
            /* EOF (Ctrl+D) - sair limpo */
            printf("\n");
            break;
        }

        /* Remover '\n' final, se houver */
        if (lidos > 0 && linha[lidos - 1] == '\n') {
            linha[lidos - 1] = '\0';
        }

        /* Linha vazia: voltar ao prompt sem fazer nada */
        if (linha[0] == '\0') continue;

        char *tokens[MAX_TOKENS];
        int   n = tokenizar(linha, tokens, MAX_TOKENS);
        if (n == 0) continue;

        /* exit e' tratado aqui para sairmos do loop */
        if (strcmp(tokens[0], "exit") == 0) {
            break;
        }

        if (eh_builtin(tokens[0])) {
            executar_builtin(n, tokens);
        } else {
            executar_externo(tokens);
        }
    }

    free(linha);
    return 0;
}


/* ============================================================
 *  Prompt: username@hostname (cwd) >
 * ============================================================ */
static void mostrar_prompt(void) {
    const char *user = getenv("USER");
    if (user == NULL) user = "?";

    char hostname[HOSTNAME_LEN];
    if (gethostname(hostname, sizeof(hostname)) != 0) {
        strncpy(hostname, "?", sizeof(hostname));
        hostname[sizeof(hostname) - 1] = '\0';
    }

    char cwd[PATH_LEN];
    if (getcwd(cwd, sizeof(cwd)) == NULL) {
        strncpy(cwd, "?", sizeof(cwd));
        cwd[sizeof(cwd) - 1] = '\0';
    }

    printf("%s@%s (%s) > ", user, hostname, cwd);
}


/* ============================================================
 *  Tokenizacao: parte a linha em tokens separados por espaco/tab
 *  MODIFICA a linha original (insere '\0').
 * ============================================================ */
static int tokenizar(char *linha, char **tokens, int max_tokens) {
    int   n = 0;
    char *saveptr;
    char *tok = strtok_r(linha, " \t", &saveptr);
    while (tok != NULL && n < max_tokens - 1) {
        tokens[n++] = tok;
        tok = strtok_r(NULL, " \t", &saveptr);
    }
    tokens[n] = NULL;   /* terminator que o execvp exige */
    return n;
}


/* ============================================================
 *  Builtins
 * ============================================================ */
static int eh_builtin(const char *cmd) {
    return strcmp(cmd, "cd")    == 0
        || strcmp(cmd, "pwd")   == 0
        || strcmp(cmd, "echo")  == 0
        || strcmp(cmd, "sleep") == 0;
    /* exit e' tratado no main */
}

static void executar_builtin(int n, char **tokens) {
    const char *cmd = tokens[0];

    /* --- cd --- */
    if (strcmp(cmd, "cd") == 0) {
        const char *destino;
        if (n < 2) {
            /* 'cd' sem args -> HOME (comportamento standard de shells) */
            destino = getenv("HOME");
            if (destino == NULL) {
                fprintf(stderr, "cd: HOME nao definida\n");
                return;
            }
        } else {
            destino = tokens[1];
        }
        if (chdir(destino) != 0) {
            fprintf(stderr, "cd: %s: %s\n", destino, strerror(errno));
        }
        return;
    }

    /* --- pwd --- */
    if (strcmp(cmd, "pwd") == 0) {
        char cwd[PATH_LEN];
        if (getcwd(cwd, sizeof(cwd)) == NULL) {
            fprintf(stderr, "pwd: %s\n", strerror(errno));
            return;
        }
        printf("%s\n", cwd);
        return;
    }

    /* --- echo --- */
    if (strcmp(cmd, "echo") == 0) {
        for (int i = 1; i < n; i++) {
            if (i > 1) printf(" ");
            printf("%s", tokens[i]);
        }
        printf("\n");
        return;
    }

    /* --- sleep --- */
    if (strcmp(cmd, "sleep") == 0) {
        if (n < 2) {
            fprintf(stderr, "sleep: numero de segundos em falta\n");
            return;
        }
        char *end;
        long segundos = strtol(tokens[1], &end, 10);
        if (*end != '\0' || segundos < 0) {
            fprintf(stderr, "sleep: argumento invalido: %s\n", tokens[1]);
            return;
        }
        sleep((unsigned int)segundos);
        return;
    }
}


/* ============================================================
 *  Comandos externos: fork + execvp + waitpid
 * ============================================================ */
static void executar_externo(char **tokens) {

    pid_t pid = fork();

    if (pid < 0) {
        fprintf(stderr, "islash: fork falhou: %s\n", strerror(errno));
        return;
    }

    if (pid == 0) {
        /*
         * Processo FILHO.
         * Restaurar comportamento default do Ctrl+C - o shell ignora
         * mas o filho nao deve.
         */
        signal(SIGINT, SIG_DFL);

        /* Substituir este processo pelo programa pedido */
        execvp(tokens[0], tokens);

        /* Se execvp() retorna, e' porque falhou */
        fprintf(stderr, "islash: %s: %s\n", tokens[0], strerror(errno));
        exit(127);
    }

    /*
     * Processo PAI.
     * Esperar que o filho termine antes de mostrar novo prompt.
     */
    int status;
    if (waitpid(pid, &status, 0) < 0) {
        fprintf(stderr, "islash: waitpid: %s\n", strerror(errno));
    }
}