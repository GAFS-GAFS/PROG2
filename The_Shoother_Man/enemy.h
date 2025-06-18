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
    int direction; // 1 = esquerda, 0 = direita
    // Sprites para animações (igual Character, mas sem pulo/agachar)
    ALLEGRO_BITMAP **walk_frames_arr_right;
    ALLEGRO_BITMAP **idle_frames_arr_right;
    ALLEGRO_BITMAP **walk_shoot_frames_arr_right;
    ALLEGRO_BITMAP **idle_shoot_frames_arr_right;
    ALLEGRO_BITMAP **walk_frames_arr_left;
    ALLEGRO_BITMAP **idle_frames_arr_left;
    ALLEGRO_BITMAP **walk_shoot_frames_arr_left;
    ALLEGRO_BITMAP **idle_shoot_frames_arr_left;
    int walk_frames_right;
    int idle_frames_right;
    int walk_shoot_frames_right;
    int idle_shoot_frames_right;
    int walk_frames_left;
    int idle_frames_left;
    int walk_shoot_frames_left;
    int idle_shoot_frames_left;
    int state; // 0 = idle, 1 = walk, 2 = idle_shoot, 3 = walk_shoot
    int frame;
    ALLEGRO_BITMAP *sprite;
    pistol *gun;
    // --- Novos campos para patrulha ---
    int patrol_timer;     // tempo restante andando/parado
    int patrol_dir_timer; // tempo até trocar direção
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
void loadEnemySprites(Enemy *enemy,
                      const char **walk_right, int walk_frames_right,
                      const char **idle_right, int idle_frames_right,
                      const char **walk_shoot_right, int walk_shoot_frames_right,
                      const char **idle_shoot_right, int idle_shoot_frames_right,
                      const char **walk_left, int walk_frames_left,
                      const char **idle_left, int idle_frames_left,
                      const char **walk_shoot_left, int walk_shoot_frames_left,
                      const char **idle_shoot_left, int idle_shoot_frames_left);
void destroyEnemySprites(Enemy *enemy);
void updateEnemyStateAndFrame(Enemy *enemy);

#endif