/*
 * grep.c - Procura strings dentro de ficheiros.
 * Sistemas Operativos - IADE 2025/2026
 */

#define _POSIX_C_SOURCE 200809L  /* strncasecmp */

#include "utilitarios_comuns.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>    /* strncasecmp */
#include <errno.h>

static int processar_ficheiro(const char *nome, const char *padrao,
                              int mostrar_contagem, int numerar,
                              int ignorar_case, int inverter,
                              int multiplos_ficheiros);
static int linha_contem(const char *haystack, const char *needle, int ignorar_case);


int main(int argc, char *argv[]) {

    const char *ajuda_grep =
        "Uso: grep [OPCOES] STRING FICHEIROS...\n"
        "Procura uma string dentro de ficheiros e mostra as linhas que a contem.\n"
        "\n"
        "Opcoes:\n"
        "  -c    mostra apenas o numero de ocorrencias por ficheiro\n"
        "  -n    prefixa cada linha com o seu numero, separado por ':'\n"
        "  -i    ignora maiusculas e minusculas\n"
        "  -v    inverte: mostra linhas que NAO contem a string\n"
        "  -h    mostra esta ajuda";

    verificar_ajuda(argc, argv, ajuda_grep);

    int mostrar_contagem = 0;
    int numerar          = 0;
    int ignorar_case     = 0;
    int inverter         = 0;
    const char *padrao   = NULL;
    int idx_primeiro_ficheiro = -1;

    /*
     * Parsing: as flags vem primeiro, depois o padrao (1o nao-flag),
     * depois um ou mais ficheiros.
     */
    for (int i = 1; i < argc; i++) {
        if      (strcmp(argv[i], "-c") == 0) mostrar_contagem = 1;
        else if (strcmp(argv[i], "-n") == 0) numerar          = 1;
        else if (strcmp(argv[i], "-i") == 0) ignorar_case     = 1;
        else if (strcmp(argv[i], "-v") == 0) inverter         = 1;
        else if (argv[i][0] != '-') {
            if (padrao == NULL) {
                padrao = argv[i];
            } else {
                idx_primeiro_ficheiro = i;
                break;
            }
        }
        else {
            fprintf(stderr, "grep: opcao desconhecida: %s\n", argv[i]);
            return 1;
        }
    }

    if (padrao == NULL) {
        fprintf(stderr, "grep: e preciso indicar a string a procurar\n");
        return 1;
    }
    if (idx_primeiro_ficheiro == -1) {
        fprintf(stderr, "grep: e preciso indicar pelo menos um ficheiro\n");
        return 1;
    }

    int multiplos = (argc - idx_primeiro_ficheiro) > 1;
    int erros = 0;

    for (int i = idx_primeiro_ficheiro; i < argc; i++) {
        if (processar_ficheiro(argv[i], padrao, mostrar_contagem,
                               numerar, ignorar_case, inverter, multiplos) != 0) {
            erros++;
        }
    }

    return (erros == 0) ? 0 : 1;
}


/*
 * Devolve 1 se 'needle' aparece em 'haystack', 0 caso contrario.
 * Se ignorar_case == 1, compara sem distinguir maiusculas/minusculas.
 */
static int linha_contem(const char *haystack, const char *needle, int ignorar_case) {

    if (!ignorar_case) {
        /* Caso normal: strstr da stdlib */
        return strstr(haystack, needle) != NULL;
    }

    /* Caso insensitivo: percorrer manualmente com strncasecmp */
    size_t nlen = strlen(needle);
    if (nlen == 0) return 1;   /* needle vazio "encontra-se" em qualquer string */

    for (size_t i = 0; haystack[i] != '\0'; i++) {
        if (strncasecmp(haystack + i, needle, nlen) == 0) {
            return 1;
        }
    }
    return 0;
}


/*
 * Processa um ficheiro - aplica todas as flags na ordem certa.
 */
static int processar_ficheiro(const char *nome, const char *padrao,
                              int mostrar_contagem, int numerar,
                              int ignorar_case, int inverter,
                              int multiplos_ficheiros) {

    FILE *fp = fopen(nome, "r");
    if (fp == NULL) {
        fprintf(stderr, "grep: '%s': %s\n", nome, strerror(errno));
        return 1;
    }

    char linha[4096];
    int  n_linha  = 0;
    int  contagem = 0;

    while (fgets(linha, sizeof(linha), fp) != NULL) {
        n_linha++;

        int encontrou    = linha_contem(linha, padrao, ignorar_case);
        int deve_mostrar = inverter ? !encontrou : encontrou;

        if (!deve_mostrar) continue;

        contagem++;

        if (!mostrar_contagem) {
            /* Imprimir a linha (com prefixos consoante as flags) */
            if (multiplos_ficheiros) printf("%s:", nome);
            if (numerar)             printf("%d:", n_linha);
            printf("%s", linha);

            /* Se a ultima linha do ficheiro nao tinha '\n', acrescenta */
            size_t len = strlen(linha);
            if (len == 0 || linha[len - 1] != '\n') printf("\n");
        }
    }

    fclose(fp);

    /* Com -c, mostra a contagem (uma linha por ficheiro) */
    if (mostrar_contagem) {
        if (multiplos_ficheiros) printf("%s:%d\n", nome, contagem);
        else                     printf("%d\n", contagem);
    }

    return 0;
}