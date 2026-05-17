/*
 * sort.c - Ordena ficheiros de texto e grava em FICHEIRO.sort
 * Sistemas Operativos - IADE 2025/2026
 */

#define _POSIX_C_SOURCE 200809L  /* necessario para getline() */

#include "utilitarios_comuns.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

static int processar_ficheiro(const char *nome, int decrescente);
static int comparar_crescente(const void *a, const void *b);
static int comparar_decrescente(const void *a, const void *b);


int main(int argc, char *argv[]) {

    const char *ajuda_sort =
        "Uso: sort [OPCOES] FICHEIRO...\n"
        "Ordena cada ficheiro de texto alfabeticamente.\n"
        "O resultado e' gravado num novo ficheiro com sufixo '.sort'.\n"
        "Em caso de sucesso, nao produz output no ecra.\n"
        "\n"
        "Opcoes:\n"
        "  -d    ordenacao decrescente (default: crescente)\n"
        "  -h    mostra esta ajuda";

    verificar_ajuda(argc, argv, ajuda_sort);

    int decrescente = 0;
    int idx_primeiro_ficheiro = -1;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-d") == 0) {
            decrescente = 1;
        }
        else if (argv[i][0] != '-') {
            idx_primeiro_ficheiro = i;
            break;
        }
        else {
            fprintf(stderr, "sort: opcao desconhecida: %s\n", argv[i]);
            return 1;
        }
    }

    if (idx_primeiro_ficheiro == -1) {
        fprintf(stderr, "sort: e preciso indicar pelo menos um ficheiro\n");
        return 1;
    }

    int erros = 0;
    for (int i = idx_primeiro_ficheiro; i < argc; i++) {
        if (processar_ficheiro(argv[i], decrescente) != 0) {
            erros++;
        }
    }

    return (erros == 0) ? 0 : 1;
}


/*
 * Ordena um ficheiro e grava o resultado em <nome>.sort
 * Retorna 0 em sucesso, 1 em erro.
 */
static int processar_ficheiro(const char *nome, int decrescente) {

    /* --- 1. Abrir ficheiro de entrada --- */
    FILE *fp = fopen(nome, "r");
    if (fp == NULL) {
        fprintf(stderr, "sort: '%s': %s\n", nome, strerror(errno));
        return 1;
    }

    /* --- 2. Ler todas as linhas para um array dinamico --- */
    char **linhas    = NULL;
    size_t n_linhas  = 0;
    size_t capacidade = 0;

    char  *buffer   = NULL;
    size_t buf_size = 0;

    while (getline(&buffer, &buf_size, fp) != -1) {

        /* Crescer o array se cheio (estrategia: duplicar capacidade) */
        if (n_linhas == capacidade) {
            capacidade = (capacidade == 0) ? 16 : capacidade * 2;
            char **temp = realloc(linhas, capacidade * sizeof(char *));
            if (temp == NULL) {
                fprintf(stderr, "sort: sem memoria\n");
                free(buffer);
                for (size_t j = 0; j < n_linhas; j++) free(linhas[j]);
                free(linhas);
                fclose(fp);
                return 1;
            }
            linhas = temp;
        }

        /* Duplicar a string (getline reutiliza o mesmo buffer interno) */
        linhas[n_linhas] = strdup(buffer);
        if (linhas[n_linhas] == NULL) {
            fprintf(stderr, "sort: sem memoria\n");
            free(buffer);
            for (size_t j = 0; j < n_linhas; j++) free(linhas[j]);
            free(linhas);
            fclose(fp);
            return 1;
        }
        n_linhas++;
    }

    free(buffer);
    fclose(fp);

    /* --- 3. Ordenar com qsort da stdlib --- */
    qsort(linhas, n_linhas, sizeof(char *),
          decrescente ? comparar_decrescente : comparar_crescente);

    /* --- 4. Construir nome do ficheiro de saida (<nome>.sort) --- */
    size_t len = strlen(nome);
    char *nome_saida = malloc(len + 6);   /* ".sort" + '\0' = 6 chars */
    if (nome_saida == NULL) {
        fprintf(stderr, "sort: sem memoria\n");
        for (size_t j = 0; j < n_linhas; j++) free(linhas[j]);
        free(linhas);
        return 1;
    }
    strcpy(nome_saida, nome);
    strcat(nome_saida, ".sort");

    /* --- 5. Escrever no ficheiro de saida --- */
    FILE *out = fopen(nome_saida, "w");
    if (out == NULL) {
        fprintf(stderr, "sort: nao consegui criar '%s': %s\n",
                nome_saida, strerror(errno));
        free(nome_saida);
        for (size_t j = 0; j < n_linhas; j++) free(linhas[j]);
        free(linhas);
        return 1;
    }

    for (size_t i = 0; i < n_linhas; i++) {
        fputs(linhas[i], out);
        /* Garantir newline no fim, caso a linha original nao tivesse */
        size_t l = strlen(linhas[i]);
        if (l > 0 && linhas[i][l - 1] != '\n') {
            fputc('\n', out);
        }
    }

    fclose(out);

    /* --- 6. Libertar memoria --- */
    free(nome_saida);
    for (size_t i = 0; i < n_linhas; i++) free(linhas[i]);
    free(linhas);

    return 0;
}


/* Comparador para qsort: ordem crescente (A -> Z) */
static int comparar_crescente(const void *a, const void *b) {
    const char *str_a = *(const char *const *)a;
    const char *str_b = *(const char *const *)b;
    return strcmp(str_a, str_b);
}

/* Comparador para qsort: ordem decrescente (Z -> A) - argumentos trocados */
static int comparar_decrescente(const void *a, const void *b) {
    const char *str_a = *(const char *const *)a;
    const char *str_b = *(const char *const *)b;
    return strcmp(str_b, str_a);
}