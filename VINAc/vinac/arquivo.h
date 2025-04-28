#ifndef ARQUIVO_H
#define ARQUIVO_H

#include <stddef.h>  
#include <stdint.h>
#include <stdio.h>   
#include <stdlib.h>  
#include <string.h> 
#include <time.h> 
#include <dirent.h>
#include "lz.h"   

// Estrutura representando um arquivo no .vc

typedef struct {
    char nome[256];            // Nome do arquivo original (sem espaços)
    unsigned int uid;          // UID do usuário que inseriu 
    unsigned int tamanho;      // Tamanho original do arquivo
    unsigned int emDisco;      // Tamanho em disco (comprimido ou não)
    time_t modificacao;        // Data de modificação
    int ordem;                 // Ordem de inserção no .vc
    long offset;               // Offset a partir do início do .vc
} ArquivoMembro;

// Insere um arquivo no .vc (com ou sem compressão)
int inserirArquivo(const char* nomeArquivo, FILE* vc, long* offsetAtual,
                   ArquivoMembro* out, int compressao, int ordem);

// Extrai um arquivo do .vc
int extrairArquivo(FILE* vc, ArquivoMembro membro, const char* pastaDestino);

// Remove logicamente do índice
int removerArquivo(ArquivoMembro* lista, int* qtd, const char* nome);

// Funções de manipulação binária
int moverDados(FILE* vc, long offsetInicio, long offsetFim, size_t tamanho);
int copiarDados(FILE* origem, FILE* destino, size_t tamanho);
int reorganizarArquivo(FILE* vc, ArquivoMembro* membros, int quantidade);
long encontrarEspacoLivre(FILE* vc, size_t tamanhoNecessario);

// Funções auxiliares para manipulação do arquivo .vc
int atualizarOffsets(ArquivoMembro* membros, int quantidade, long offsetBase, long diferenca);
int verificarEspacoDisponivel(FILE* vc, size_t tamanhoNecessario);

#endif
