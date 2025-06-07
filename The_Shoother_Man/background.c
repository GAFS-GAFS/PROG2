#include "background.h"

static ALLEGRO_BITMAP *bg_image = NULL;
static float bg_x = 0;

void init_background(const char *image_path)
{
    bg_image = al_load_bitmap(image_path);
    if (!bg_image)
    {
        fprintf(stderr, "Erro ao carregar imagem de fundo!\n");
    }
    bg_x = 0;
}

void update_background(float speed)
{
    bg_x -= speed;
    if (bg_x <= -al_get_bitmap_width(bg_image))
    {
        bg_x = 0;
    }
}

void draw_background()
{
    if (!bg_image)
        return;

    al_draw_bitmap(bg_image, bg_x, 0, 0);
    al_draw_bitmap(bg_image, bg_x + al_get_bitmap_width(bg_image), 0, 0);
}

void destroy_background()
{
    if (bg_image)
        al_destroy_bitmap(bg_image);
}