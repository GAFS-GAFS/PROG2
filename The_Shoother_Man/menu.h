#ifndef MENU_H
#define MENU_H

#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_color.h>
#include <allegro5/allegro_image.h>

#include <stdio.h>

typedef enum
{
    MENU_START,
    MENU_CONTROLS,
    MENU_EXIT
} menuOption;

#define X_SCREEN 800
#define Y_SCREEN 600

void menuDisplay(ALLEGRO_DISPLAY *display, ALLEGRO_FONT *font, ALLEGRO_EVENT_QUEUE *queue);

#endif