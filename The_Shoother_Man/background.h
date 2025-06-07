#ifndef BACKGROUND_H
#define BACKGROUND_H

#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>

#include <stdlib.h>
#include <stdio.h>

#define X_SCREEN 800
#define Y_SCREEN 600


void init_background(const char *image_path);
void update_background(float speed);
void draw_background();
void destroy_background();

#endif