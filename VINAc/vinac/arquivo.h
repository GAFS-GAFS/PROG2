#ifndef ARQUIVO_H
#define ARQUIVO_H

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <ctype.h>
#include "lz.h"
#include "membros.h"
#include "lista.h"

// Estrutura que representa o índice completo do arquivador (.vc)
// Contém a lista de todos os arquivos armazenados e seus metadados
typedef struct
{
    Lista lista;        // Lista duplamente encadeada com os metadados dos arquivos
    uint32_t ultimoUID; // UID mais alto já utilizado
} IndiceArquivador;

// Insere apenas os dados binários de um arquivo no .vc
int inserirArquivo(const char *nomeArquivo, FILE *vc, uint64_t *offset,
                   uint32_t *tamanhoFinal, uint32_t compressao);

// Manipula apenas os dados binários dentro do archive.vc
// Retorna o novo tamanho em disco (>0) ou -1 em erro. diffTamanho recebe a diferença de tamanho (novo - antigo), tamanhoOriginal recebe o tamanho descomprimido
int inserirComp(const char *nomeArquivo, FILE *vc, uint64_t offset, uint32_t emDiscoAntigo, uint32_t compressao, unsigned char *buffer, size_t bufferSize, int *diffTamanho, uint32_t *tamanhoOriginal);

// Extrai um arquivo do .vc
int extrairArquivo(FILE *vc, ArquivoMembro membro, const char *pastaDestino);

// Remove logicamente do índice e ajusta os arquivos no disco
int removerArquivo(FILE *vc, Lista *lista, const char *nome);

// Move um bloco de dados dentro do arquivo
int moverDados(FILE *vc, uint64_t offsetInicio, uint64_t offsetFim, size_t tamanho, size_t tamanhoBuffer);

// Encontra o maior arquivo na lista de arquivos
size_t encontrarMaiorArquivo(Lista *lista, size_t tamanhoAtual);

#endif
