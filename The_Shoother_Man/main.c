#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>

#define X_SCREEN 800
#define Y_SCREEN 600

int main ()
{
    al_init();
    al_install_keyboard();

    ALLEGRO_TIMER* timer = al_create_timer(1.0/30.0);

    ALLEGRO_EVENT_QUEUE* queue();
}