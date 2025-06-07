#ifndef __JOYSTICK__
#define __JOYSTICK__

#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct
{
    unsigned char right;
    unsigned char left;
    unsigned char up;
    unsigned char down;
    unsigned char fire;
} joystick;

joystick *createJoystick();

void destroyJoystick(joystick *element);
void rightMove(joystick *element);
void leftMove(joystick *element);
void upMove(joystick *element);
void downMove(joystick *element);
void fire(joystick *element);

#endif