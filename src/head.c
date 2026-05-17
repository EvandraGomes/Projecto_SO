#include "utilitarios_comuns.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

static void processar_ficheiro(const char *nome, int linhas_a_mostrar,
                               int numerar, int marcar_fim);

int main(int argc, char *argv[]) {

    const char *ajuda_head =
        "Uso: head [OPCOES] [FICHEIRO]...\n"
        "Lista as primeiras linhas de um ficheiro de texto (default: 10).\n"
        "\n"
        "Opcoes:\n"
        "  -NUM    mostra as primeiras NUM linhas (ex: -5)\n"
        "  -n      mostra o numero de cada linha\n"
        "  -E      marca o fim de cada linha com '$'\n"
        "  -h      mostra esta ajuda";

    verificar_ajuda(argc, argv, ajuda_head);

    int linhas_a_mostrar = 10;
    int numerar = 0;
    int marcar_fim = 0;
    int idx_primeiro_ficheiro = -1;

    /* primeira passagem: separar flags de ficheiros */
    for (int i = 1; i < argc; i++) {
        if (argv[i][0] == '-' && isdigit((unsigned char)argv[i][1])) {
            linhas_a_mostrar = atoi(argv[i] + 1);
            if (linhas_a_mostrar <= 0) {
                fprintf(stderr, "head: numero invalido: %s\n", argv[i]);
                return 1;
            }
        }
        else if (strcmp(argv[i], "-n") == 0) {
            numerar = 1;
        }
        else if (strcmp(argv[i], "-E") == 0) {
            marcar_fim = 1;
        }
        else if (argv[i][0] != '-') {
            idx_primeiro_ficheiro = i;
            break;
        }
        else {
            fprintf(stderr, "head: opcao desconhecida: %s\n", argv[i]);
            return 1;
        }
    }

    if (idx_primeiro_ficheiro == -1) {
        fprintf(stderr, "head: e preciso indicar pelo menos um ficheiro\n");
        return 1;
    }

    int multiplos = (argc - idx_primeiro_ficheiro) > 1;
    for (int i = idx_primeiro_ficheiro; i < argc; i++) {
        if (multiplos) printf("==> %s <==\n", argv[i]);
        processar_ficheiro(argv[i], linhas_a_mostrar, numerar, marcar_fim);
        if (multiplos && i < argc - 1) printf("\n");
    }

    return 0;
}

static void processar_ficheiro(const char *nome, int linhas_a_mostrar,
                               int numerar, int marcar_fim) {
    FILE *fp = fopen(nome, "r");
    if (fp == NULL) tratar_erro("nao consegui abrir o ficheiro");

    char linha[1024];
    int contador = 0;

    while (contador < linhas_a_mostrar &&
           fgets(linha, sizeof(linha), fp) != NULL) {
        contador++;

        if (numerar) printf("%d  ", contador);

        if (marcar_fim) {
            size_t len = strlen(linha);
            if (len > 0 && linha[len - 1] == '\n') {
                linha[len - 1] = '\0';
                printf("%s$\n", linha);
            } else {
                printf("%s$\n", linha);
            }
        } else {
            printf("%s", linha);
        }
    }

    fclose(fp);
}