#ifndef CHARACTER_H
#define CHARACTER_H

#include "joystick.h"
#include "pistol.h"
#include <allegro5/allegro.h>
#include "background.h"

#define caracther_STEP 4
#define X_SCREEN 800
#define Y_SCREEN 600
#define PLAYER_INITIAL_HP 200

typedef enum
{
    CHAR_STATE_IDLE,
    CHAR_STATE_WALK,
    CHAR_STATE_JUMP,
    CHAR_STATE_CROUCH
} CharacterState;

typedef struct Character
{
    unsigned char side;
    unsigned char face;
    int hp;
    unsigned short x;
    unsigned short y;
    int jumping;
    int crouching;
    unsigned short width;  // largura de um frame da sprite
    unsigned short height; // altura de um frame da sprite
    joystick *control;
    pistol *gun;

    // Arrays de frames para cada animação (direita)
    ALLEGRO_BITMAP **walk_frames_arr_right;
    ALLEGRO_BITMAP **jump_frames_arr_right;
    ALLEGRO_BITMAP **crouch_frames_arr_right;
    ALLEGRO_BITMAP **idle_frames_arr_right;
    ALLEGRO_BITMAP **walk_shoot_frames_arr_right;
    ALLEGRO_BITMAP **jump_shoot_frames_arr_right;
    ALLEGRO_BITMAP **crouch_shoot_frames_arr_right;
    ALLEGRO_BITMAP **idle_shoot_frames_arr_right;

    // Arrays de frames para cada animação (esquerda)
    ALLEGRO_BITMAP **walk_frames_arr_left;
    ALLEGRO_BITMAP **jump_frames_arr_left;
    ALLEGRO_BITMAP **crouch_frames_arr_left;
    ALLEGRO_BITMAP **idle_frames_arr_left;
    ALLEGRO_BITMAP **walk_shoot_frames_arr_left;
    ALLEGRO_BITMAP **jump_shoot_frames_arr_left;
    ALLEGRO_BITMAP **crouch_shoot_frames_arr_left;
    ALLEGRO_BITMAP **idle_shoot_frames_arr_left;

    // Número de frames por animação (direita)
    int walk_frames_right;
    int jump_frames_right;
    int crouch_frames_right;
    int idle_frames_right;
    int walk_shoot_frames_right;
    int jump_shoot_frames_right;
    int crouch_shoot_frames_right;
    int idle_shoot_frames_right;

    // Número de frames por animação (esquerda)
    int walk_frames_left;
    int jump_frames_left;
    int crouch_frames_left;
    int idle_frames_left;
    int walk_shoot_frames_left;
    int jump_shoot_frames_left;
    int crouch_shoot_frames_left;
    int idle_shoot_frames_left;

    CharacterState state;
    int shooting;      // 1 se está atirando, 0 caso contrário
    int frame;         // frame atual da animação
    int fire_cooldown; // frames até poder atirar novamente

    // Campos da hitbox
    int hitbox_x, hitbox_y, hitbox_w, hitbox_h;
} Character;

Character *createCharacter(unsigned char side, unsigned char face, unsigned short x, unsigned short y, unsigned short max_x, unsigned short max_y, int ground_y);

void moveCharacter(Character *element, char steps, unsigned char trajectory, unsigned short max_x, unsigned short max_y);

void shotCharacter(Character *element);

void destroyCharacter(Character *element);

void drawCharacter(Character *player, ALLEGRO_BITMAP *character_sprite, bool debug_mode);

// Agora recebe ground_y e ground_height
void positionUpdate(Character *player, int ground_y, int ground_height);

void bulletUpdate(Character *player);

void updateCharacterHp(Character *player, int delta_hp);

void updateCharacterState(Character *ch);

// Funções para carregar e liberar arrays de frames individuais (agora para direita e esquerda)
void loadCharacterSprites(
    Character *ch,
    const char **walk_right, int walk_frames,
    const char **jump_right, int jump_frames,
    const char **crouch_right, int crouch_frames,
    const char **idle_right, int idle_frames,
    const char **walk_shoot_right, int walk_shoot_frames,
    const char **jump_shoot_right, int jump_shoot_frames,
    const char **crouch_shoot_right, int crouch_shoot_frames,
    const char **idle_shoot_right, int idle_shoot_frames,
    const char **walk_left, int walk_frames_left,
    const char **jump_left, int jump_frames_left,
    const char **crouch_left, int crouch_frames_left,
    const char **idle_left, int idle_frames_left,
    const char **walk_shoot_left, int walk_shoot_frames_left,
    const char **jump_shoot_left, int jump_shoot_frames_left,
    const char **crouch_shoot_left, int crouch_shoot_frames_left,
    const char **idle_shoot_left, int idle_shoot_frames_left);
void destroyCharacterSprites(Character *ch);

void draw_life_bar(Character *player);

#endif