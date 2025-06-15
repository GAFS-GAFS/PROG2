#ifndef _ENEMY_
#define _ENEMY_

#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_font.h>

#include "pistol.h"
#include "character.h"

#define ENEMY_HP 50

typedef struct Enemy
{
    int x, y;
    int width, height;
    int hp;
    int hitbox_x, hitbox_y, hitbox_w, hitbox_h;
    int fire_cooldown;
    ALLEGRO_BITMAP *sprite;
    pistol *gun;
} Enemy;

Enemy *createEnemy(int x, int y, int width, int height, int hp);
void destroyEnemy(Enemy *enemy);
void moveEnemy(Enemy *enemy, int dx, int dy, int max_x, int max_y);
void drawEnemy(Enemy *enemy);
void enemyShoot(Enemy *enemy, Character *player);
void bulletUpdateEnemy(Enemy *enemy);
void checkPlayerBulletHitsEnemy(Character *player, Enemy *enemy);
void checkEnemyBulletHitsPlayer(Enemy *enemy, Character *player);
int checkEnemyPlayerCollision(Enemy *enemy, Character *player);
void updateEnemy(Enemy *enemy, Character *player, int ground_y);

#endif