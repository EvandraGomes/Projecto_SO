#include "utilitarios_comuns.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>     /* unlink, access */
#include <errno.h>

static int apagar_ficheiro(const char *nome, int interativo);
static int confirmar(const char *nome);

int main(int argc, char *argv[]) {

    const char *ajuda_rm =
        "Uso: rm [OPCOES] FICHEIRO...\n"
        "Apaga ficheiros do sistema de ficheiros.\n"
        "\n"
        "Opcoes:\n"
        "  -y    modo interativo: pede confirmacao para cada ficheiro\n"
        "  -h    mostra esta ajuda";

    verificar_ajuda(argc, argv, ajuda_rm);

    int interativo = 0;
    int idx_primeiro_ficheiro = -1;

    /* Primeira passagem: separar flags de nomes de ficheiros */
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-y") == 0) {
            interativo = 1;
        }
        else if (argv[i][0] != '-') {
            idx_primeiro_ficheiro = i;
            break;
        }
        else {
            fprintf(stderr, "rm: opcao desconhecida: %s\n", argv[i]);
            return 1;
        }
    }

    if (idx_primeiro_ficheiro == -1) {
        fprintf(stderr, "rm: e preciso indicar pelo menos um ficheiro\n");
        return 1;
    }

    /* Apagar cada ficheiro - continuar mesmo que um falhe */
    int erros = 0;
    for (int i = idx_primeiro_ficheiro; i < argc; i++) {
        if (apagar_ficheiro(argv[i], interativo) != 0) {
            erros++;
        }
    }

    return (erros == 0) ? 0 : 1;
}

/*
 * Apaga um unico ficheiro.
 * Retorna 0 em sucesso (ou se o utilizador disse 'nao' em modo -y),
 * 1 em caso de erro (ficheiro nao existe ou unlink falhou).
 */
static int apagar_ficheiro(const char *nome, int interativo) {

    /* O briefing exige verificar previamente se o ficheiro existe */
    if (access(nome, F_OK) != 0) {
        fprintf(stderr, "rm: '%s': %s\n", nome, strerror(errno));
        return 1;
    }

    /* Modo interativo: pedir confirmacao */
    if (interativo) {
        if (!confirmar(nome)) {
            return 0;   /* nao apagar nao e' um erro */
        }
    }

    /* Apagar com unlink() */
    if (unlink(nome) != 0) {
        fprintf(stderr, "rm: nao foi possivel apagar '%s': %s\n",
                nome, strerror(errno));
        return 1;
    }

    return 0;
}

/*
 * Pergunta ao utilizador se quer apagar o ficheiro.
 * Retorna 1 para sim, 0 para qualquer outra resposta.
 */
static int confirmar(const char *nome) {
    printf("Apagar '%s'? (s/n): ", nome);
    fflush(stdout);

    int c = getchar();

    /* Consumir o resto da linha para nao baralhar a proxima pergunta */
    int extra = c;
    while (extra != '\n' && extra != EOF) {
        extra = getchar();
    }

    return (c == 's' || c == 'S' || c == 'y' || c == 'Y');
}