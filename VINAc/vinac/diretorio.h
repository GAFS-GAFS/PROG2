#ifndef DIRETORIO_H
#define DIRETORIO_H

#include "arquivo.h"

// Inicializa um novo índice vazio
// idx: ponteiro para o índice a ser inicializado
void inicializarIndice(IndiceArquivador *idx);

// Libera toda a memória alocada pelo índice, incluindo a lista de arquivos
// idx: ponteiro para o índice a ser destruído
void destruirIndice(IndiceArquivador *idx);

// Adiciona um novo arquivo ao índice ou atualiza se já existir
// idx: ponteiro para o índice
// membro: estrutura contendo os metadados do arquivo a ser adicionado
void adicionarAoIndice(IndiceArquivador *idx, ArquivoMembro membro);

// Carrega os metadados do arquivo .vc para a memória
// vc: arquivo .vc aberto para leitura
// idx: ponteiro para o índice onde os dados serão carregados
// Retorna 0 em caso de sucesso, ou -1 em caso de erro
int carregarIndice(FILE *vc, IndiceArquivador *idx);

// Salva o índice atual da memória de volta no arquivo .vc
// vc: arquivo .vc aberto para escrita
// idx: ponteiro para o índice a ser salvo
// Retorna 0 em caso de sucesso, ou -1 em caso de erro
int salvarIndice(FILE *vc, IndiceArquivador *idx);

// Lista todos os arquivos presentes no índice com seus metadados
// idx: ponteiro para o índice a ser listado
void listarArquivos(const IndiceArquivador *idx);

// Move um nó dentro da lista de arquivos
// lista: ponteiro para a lista de arquivos
// atualPos: posição atual do nó
// novaPos: nova posição desejada do nó
int moverNoLista(Lista *lista, uint32_t atualPos, uint32_t novaPos);

#endif
