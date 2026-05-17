/*
 * tail.c - Lista as ultimas linhas de um ficheiro de texto.
 * Sistemas Operativos - IADE 2025/2026
 *
 * Estrategia: 2 passagens.
 *   1a passagem: contar quantas linhas tem o ficheiro
 *   2a passagem: saltar as primeiras (total - N) e imprimir o resto
 */

#include "utilitarios_comuns.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

static void processar_ficheiro(const char *nome, int linhas_a_mostrar,
                               int numerar, int marcar_fim);
static int contar_linhas(const char *nome);


int main(int argc, char *argv[]) {

    const char *ajuda_tail =
        "Uso: tail [OPCOES] [FICHEIRO]...\n"
        "Lista as ultimas linhas de um ficheiro de texto (default: 10).\n"
        "\n"
        "Opcoes:\n"
        "  -NUM    mostra as ultimas NUM linhas (ex: -5)\n"
        "  -n      mostra o numero de cada linha (posicao real no ficheiro)\n"
        "  -E      marca o fim de cada linha com '$'\n"
        "  -h      mostra esta ajuda";

    verificar_ajuda(argc, argv, ajuda_tail);

    int linhas_a_mostrar = 10;
    int numerar = 0;
    int marcar_fim = 0;
    int idx_primeiro_ficheiro = -1;

    /* Separar flags de nomes de ficheiros (mesma logica do head) */
    for (int i = 1; i < argc; i++) {
        if (argv[i][0] == '-' && isdigit((unsigned char)argv[i][1])) {
            linhas_a_mostrar = atoi(argv[i] + 1);
            if (linhas_a_mostrar <= 0) {
                fprintf(stderr, "tail: numero invalido: %s\n", argv[i]);
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
            fprintf(stderr, "tail: opcao desconhecida: %s\n", argv[i]);
            return 1;
        }
    }

    if (idx_primeiro_ficheiro == -1) {
        fprintf(stderr, "tail: e preciso indicar pelo menos um ficheiro\n");
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


/*
 * Conta o numero total de linhas de um ficheiro.
 * Retorna -1 em caso de erro a abrir.
 */
static int contar_linhas(const char *nome) {
    FILE *fp = fopen(nome, "r");
    if (fp == NULL) return -1;

    char buffer[1024];
    int contador = 0;
    while (fgets(buffer, sizeof(buffer), fp) != NULL) {
        contador++;
    }

    fclose(fp);
    return contador;
}


/*
 * Le um ficheiro e imprime as ultimas N linhas, respeitando as flags.
 */
static void processar_ficheiro(const char *nome, int linhas_a_mostrar,
                               int numerar, int marcar_fim) {

    /* --- Primeira passagem: contar linhas --- */
    int total = contar_linhas(nome);
    if (total < 0) {
        tratar_erro("nao consegui abrir o ficheiro");
    }

    /* Quantas saltar? Se o ficheiro tem menos que NUM, nao saltamos nenhuma */
    int saltar = total - linhas_a_mostrar;
    if (saltar < 0) saltar = 0;

    /* --- Segunda passagem: reabrir e imprimir as ultimas --- */
    FILE *fp = fopen(nome, "r");
    if (fp == NULL) tratar_erro("nao consegui abrir o ficheiro");

    char linha[1024];
    int contador = 0;

    while (fgets(linha, sizeof(linha), fp) != NULL) {
        contador++;
        if (contador <= saltar) continue;  /* saltar as primeiras */

        /* Numero da linha = posicao REAL no ficheiro (16, 17, 18...) */
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