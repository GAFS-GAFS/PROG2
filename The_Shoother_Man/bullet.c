#include "bullet.h"
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>

static ALLEGRO_BITMAP *bullet_sprite = NULL;

void load_bullet_sprite(const char *path)
{
    if (!bullet_sprite)
        bullet_sprite = al_load_bitmap(path);
}

void destroy_bullet_sprite()
{
    if (bullet_sprite)
    {
        al_destroy_bitmap(bullet_sprite);
        bullet_sprite = NULL;
    }
}

void drawBullets(bullet *elements)
{
    for (bullet *current = elements; current != NULL; current = (bullet *)current->next)
    {
        if (bullet_sprite)
            al_draw_bitmap(bullet_sprite, current->x, current->y, 0);
    }
}

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