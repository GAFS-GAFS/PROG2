#include "lista.h"

// Inicializa uma nova lista vazia
// Configura os ponteiros como NULL e zera o contador
void inicializarLista(Lista* lista) {
    lista->primeiro = NULL;
    lista->ultimo = NULL;
    lista->quantidade = 0;
}

// Libera toda a memória alocada pela lista
// Percorre todos os nós liberando cada um
void destruirLista(Lista* lista) {
    No* atual = lista->primeiro;
    while (atual != NULL) {
        No* proximo = atual->proximo;
        free(atual);
        atual = proximo;
    }
    lista->primeiro = NULL;
    lista->ultimo = NULL;
    lista->quantidade = 0;
}

// Insere um novo arquivo na lista
// O novo arquivo é sempre inserido no final da lista
// Retorna 1 em caso de sucesso, 0 em caso de falha
int inserirArquivoLista(Lista* lista, ArquivoMembro arquivo) {
    No* novoNo = (No*)malloc(sizeof(No));
    if (!novoNo) return 0;

    novoNo->arquivo = arquivo;
    novoNo->proximo = NULL;
    novoNo->anterior = NULL;

    // Se a lista está vazia, o novo nó será o primeiro e último
    if (lista->primeiro == NULL) {
        lista->primeiro = novoNo;
        lista->ultimo = novoNo;
    } else {
        // Insere no final da lista
        novoNo->anterior = lista->ultimo;
        lista->ultimo->proximo = novoNo;
        lista->ultimo = novoNo;
    }

    lista->quantidade++;
    atualizarOrdens(lista);
    return 1;
}

// Remove um arquivo da lista pelo nome
// Ajusta os ponteiros anterior/próximo conforme necessário
// Retorna 1 se removeu, 0 se não encontrou
int removerArquivoLista(Lista* lista, const char* nome) {
    No* atual = lista->primeiro;

    while (atual != NULL) {
        if (strcmp(atual->arquivo.nome, nome) == 0) {
            // Ajusta os ponteiros
            if (atual->anterior == NULL) {
                lista->primeiro = atual->proximo;
                if (lista->primeiro) {
                    lista->primeiro->anterior = NULL;
                }
            } else {
                atual->anterior->proximo = atual->proximo;
            }

            if (atual->proximo == NULL) {
                lista->ultimo = atual->anterior;
                if (lista->ultimo) {
                    lista->ultimo->proximo = NULL;
                }
            } else {
                atual->proximo->anterior = atual->anterior;
            }

            free(atual);
            lista->quantidade--;
            return 1; // Sucesso
        }
        atual = atual->proximo;
    }
    return 0; // Arquivo não encontrado
}

// Busca um arquivo na lista pelo nome
// Retorna o nó encontrado ou NULL se não existir
No* buscarArquivo(Lista* lista, const char* nome) {
    No* atual = lista->primeiro;
    while (atual != NULL) {
        if (strcmp(atual->arquivo.nome, nome) == 0) {
            return atual;
        }
        atual = atual->proximo;
    }
    return NULL;
}

// Imprime o conteúdo da lista para debug
// Mostra ordem, nome e UID de cada arquivo
void imprimirLista(Lista* lista) {
    No* atual = lista->primeiro;
    while (atual != NULL) {
        printf("Ordem: %u, Nome: %s, UID: %u\n", 
               atual->arquivo.ordem,
               atual->arquivo.nome,
               atual->arquivo.uid);
        atual = atual->proximo;
    }
}

// Retorna a quantidade de elementos na lista
uint32_t contarElementos(Lista* lista) {
    return lista->quantidade;
}

// Busca um arquivo na lista pelo UID
// Retorna o nó encontrado ou NULL se não existir
No* buscarPorUid(Lista* lista, uint32_t uid) {
    No* atual = lista->primeiro;
    while (atual != NULL) {
        if (atual->arquivo.uid == uid) {
            return atual;
        }
        atual = atual->proximo;
    }
    return NULL;
}

// Atualiza a ordem de todos os arquivos na lista
// A ordem é definida pela posição do arquivo na lista
void atualizarOrdens(Lista* lista) {
    No* atual = lista->primeiro;
    uint32_t ordem = 0;
    while (atual != NULL) {
        atual->arquivo.ordem = ordem++;
        atual = atual->proximo;
    }
}