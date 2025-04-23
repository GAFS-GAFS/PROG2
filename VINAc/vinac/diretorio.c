// filepath: /home/gabriel/Área de Trabalho/PROG2/VINAc/vinac/diretorio.c
#include "diretorio.h"
#include <stdio.h>
#include <stdlib.h>

void inicializarIndice(IndiceArquivador* idx) {
    idx->membros = NULL;
    idx->quantidade = 0;
    idx->capacidade = 0;
}

void destruirIndice(IndiceArquivador* idx) {
    free(idx->membros);
    idx->membros = NULL;
    idx->quantidade = 0;
    idx->capacidade = 0;
}

void adicionarAoIndice(IndiceArquivador* idx, ArquivoMembro membro) {
    printf("Stub: adicionarAoIndice chamado.\n");
}

int carregarIndice(FILE* vc, IndiceArquivador* idx) {
    printf("Stub: carregarIndice chamado.\n");

    // Simula a presença de dois arquivos no índice
    idx->quantidade = 2;
    idx->capacidade = 2;
    idx->membros = malloc(2 * sizeof(ArquivoMembro));

    strcpy(idx->membros[0].nome, "arquivo1.txt");
    idx->membros[0].ordem = 0;

    strcpy(idx->membros[1].nome, "arquivo2.txt");
    idx->membros[1].ordem = 1;

    return 0;
}

int salvarIndice(FILE* vc, IndiceArquivador* idx) {
    printf("Stub: salvarIndice chamado.\n");
    return 0;
}

void listarArquivos(const IndiceArquivador* idx) {
    printf("Arquivos no arquivador:\n");
    for (int i = 0; i < idx->quantidade; i++) {
        printf("  %d: %s\n", i, idx->membros[i].nome);
    }
}