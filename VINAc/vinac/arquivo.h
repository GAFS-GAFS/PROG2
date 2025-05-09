#ifndef ARQUIVO_H
#define ARQUIVO_H

#include <stddef.h>  
#include <stdio.h>   
#include <stdlib.h>  
#include <string.h> 
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include "lz.h"
#include "membros.h"
#include "lista.h"

// Insere apenas os dados binários de um arquivo no .vc
int inserirArquivo(const char* nomeArquivo, FILE* vc, uint64_t* offset,
    uint32_t* tamanhoFinal, uint32_t compressao);

// Extrai um arquivo do .vc
int extrairArquivo(FILE* vc, ArquivoMembro membro, const char* pastaDestino);

// Remove logicamente do índice e ajusta os arquivos no disco
int removerArquivo(FILE* vc, Lista* lista, const char* nome);

// Ajusta os metadados após remoção
void ajustarMetadados(FILE* vc, Lista* lista, uint32_t posRemovida);

// Move um bloco de dados dentro do arquivo
int moverDados(FILE* vc, uint64_t offsetInicio, uint64_t offsetFim, size_t tamanho);

// Reorganiza o arquivo removendo espaços vazios
int reorganizarArquivo(FILE* vc, Lista* lista);

// Atualiza os offsets após uma movimentação
int atualizarOffsets(Lista* lista, uint64_t offsetBase, int64_t diferenca);

#endif
