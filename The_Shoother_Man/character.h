#ifndef Character_H
#define caracther_H

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

    // Spritesheets
    ALLEGRO_BITMAP *walk_spritesheet;
    ALLEGRO_BITMAP *jump_spritesheet;
    ALLEGRO_BITMAP *crouch_spritesheet;
    ALLEGRO_BITMAP *idle_spritesheet;
    ALLEGRO_BITMAP *walk_shoot_spritesheet;
    ALLEGRO_BITMAP *jump_shoot_spritesheet;
    ALLEGRO_BITMAP *crouch_shoot_spritesheet;
    ALLEGRO_BITMAP *idle_shoot_spritesheet;

    // Número de frames por animação
    int walk_frames;
    int jump_frames;
    int crouch_frames;
    int idle_frames;
    int walk_shoot_frames;
    int jump_shoot_frames;
    int crouch_shoot_frames;
    int idle_shoot_frames;

    CharacterState state;
    int shooting; // 1 se está atirando, 0 caso contrário
    int frame;    // frame atual da animação
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

// Adicione funções para carregar e liberar spritesheets
void loadCharacterSprites(
    Character *ch,
    const char *walk, int walk_frames,
    const char *jump, int jump_frames,
    const char *crouch, int crouch_frames,
    const char *idle, int idle_frames,
    const char *walk_shoot, int walk_shoot_frames,
    const char *jump_shoot, int jump_shoot_frames,
    const char *crouch_shoot, int crouch_shoot_frames,
    const char *idle_shoot, int idle_shoot_frames);
void destroyCharacterSprites(Character *ch);

void draw_life_bar(Character *player);

#endif