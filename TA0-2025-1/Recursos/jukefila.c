#include <stdio.h>
#include <stdlib.h>
#include "jukefila.h"
#include "pedido.h"

jukefila *criar_jukefila()
{
    jukefila *fila = (jukefila *)malloc(sizeof(jukefila));
    // se usar calloc, os ponteiros inicim com 0, e a fila inicia como NULL

    if (!fila)
    {
        return 0;
    }

    fila->final = NULL;
    fila->inicio = NULL;

    return (fila);
}

void inserir_jukefila(pedido *elemento, jukefila *fila)
{

    if (!fila)
    {
        return;
    }

    if (fila->inicio == NULL || elemento->valor > fila->inicio->valor)
    {
        elemento->proximo = fila->inicio;
        fila->inicio = elemento;
        return;
    }

    pedido *new_music = fila->inicio;

    while (new_music->proximo != NULL && new_music->proximo->valor >= elemento->valor)
    {
        new_music = new_music->proximo;
    }

    elemento->proximo = new_music->proximo;
    new_music->proximo = elemento;
}
/*
    void inserir_jukefila(pedido *elemento, jukefila *fila)

    if((!fila) || (!elemento))
    {
        return;
    }

    if (!fila->inicio)
    {
        fila->inicio = elemento;
        fila->final = elemento;
        return;
    }

    pedido *i;

    for (i = fila->inicio; ((i) && (i->valor <= elemento->valor)); i = i->proximo){

        if (!i)
        {
            elemento->proximo = i;
            i->anterior = elemento;
            fila->inicio = elemento;
        }
    }
*/

pedido *consumir_jukefila(jukefila *fila)
{
    if (!fila)
    {
        return NULL;
    }

    pedido *song = fila->inicio;

    if (song == NULL)
    {
        return (NULL);
    }

    if (contar_jukefila(fila) == 1)
    {
        fila->inicio = NULL;
        fila->inicio = NULL;
    }
    else
    {

        fila->inicio = fila->inicio->proximo;
        fila->inicio->anterior = NULL;
    }

    return (song);
}

unsigned int contar_jukefila(jukefila *fila)
{
    int numero = 0;

    pedido *contagem = fila->inicio;

    if (contagem == NULL)
    {
        return (0);
    }

    while (contagem != NULL)
    {
        numero = numero + 1;
        contagem = contagem->proximo;
    }

    return (numero);
}

void destruir_jukefila(jukefila *fila)
{

    if (!fila)
    {
        return;
    }

    pedido *atual = fila->inicio;

    while (atual != NULL)
    {
        pedido *temp = atual;

        atual = atual->proximo;

        destruir_pedido(temp);
    }

    free(fila);

    return;
}
