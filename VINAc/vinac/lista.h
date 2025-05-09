#ifndef LISTA_H
#define LISTA_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "membros.h"

// Define a estrutura de um nó da lista duplamente encadeada
// Cada nó contém os metadados de um arquivo e ponteiros para o próximo e anterior
typedef struct No {
    ArquivoMembro arquivo;  // Metadados do arquivo
    struct No* proximo;     // Ponteiro para o próximo nó
    struct No* anterior;    // Ponteiro para o nó anterior
} No;

// Estrutura principal da lista duplamente encadeada
typedef struct {
    No* primeiro;          // Ponteiro para o primeiro nó da lista
    No* ultimo;           // Ponteiro para o último nó da lista
    uint32_t quantidade;   // Quantidade total de arquivos na lista
} Lista;

// Funções de inicialização e limpeza
void inicializarLista(Lista* lista);      // Inicializa uma nova lista vazia

void destruirLista(Lista* lista);         // Libera toda a memória alocada pela lista

// Funções de manipulação básica
// Insere um novo arquivo na lista, retorna 1 se sucesso, 0 se falha
int inserirArquivoLista(Lista* lista, ArquivoMembro arquivo);

// Remove um arquivo da lista pelo nome, retorna 1 se removeu, 0 se não encontrou
int removerArquivoLista(Lista* lista, const char* nome);

// Busca um arquivo na lista pelo nome, retorna NULL se não encontrar
No* buscarArquivo(Lista* lista, const char* nome);

// Funções utilitárias
void imprimirLista(Lista* lista);                    // Imprime todos os arquivos da lista

void atualizarOrdens(Lista* lista);                  // Atualiza a ordem dos arquivos na lista

uint32_t contarElementos(Lista* lista);              // Retorna o número de elementos na lista

No* buscarPorUid(Lista* lista, uint32_t uid);       // Busca um arquivo pelo seu UID

#endif