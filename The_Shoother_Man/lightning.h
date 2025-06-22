#ifndef _LIGHTNING_
#define _LIGHTNING_  

#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>

#include "pistol.h"
#include "character.h"
#include "enemy.h"
#include "boss.h"
#include "background.h"
#include "bullet.h"
#include "menu.h"

#define LIGHTNING_STUN_DURATION 150 // 5 segundos a 30 FPS

typedef struct Lightning
{
    int x, y;                   // posição na tela
    int active;                 // 1 se disponível para pegar, 0 se não
    int collected;              // 1 se o player pegou, 0 se não
    ALLEGRO_BITMAP *sprite;     // sprite do raio
} Lightning;

// Cria o powerup na posição (x, y)
Lightning *createLightning(int x, int y, ALLEGRO_BITMAP *sprite);

// Libera memória do powerup
void destroyLightning(Lightning *lightning);

// Desenha o powerup se ativo
void drawLightning(Lightning *lightning);

// Checa se o player pegou o powerup
int checkLightningPickup(Lightning *lightning, Character *player);

// Usa o poder no boss (stun)
void useLightningOnBoss(Lightning *lightning, Boss *boss, int *boss_stun_timer);

#endif