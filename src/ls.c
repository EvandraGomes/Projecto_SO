/*
 * ls.c - Lista o conteudo de um diretorio.
 * Sistemas Operativos - IADE 2025/2026
 */

#define _POSIX_C_SOURCE 200809L  /* lstat, strdup */
#define _DEFAULT_SOURCE          /* DT_DIR, etc. */

#include "utilitarios_comuns.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>

#define LARGURA_TERMINAL 80
#define TAMANHO_PATH 4096

/* Tipos de ordenacao */
typedef enum {
    ORD_NOME,
    ORD_TAMANHO,
    ORD_DATA
} Ordem;

/* Informacao guardada por cada entrada do diretorio */
typedef struct {
    char  *nome;
    off_t  tamanho;
    time_t mtime;
} Entrada;

/* Prototipos */
static int  ler_entradas(const char *caminho, Entrada **out, size_t *n);
static void ordenar_entradas(Entrada *arr, size_t n, Ordem ord);
static void imprimir_simples(const Entrada *arr, size_t n);
static void imprimir_longo(const Entrada *arr, size_t n);
static void imprimir_colunas(const Entrada *arr, size_t n);
static void libertar_entradas(Entrada *arr, size_t n);

static int cmp_nome(const void *a, const void *b);
static int cmp_tamanho(const void *a, const void *b);
static int cmp_data(const void *a, const void *b);


int main(int argc, char *argv[]) {

    const char *ajuda_ls =
        "Uso: ls [OPCOES] [CAMINHO]\n"
        "Lista o conteudo de um diretorio (default: diretorio actual).\n"
        "\n"
        "Opcoes:\n"
        "  -l        listagem longa (nome, tamanho, data)\n"
        "  -on       ordenar por nome (default)\n"
        "  -os       ordenar por tamanho\n"
        "  -od       ordenar por data\n"
        "  -c        listagem em colunas\n"
        "  -h        mostra esta ajuda";

    verificar_ajuda(argc, argv, ajuda_ls);

    int modo_longo   = 0;
    int modo_colunas = 0;
    Ordem ordem      = ORD_NOME;
    const char *caminho = ".";

    for (int i = 1; i < argc; i++) {
        if      (strcmp(argv[i], "-l")  == 0) modo_longo   = 1;
        else if (strcmp(argv[i], "-c")  == 0) modo_colunas = 1;
        else if (strcmp(argv[i], "-on") == 0) ordem = ORD_NOME;
        else if (strcmp(argv[i], "-os") == 0) ordem = ORD_TAMANHO;
        else if (strcmp(argv[i], "-od") == 0) ordem = ORD_DATA;
        else if (argv[i][0] != '-')           caminho = argv[i];
        else {
            fprintf(stderr, "ls: opcao desconhecida: %s\n", argv[i]);
            return 1;
        }
    }

    Entrada *entradas = NULL;
    size_t   n        = 0;

    if (ler_entradas(caminho, &entradas, &n) != 0) {
        return 1;
    }

    ordenar_entradas(entradas, n, ordem);

    if (modo_longo)        imprimir_longo(entradas, n);
    else if (modo_colunas) imprimir_colunas(entradas, n);
    else                   imprimir_simples(entradas, n);

    libertar_entradas(entradas, n);
    return 0;
}


/* ============================================================
 *  Leitura do diretorio
 * ============================================================ */

static int ler_entradas(const char *caminho, Entrada **out, size_t *n) {

    DIR *dir = opendir(caminho);
    if (dir == NULL) {
        fprintf(stderr, "ls: nao consegui abrir '%s': %s\n",
                caminho, strerror(errno));
        return 1;
    }

    Entrada *arr    = NULL;
    size_t  total   = 0;
    size_t  capacid = 0;

    struct dirent *entrada;
    while ((entrada = readdir(dir)) != NULL) {

        /* Ignorar ficheiros escondidos (incluindo "." e "..") */
        if (entrada->d_name[0] == '.') continue;

        /* Crescer o array se necessario */
        if (total == capacid) {
            capacid = (capacid == 0) ? 16 : capacid * 2;
            Entrada *temp = realloc(arr, capacid * sizeof(Entrada));
            if (temp == NULL) {
                fprintf(stderr, "ls: sem memoria\n");
                libertar_entradas(arr, total);
                closedir(dir);
                return 1;
            }
            arr = temp;
        }

        /* Construir o caminho completo para stat */
        char path[TAMANHO_PATH];
        snprintf(path, sizeof(path), "%s/%s", caminho, entrada->d_name);

        struct stat st;
        if (lstat(path, &st) != 0) {
            /* Se nao conseguirmos fazer stat, ainda assim listamos o nome */
            arr[total].tamanho = 0;
            arr[total].mtime   = 0;
        } else {
            arr[total].tamanho = st.st_size;
            arr[total].mtime   = st.st_mtime;
        }

        arr[total].nome = strdup(entrada->d_name);
        if (arr[total].nome == NULL) {
            fprintf(stderr, "ls: sem memoria\n");
            libertar_entradas(arr, total);
            closedir(dir);
            return 1;
        }

        total++;
    }

    closedir(dir);

    *out = arr;
    *n   = total;
    return 0;
}


/* ============================================================
 *  Ordenacao
 * ============================================================ */

static void ordenar_entradas(Entrada *arr, size_t n, Ordem ord) {
    int (*cmp)(const void *, const void *) = cmp_nome;

    switch (ord) {
        case ORD_NOME:    cmp = cmp_nome;    break;
        case ORD_TAMANHO: cmp = cmp_tamanho; break;
        case ORD_DATA:    cmp = cmp_data;    break;
    }
    qsort(arr, n, sizeof(Entrada), cmp);
}

static int cmp_nome(const void *a, const void *b) {
    const Entrada *ea = (const Entrada *)a;
    const Entrada *eb = (const Entrada *)b;
    return strcmp(ea->nome, eb->nome);
}

static int cmp_tamanho(const void *a, const void *b) {
    const Entrada *ea = (const Entrada *)a;
    const Entrada *eb = (const Entrada *)b;
    if (ea->tamanho < eb->tamanho) return -1;
    if (ea->tamanho > eb->tamanho) return  1;
    return 0;
}

static int cmp_data(const void *a, const void *b) {
    const Entrada *ea = (const Entrada *)a;
    const Entrada *eb = (const Entrada *)b;
    if (ea->mtime < eb->mtime) return -1;
    if (ea->mtime > eb->mtime) return  1;
    return 0;
}


/* ============================================================
 *  Impressao
 * ============================================================ */

static void imprimir_simples(const Entrada *arr, size_t n) {
    for (size_t i = 0; i < n; i++) {
        printf("%s\n", arr[i].nome);
    }
}

static void imprimir_longo(const Entrada *arr, size_t n) {
    for (size_t i = 0; i < n; i++) {
        char data[32];
        struct tm *tm = localtime(&arr[i].mtime);
        if (tm != NULL) {
            strftime(data, sizeof(data), "%Y-%m-%d %H:%M", tm);
        } else {
            strncpy(data, "?", sizeof(data));
        }
        printf("%10lld  %s  %s\n",
               (long long)arr[i].tamanho,
               data,
               arr[i].nome);
    }
}

static void imprimir_colunas(const Entrada *arr, size_t n) {
    if (n == 0) return;

    /* Calcular largura da coluna (maior nome + 2 espacos) */
    size_t max_len = 0;
    for (size_t i = 0; i < n; i++) {
        size_t l = strlen(arr[i].nome);
        if (l > max_len) max_len = l;
    }
    size_t col_width = max_len + 2;

    /* Quantas colunas cabem em LARGURA_TERMINAL */
    size_t n_cols = LARGURA_TERMINAL / col_width;
    if (n_cols == 0) n_cols = 1;

    size_t n_rows = (n + n_cols - 1) / n_cols;  /* arredondamento para cima */

    /* Imprimir em ordem column-major (desce, depois passa para a coluna seguinte) */
    for (size_t row = 0; row < n_rows; row++) {
        for (size_t col = 0; col < n_cols; col++) {
            size_t idx = col * n_rows + row;
            if (idx < n) {
                printf("%-*s", (int)col_width, arr[idx].nome);
            }
        }
        printf("\n");
    }
}


/* ============================================================
 *  Cleanup
 * ============================================================ */

static void libertar_entradas(Entrada *arr, size_t n) {
    if (arr == NULL) return;
    for (size_t i = 0; i < n; i++) {
        free(arr[i].nome);
    }
    free(arr);
}