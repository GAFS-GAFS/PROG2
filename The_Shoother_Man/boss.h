#ifndef _BOSS_
#define _BOSS_

#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_font.h>

#include "pistol.h"
#include "character.h"

#include <stdlib.h>
#include <stdio.h>

#define BOSS_HP 300
#define BOSS_STEP 2
#define BOSS_WIDTH 97
#define BOSS_HEIGHT 54

typedef struct Boss
{
    int x, y;
    int width, height;
    int hp;
    int hitbox_x, hitbox_y, hitbox_w, hitbox_h;
    int fire_cooldown;
    int direction; // 1 = esquerda, 0 = direita
    int state;     // 0 = idle, 1 = walk, 2 = shoot, 3 = special
    int frame;
    ALLEGRO_BITMAP **walk_frames_arr;
    ALLEGRO_BITMAP **idle_frames_arr;
    ALLEGRO_BITMAP **shoot_frames_arr;
    int walk_frames;
    int idle_frames;
    int shoot_frames;
    pistol *gun;
    int special_cooldown; // Para ataques especiais
} Boss;

Boss *createBoss(int x, int y);
void destroyBoss(Boss *boss);
void updateBoss(Boss *boss, Character *player, int ground_y);
void drawBoss(Boss *boss);
void bossShoot(Boss *boss, Character *player);
int checkBossPlayerCollision(Boss *boss, Character *player);
void loadBossSprites(Boss *boss,
                     const char **walk_frames, int walk_frames_count,
                     const char **idle_frames, int idle_frames_count,
                     const char **shoot_frames, int shoot_frames_count);
void destroyBossSprites(Boss *boss);
void checkPlayerBulletHitsBoss(Character *player, Boss *boss);
void checkBossBulletHitsPlayer(Boss *boss, Character *player);
void drawBossLifeBar(Boss *boss);

// Adicione as funções para carregar/liberar a sprite da bala do boss
void load_boss_bullet_sprite(const char *path);
void destroy_boss_bullet_sprite(void);

#endif