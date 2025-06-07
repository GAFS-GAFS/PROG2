#ifndef Character_H
#define caracther_H

#include "joystick.h"
#include "pistol.h"

#define caracther_STEP 4
#define X_SCREEN 800
#define Y_SCREEN 600
#define PLAYER_INITIAL_HP 200
#define GROUND_Y 552

typedef struct
{
    unsigned char side;
    unsigned char face;
    int hp;
    unsigned short x;
    unsigned short y;
    int jumping;
    int crouching;
    unsigned short width;
    unsigned short height;
    joystick *control;
    pistol *gun;
} Character;

Character *createCharacter(unsigned char side, unsigned char face, unsigned short x, unsigned short y, unsigned short max_x, unsigned short max_y);

void moveCharacter(Character *element, char steps, unsigned char trajectory, unsigned short max_x, unsigned short max_y);

void shotCharacter(Character *element);

void destroyCharacter(Character *element);

void drawCharacter(Character *player, ALLEGRO_BITMAP *spaceship_image, bool debug_mode);

void positionUpdate(Character *player);

void bulletUpdate(Character *player);

void updateCharacterHp(Character *player, int delta_hp);

#endif