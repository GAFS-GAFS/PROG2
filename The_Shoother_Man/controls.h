#ifndef CONTROLS_H
#define CONTROLS_H  

#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>

#include <stdlib.h>
#include <stdio.h>

#define X_SCREEN 800
#define Y_SCREEN 600

void controlsDisplay(ALLEGRO_DISPLAY *display, ALLEGRO_FONT *font, ALLEGRO_EVENT_QUEUE *queue);

#endif 