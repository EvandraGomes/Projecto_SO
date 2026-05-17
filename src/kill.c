/*
 * kill.c - Termina um processo ativo enviando-lhe SIGTERM.
 * Sistemas Operativos - IADE 2025/2026
 */

#define _POSIX_C_SOURCE 200809L  /* necessario para kill() com -std=c99 */

#include "utilitarios_comuns.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <ctype.h>
#include <sys/types.h>


int main(int argc, char *argv[]) {

    const char *ajuda_kill =
        "Uso: kill PID\n"
        "Termina um processo ativo enviando-lhe o sinal SIGTERM.\n"
        "\n"
        "Opcoes:\n"
        "  -h    mostra esta ajuda";

    verificar_ajuda(argc, argv, ajuda_kill);

    /* O briefing diz: 'Sintaxe: kill PID' - exatamente um argumento */
    if (argc != 2) {
        fprintf(stderr, "Uso: kill PID\n");
        return 1;
    }

    const char *arg = argv[1];

    /* Validar que o argumento e' so digitos (PID e' um inteiro positivo) */
    for (size_t i = 0; arg[i] != '\0'; i++) {
        if (!isdigit((unsigned char)arg[i])) {
            fprintf(stderr, "kill: PID invalido: '%s'\n", arg);
            return 1;
        }
    }

    /* Converter para long e validar */
    char *end;
    errno = 0;
    long pid_val = strtol(arg, &end, 10);

    if (*end != '\0' || errno != 0 || pid_val <= 0) {
        fprintf(stderr, "kill: PID invalido: '%s'\n", arg);
        return 1;
    }

    /* Enviar SIGTERM ao processo */
    if (kill((pid_t)pid_val, SIGTERM) != 0) {
        fprintf(stderr, "kill: nao foi possivel terminar PID %ld: %s\n",
                pid_val, strerror(errno));
        return 1;
    }

    return 0;
}