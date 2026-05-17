#include "../include/utilitarios_comuns.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {

    const char *ajuda_head = "Para usar: ./head [N de] [FICHEIRO]\n "
    "\n\"Opções:\n\" \"-n NUM\n\" \"-h\n\";";

    verificar_ajuda(argc, argv, ajuda_head);

    int linhas_a_mostrar = 10;
    char *nome_ficheiro = NULL;

    if (argc == 2) {
        nome_ficheiro = argv[1];
    }
    else if (argc == 4 && strcmp(argv[1], "-n") == 0) {
        linhas_a_mostrar = atoi(argv[2]);
        nome_ficheiro = argv[3];
    }
    else {
        printf("%s\n", ajuda_head);
        return 1;
    }

    FILE *ficheiro = fopen(nome_ficheiro, "r");
    if (ficheiro == NULL) {
        tratar_erro("Erro ao abrir o ficheiro");
    }

    char linha[1024];
    int contador = 0;

    while (contador < linhas_a_mostrar && fgets(linha, sizeof(linha), ficheiro) != NULL) {
        printf("%s", linha);
        contador++;
    }
}