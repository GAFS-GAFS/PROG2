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

bullet *firePistol(unsigned short x, unsigned short y, unsigned char trajectory, pistol *gun)
{
    unsigned char damage = 10;

    bullet *newBullet = initBullet(x, y, trajectory, damage, gun->shots);

    if (!newBullet)
    {
        fprintf(stderr, "Falha ao criar a bala.\n");
        return (NULL);
    }

    return (newBullet);
}

void destroyPistol(pistol *element)
{
    bullet *sentinel;

    for (bullet *current = element->shots; current != NULL; current = sentinel)
    {
        sentinel = (bullet *)current->next;
        destroyBullet(current);
    }

    free(element);
}