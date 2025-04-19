#ifndef ARQUIVO_H
#define ARQUIVO_H

#include <stdio.h>
#include <time.h>

// Estrutura que representa um arquivo armazenado no arquivador .vc.

typedef struct {
    char nome[256];            // Nome do arquivo original
    unsigned int tamanho;      // Tamanho original em bytes
    unsigned int comprimido;   // Tamanho após compressão (ou igual ao original)
    int estaComprimido;        // 1 = comprimido, 0 = não comprimido
    time_t dataInsercao;       // Data e hora da inserção no arquivador
    long offsetDados;          // Offset no .vc onde os dados do arquivo estão
} ArquivoMembro;

//Insere um arquivo no .vc (com ou sem compressão).

// nomeArquivo  Caminho do arquivo no disco.
// vc           Ponteiro para o arquivo .vc.
// offsetAtual  Offset onde o novo arquivo será inserido.
// out          Struct preenchida com os metadados do membro.
// compressao   1 para compactar, 0 para armazenar puro.

// 0 se sucesso, <0 se erro.

int inserirArquivo(const char* nomeArquivo, FILE* vc, long* offsetAtual, ArquivoMembro* out, int compressao);

// Extrai um arquivo do .vc e salva no disco.

// vc             Ponteiro para o .vc.
// membro         Metadados do arquivo a ser extraído.
// pastaDestino   Caminho onde o arquivo extraído será salvo.

// 0 se sucesso, <0 se erro.

int extrairArquivo(FILE* vc, ArquivoMembro membro, const char* pastaDestino);

// Remove logicamente um arquivo do índice em RAM.
// (não remove os dados do .vc, só do índice!)

// lista   Vetor de ArquivoMembro.
// qtd     Ponteiro para a quantidade atual.
// nome    Nome do arquivo a ser removido.

// 1 se removido, 0 se não encontrado.

int removerArquivo(ArquivoMembro* lista, int* qtd, const char* nome);

#endif 