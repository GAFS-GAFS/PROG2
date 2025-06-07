#include "bullet.h"

bullet *initBullet(unsigned short x, unsigned short y, unsigned char trajectory, unsigned char damage, bullet *next)
{
    if ((trajectory < 0) || (trajectory > 1))
    {
        return (NULL);
    }

    bullet *newBullet = (bullet *)malloc(sizeof(bullet));

    if (!newBullet)
    {
        fprintf(stderr, "Alocação de memória da Bala deu errado.\n");
        return (NULL);
    }

    newBullet->x = x;
    newBullet->y = y;
    newBullet->trajectory = trajectory;
    newBullet->damage = damage;
    newBullet->next = next;

    return (newBullet);
}

void moveBullet(bullet *elements)
{
    for (bullet *current = elements; current != NULL; current = (bullet *)current->next)
    {
        if (!current->trajectory)
        {
            current->x += BULLET_MOVE;
        }
        else
        {
            current->x -= BULLET_MOVE;
        }
    }
}

void destroyBullet(bullet *element)
{
    if (element != NULL)
    {
        destroyBullet((bullet *)element->next);
        free(element);
    }
}