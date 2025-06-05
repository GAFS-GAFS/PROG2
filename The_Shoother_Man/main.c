#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
#include <stdio.h>
#include "menu.h"

#define X_SCREEN 800
#define Y_SCREEN 600

int main()
{
    al_init();
    al_install_keyboard();
    al_init_image_addon();

    ALLEGRO_TIMER *timer = al_create_timer(1.0 / 30.0);
    ALLEGRO_EVENT_QUEUE *queue = al_create_event_queue();
    ALLEGRO_FONT *font = al_create_builtin_font();
    ALLEGRO_DISPLAY *display = al_create_display(X_SCREEN, Y_SCREEN);

    ALLEGRO_BITMAP *bg1 = al_load_bitmap("./imagens/subway_BG.png");
    if (!bg1)
    {
        fprintf(stderr, "Erro ao carregar imagem!\n");
        return (-1);
    }

    al_register_event_source(queue, al_get_keyboard_event_source());
    al_register_event_source(queue, al_get_display_event_source(display));
    al_register_event_source(queue, al_get_timer_event_source(timer));

    menuDisplay(display, font, queue);

    al_start_timer(timer);

    ALLEGRO_EVENT event;

    while (1)
    {
        al_wait_for_event(queue, &event);
        if (event.type == ALLEGRO_EVENT_TIMER)
        {
            al_draw_bitmap(bg1, 0, 0, 0);
            al_flip_display();
        }
        else if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
        {
            break;
        }
    }

    al_destroy_bitmap(bg1);
    al_destroy_display(display);
    al_destroy_timer(timer);
    al_destroy_event_queue(queue);
    al_destroy_font(font);

    return (0);
}