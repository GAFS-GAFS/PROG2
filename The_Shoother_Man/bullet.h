#ifndef __BULLET__
#define __BULLET__

#define BULLET_MOVE 10

#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_font.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct bullet
{
    unsigned short x;
    unsigned short y;
    unsigned char trajectory;
    unsigned char damage;
    struct bullet *next;
} bullet;

bullet *initBullet(unsigned short x, unsigned short y, unsigned char trajectory, unsigned char damage, bullet *next);

void moveBullet(bullet *elements);

void destroyBullet(bullet *element);

#endif