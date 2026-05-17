#include "../include/utilitarios_comuns.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void verificar_ajuda(int argc, char *argv[], const char *msg_de_ajuda) {
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-h") == 0) {
            printf("%s\n", msg_de_ajuda);
            exit(0);
        }
    }
}

void tratar_erro(const char *msg_de_erro) {
    perror(msg_de_erro);
    exit(1);
}