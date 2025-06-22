#include "lightning.h"
#include <stdlib.h>

Lightning *createLightning(int x, int y, ALLEGRO_BITMAP *sprite)
{
    Lightning *l = (Lightning *)malloc(sizeof(Lightning));
    if (!l)
        return NULL;
    l->x = x;
    l->y = y;
    l->active = 1;
    l->collected = 0;
    l->sprite = sprite;
    return l;
}

void destroyLightning(Lightning *lightning)
{
    if (!lightning)
        return;
    // Não destrua a sprite aqui se ela for compartilhada!
    free(lightning);
}

void drawLightning(Lightning *lightning)
{
    if (!lightning || !lightning->active || lightning->collected)
        return;
    if (lightning->sprite)
        al_draw_bitmap(lightning->sprite, lightning->x, lightning->y, 0);
}

int checkLightningPickup(Lightning *lightning, Character *player)
{
    if (!lightning || !player || !lightning->active || lightning->collected)
        return 0;
    // Simples colisão retangular
    if (player->hitbox_x < lightning->x + 32 &&
        player->hitbox_x + player->hitbox_w > lightning->x &&
        player->hitbox_y < lightning->y + 32 &&
        player->hitbox_y + player->hitbox_h > lightning->y)
    {
        lightning->collected = 1;
        lightning->active = 0;
        return 1;
    }
    return 0;
}

void useLightningOnBoss(Lightning *lightning, Boss *boss, int *boss_stun_timer)
{
    if (!lightning || !boss || !boss_stun_timer)
        return;
    if (lightning->collected && *boss_stun_timer <= 0)
    {
        *boss_stun_timer = LIGHTNING_STUN_DURATION;
        lightning->collected = 0; // Consome o powerup
    }
}
