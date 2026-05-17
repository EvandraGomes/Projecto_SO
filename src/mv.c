/*
 * mv.c - Move ou renomeia um ficheiro.
 * Sistemas Operativos - IADE 2025/2026
 *
 * Estrategia:
 *   1. Tenta rename() (rapido, atomico, no mesmo filesystem).
 *   2. Se falhar com EXDEV (cross-filesystem), faz copia + remocao.
 */

#define _POSIX_C_SOURCE 200809L

#include "utilitarios_comuns.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>     /* unlink, access */
#include <errno.h>

static int confirmar_sobrescrever(const char *destino);
static int copiar_ficheiro(const char *origem, const char *destino);


int main(int argc, char *argv[]) {

    const char *ajuda_mv =
        "Uso: mv [OPCOES] ORIGEM DESTINO\n"
        "Move ou renomeia um ficheiro.\n"
        "\n"
        "Opcoes:\n"
        "  -i    modo interativo: pergunta antes de sobrescrever o destino\n"
        "  -h    mostra esta ajuda";

    verificar_ajuda(argc, argv, ajuda_mv);

    int interativo  = 0;
    const char *origem  = NULL;
    const char *destino = NULL;

    /* Parsing: separar flags de argumentos posicionais */
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-i") == 0) {
            interativo = 1;
        }
        else if (argv[i][0] != '-') {
            if      (origem  == NULL) origem  = argv[i];
            else if (destino == NULL) destino = argv[i];
            else {
                fprintf(stderr, "mv: argumentos a mais\n");
                return 1;
            }
        }
        else {
            fprintf(stderr, "mv: opcao desconhecida: %s\n", argv[i]);
            return 1;
        }
    }

    if (origem == NULL || destino == NULL) {
        fprintf(stderr, "Uso: mv [OPCOES] ORIGEM DESTINO\n");
        return 1;
    }

    /* Verificar que a origem existe */
    if (access(origem, F_OK) != 0) {
        fprintf(stderr, "mv: '%s': %s\n", origem, strerror(errno));
        return 1;
    }

    /* Se o destino ja existe, decidir o que fazer */
    if (access(destino, F_OK) == 0) {
        if (interativo) {
            if (!confirmar_sobrescrever(destino)) {
                return 0;   /* utilizador disse 'nao' - nao e' erro */
            }
        }
        /* Sem -i: sobrescreve silenciosamente (e' o comportamento default do mv real) */
    }

    /* Tentar rename - eficiente e atomico no mesmo filesystem */
    if (rename(origem, destino) == 0) {
        return 0;
    }

    /* Caso especial: filesystems diferentes -> copiar + apagar */
    if (errno == EXDEV) {
        if (copiar_ficheiro(origem, destino) != 0) {
            return 1;
        }
        if (unlink(origem) != 0) {
            fprintf(stderr, "mv: nao foi possivel remover '%s': %s\n",
                    origem, strerror(errno));
            return 1;
        }
        return 0;
    }

    /* Qualquer outro erro do rename */
    fprintf(stderr, "mv: nao foi possivel mover '%s' para '%s': %s\n",
            origem, destino, strerror(errno));
    return 1;
}


/*
 * Pergunta ao utilizador se quer sobrescrever o destino.
 * Retorna 1 para sim, 0 para qualquer outra resposta.
 */
static int confirmar_sobrescrever(const char *destino) {
    printf("mv: sobrescrever '%s'? (s/n): ", destino);
    fflush(stdout);

    int c = getchar();
    int extra = c;
    while (extra != '\n' && extra != EOF) {
        extra = getchar();
    }

    return (c == 's' || c == 'S' || c == 'y' || c == 'Y');
}


/*
 * Copia origem para destino, byte a byte.
 * Usado como fallback quando rename() falha com EXDEV.
 */
static int copiar_ficheiro(const char *origem, const char *destino) {
    FILE *fin = fopen(origem, "rb");
    if (fin == NULL) {
        fprintf(stderr, "mv: nao consegui abrir '%s': %s\n",
                origem, strerror(errno));
        return 1;
    }

    FILE *fout = fopen(destino, "wb");
    if (fout == NULL) {
        fprintf(stderr, "mv: nao consegui criar '%s': %s\n",
                destino, strerror(errno));
        fclose(fin);
        return 1;
    }

    char buffer[4096];
    size_t lidos;
    while ((lidos = fread(buffer, 1, sizeof(buffer), fin)) > 0) {
        if (fwrite(buffer, 1, lidos, fout) != lidos) {
            fprintf(stderr, "mv: erro ao escrever em '%s': %s\n",
                    destino, strerror(errno));
            fclose(fin);
            fclose(fout);
            return 1;
        }
    }

    if (ferror(fin)) {
        fprintf(stderr, "mv: erro ao ler '%s'\n", origem);
        fclose(fin);
        fclose(fout);
        return 1;
    }

    fclose(fin);
    fclose(fout);
    return 0;
}