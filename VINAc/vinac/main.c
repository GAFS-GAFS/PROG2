#include <stdio.h>
#include <string.h>
#include "vinac.h"

typedef enum {
    CMD_INVALIDO,
    CMD_INSERT,
    CMD_INSERT_COMP,
    CMD_REMOVE,
    CMD_LIST,
    CMD_EXTRACT,
    CMD_MOVE
} Comando;

Comando identificar_comando(const char *cmd) {
    if (strcmp(cmd, "-ip") == 0) return CMD_INSERT;
    if (strcmp(cmd, "-ic") == 0) return CMD_INSERT_COMP;
    if (strcmp(cmd, "-r") == 0) return CMD_REMOVE;
    if (strcmp(cmd, "-c") == 0) return CMD_LIST;
    if (strcmp(cmd, "-x") == 0) return CMD_EXTRACT;
    if (strcmp(cmd, "-m") == 0) return CMD_MOVE;
    return CMD_INVALIDO;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Uso: vinac <comando> [opções]\n");
        return (1);
    }

    Comando cmd = identificar_comando(argv[1]);

    switch (cmd) {
        case CMD_INSERT:
            vinac_insert(argc, argv, 0);
            break;
        case CMD_INSERT_COMP:
            vinac_insert(argc, argv, 1);
            break;
        case CMD_REMOVE:
            vinac_remove(argc, argv);
            break;
        case CMD_LIST:
            vinac_list(argc, argv);
            break;
        case CMD_EXTRACT:
            vinac_extract(argc, argv);
            break;
        case CMD_MOVE:
            vinac_move(argc, argv);
            break;
        default:
            printf("Comando desconhecido: %s\n", argv[1]);
            printf("Comandos disponíveis: inserção plana(-ip), inserção comprimida(-ic), remove(-r), listar(-c), extrair(-x), move(-m)\n");
            return (1);
    }

    return (0);
}
