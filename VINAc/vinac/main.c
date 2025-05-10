#include <stdio.h>
#include <string.h>
#include "vinac.h"

// Enumeração que define os possíveis comandos do programa
typedef enum {
    CMD_INVALIDO,         // Comando inválido ou não reconhecido
    CMD_INSERT,           // Inserir arquivo sem compressão (-ip)
    CMD_INSERT_COMP,      // Inserir arquivo com compressão (-ic)
    CMD_REMOVE,           // Remover arquivo (-r)
    CMD_LIST,            // Listar conteúdo do arquivo (-c)
    CMD_EXTRACT,         // Extrair arquivo (-x)
    CMD_MOVE             // Mover arquivo dentro do índice (-m)
} Comando;

// Identifica qual comando foi passado na linha de comando
// cmd: string contendo o comando (primeiro argumento)
// Retorna: o comando correspondente da enumeração Comando
Comando identificar_comando(const char *cmd) {
    if (strcmp(cmd, "-ip") == 0 || strcmp(cmd, "-p") == 0) return CMD_INSERT;
    if (strcmp(cmd, "-ic") == 0 || strcmp(cmd, "-i") == 0) return CMD_INSERT_COMP;
    if (strcmp(cmd, "-r") == 0) return CMD_REMOVE;
    if (strcmp(cmd, "-c") == 0) return CMD_LIST;
    if (strcmp(cmd, "-x") == 0) return CMD_EXTRACT;
    if (strcmp(cmd, "-m") == 0) return CMD_MOVE;
    return CMD_INVALIDO;
}

// Função principal do programa
// Analisa os argumentos da linha de comando e executa a operação correspondente
int main(int argc, char *argv[]) {
    // Verifica se há argumentos suficientes
    if (argc < 2) {
        printf("Uso: vinac <comando> [opções]\n");
        return (1);
    }

    // Identifica o comando passado
    Comando cmd = identificar_comando(argv[1]);

    // Executa a operação correspondente ao comando
    switch (cmd) {
        case CMD_INSERT:
            vinac_insert(argc, argv, (uint32_t)0);
            break;
        case CMD_INSERT_COMP:
            vinac_insert_comp(argc, argv);
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
