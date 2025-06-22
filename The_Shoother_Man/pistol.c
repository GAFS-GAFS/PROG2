#include "pistol.h"
#include <stdio.h>
#include <stdlib.h>

pistol *createPistol()
{
    pistol *newPistol = (pistol *)malloc(sizeof(pistol));
    if (!newPistol)
    {
        fprintf(stderr, "Alocação de Memória para a Pistola falhou.\n");
        return NULL;
    }
    newPistol->timer = 0;
    newPistol->shots = NULL;
    newPistol->max_ammo = 8;
    newPistol->ammo = newPistol->max_ammo;
    newPistol->reloading = 0;
    newPistol->reload_timer = 0;
    return newPistol;
}

bullet *firePistol(unsigned short x, unsigned short y, unsigned char trajectory, pistol *gun, int is_player)
{
    if (!gun)
        return NULL;
    if (is_player)
    {
        // Não pode atirar se está recarregando ou sem munição
        if (gun->reloading || gun->ammo <= 0)
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
    if (is_player)
    {
        gun->ammo--;
        // Recarga automática de 2s se acabar a munição
        if (gun->ammo == 0 && !gun->reloading)
        {
            gun->reloading = 1;
            gun->reload_timer = 60; // 2 segundos a 30 FPS
        }
    }
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

// A função de recarga foi removida, pois já está implementada em character.c