#include <stdio.h>
#include <stdlib.h>
#include "FilaJornal.h"

void requisitar_noticia(char *titulo, char *texto)
{
    getchar();
    printf("\nDigite o título: ");
    fgets(titulo, 33, stdin);
    printf("Digite o texto: ");
    fgets(texto, 513, stdin);
}

noticia *criar_noticia()
{
    noticia *nova = (noticia *)malloc(sizeof(noticia));

    if (!nova)
    {
        return (0);
    }

    requisitar_noticia(nova->titulo, nova->texto);

    nova->idade = 0;

    nova->prox = NULL;

    return (nova);
}

int inserir_noticia(noticia *nova, fila_jornal *fila)
{

    // adicionar verificações

    if (fila->final) // se tem algo no final, ela não está vazia e tem que apontar pra nova noticia
    {
        fila->final->prox = nova; // atualiza a fila,com a nova noticia, caso tenha alguem na ordem
        nova->prox = NULL;
        fila->final = nova;
    }
    else
    {
        fila->inicio = nova;
        fila->final = nova;
    }

    return (0);
}

noticia *remover_noticia(fila_jornal *fila)
{
    if (!fila)
    {
        return NULL;
    }

    if (!fila->inicio)
    {
        return NULL;
    }

    noticia *retorno = fila->inicio;
    fila->inicio = retorno->prox;

    if (!fila->inicio->prox)
    {
        fila->final = NULL;
    }

    return (retorno);
}

void atualizar_fila(fila_jornal *fila)
{
    for (noticia *iterador = fila->inicio; (iterador && iterador->idade == 3); iterador = fila->inicio)
        ;

    free(remover_noticia(fila));

    for (noticia *iterador = fila->inicio; iterador; iterador->idade++, iterador = iterador->prox)
        ;
}

void destruir_fila(fila_jornal *fila)
{
    while (fila->inicio)
    {
        free(remover_noticia(fila));
        free(fila);
    }
}

int main()
{
}