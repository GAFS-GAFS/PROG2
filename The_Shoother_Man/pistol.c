#include "pistol.h"

pistol *createPistol()
{
    pistol *newPistol = (pistol *)malloc(sizeof(pistol));

    if (!newPistol)
    {
        fprintf(stderr, "Alocação de Memória para a Pistola falhou.\n");
        return (NULL);
    }

    newPistol->timer = 0;
    newPistol->shots = NULL;

    return (newPistol);
}

// Altere a assinatura da função firePistol para bater exatamente com o header pistol.h:
bullet *firePistol(unsigned short x, unsigned short y, unsigned char trajectory, pistol *gun)
{
    if (!gun)
    {
        printf("firePistol: gun nulo\n");
        return NULL;
    }
    bullet *b = malloc(sizeof(bullet));
    if (!b)
    {
        printf("firePistol: malloc falhou\n");
        return NULL;
    }
    b->x = x;
    b->y = y;
    b->trajectory = trajectory;
    b->damage = 10;
    b->next = NULL;
    return b;
}

void destroyPistol(pistol *element)
{
    if (!element)
        return;
    bullet *sentinel;
    for (bullet *current = element->shots; current != NULL; current = sentinel)
    {
        sentinel = (bullet *)current->next;
        destroyBullet(current);
    }
    element->shots = NULL;
    free(element);
}