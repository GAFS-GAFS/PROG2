#ifndef ARQUIVO_H
#define ARQUIVO_H

#include <stdio.h>
#include <time.h>

typedef struct {
    char nome[256];           // Nome do arquivo original
    unsigned int tamanho;     // Tamanho em bytes do arquivo original
    unsigned int comprimido;  // Tamanho após compressão (ou igual ao tamanho)
    int estaComprimido;       // 0 = não, 1 = sim
    time_t dataInsercao;      // Data/hora da inserção
    long offsetDados;         // Posição no .vc onde os dados desse arquivo começam
} ArquivoMembro;

int inserirArquivo(const char* nomeArquivo, FILE* vc, long* offsetAtual, ArquivoMembro* out);
int extrairArquivo(FILE* vc, ArquivoMembro membro, const char* pastaDestino);
int removerArquivo(ArquivoMembro* lista, int* qtd, const char* nome); // Remove do índice

#endif