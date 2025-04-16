#ifndef DIRETORIO_H
#define DIRETORIO_H

#include "arquivo.h"

typedef struct {
    ArquivoMembro* membros; // Lista de arquivos no .vc
    int quantidade;         // Quantos membros tem
    int capacidade;         // Para alocar mais
} IndiceArquivador;

void inicializarIndice(IndiceArquivador* idx);
void destruirIndice(IndiceArquivador* idx);
void adicionarAoIndice(IndiceArquivador* idx, ArquivoMembro membro);
int carregarIndice(FILE* vc, IndiceArquivador* idx);  // Lê do .vc
int salvarIndice(FILE* vc, IndiceArquivador* idx);    // Escreve no .vc
void listarArquivos(const IndiceArquivador* idx);

#endif
