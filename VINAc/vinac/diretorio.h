#ifndef DIRETORIO_H
#define DIRETORIO_H

#include "arquivo.h"  

// Estrutura que representa o índice dos arquivos presentes no arquivador (.vc)
typedef struct {
    ArquivoMembro* membros; // Vetor dinâmico com os metadados dos arquivos (nome, tamanho, offset etc.)
    int quantidade;         // Quantidade atual de arquivos armazenados no índice
    int capacidade;         // Capacidade alocada do vetor (para evitar realocação frequente)
} IndiceArquivador;


// Inicializa o índice, alocando memória para o vetor de membros.

void inicializarIndice(IndiceArquivador* idx);


// Libera toda a memória alocada pelo índice.

void destruirIndice(IndiceArquivador* idx);


// Adiciona um novo membro ao índice (ou substitui se já existir).

void adicionarAoIndice(IndiceArquivador* idx, ArquivoMembro membro);


// Carrega os metadados (índice) do arquivo .vc para a RAM.
// Retorna 0 em caso de sucesso, ou -1 em caso de erro.

int carregarIndice(FILE* vc, IndiceArquivador* idx);


// Salva o índice atual da RAM de volta no arquivo .vc.
// Retorna 0 em caso de sucesso, ou -1 em caso de erro.

int salvarIndice(FILE* vc, IndiceArquivador* idx);


// Lista os arquivos presentes no índice, mostrando os metadados de cada um.

void listarArquivos(const IndiceArquivador* idx);

#endif
