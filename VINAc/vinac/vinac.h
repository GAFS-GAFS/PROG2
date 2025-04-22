// vinac.h
#ifndef VINAC_H
#define VINAC_H

#include "diretorio.h"

// Insere arquivos no arquivador .vc.
// argc Quantidade de argumentos da linha de comando.
// argv Vetor de argumentos da linha de comando.
// compress Define se os arquivos devem ser comprimidos (1) ou não (0).

void vinac_insert(int argc, char *argv[], int compress);


// Remove um ou mais arquivos do arquivador .vc.
// Quantidade de argumentos da linha de comando.
// Vetor de argumentos da linha de comando.

void vinac_remove(int argc, char *argv[]);


// Lista todos os arquivos presentes no arquivador .vc.
// Quantidade de argumentos da linha de comando.
// Vetor de argumentos da linha de comando.

void vinac_list(int argc, char *argv[]);


// Extrai um ou mais arquivos do arquivador .vc para o sistema de arquivos.
// Quantidade de argumentos da linha de comando.
// Vetor de argumentos da linha de comando.

void vinac_extract(int argc, char *argv[]);


// Move a posição lógica de um arquivo dentro do arquivador (por exemplo, muda a ordem no índice).
// Quantidade de argumentos da linha de comando.
// Vetor de argumentos da linha de comando.

void vinac_move(int argc, char *argv[]);

#endif
