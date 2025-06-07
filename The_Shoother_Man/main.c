#include "menu.h"
#include "background.h"
#include "character.h"

#define X_SCREEN 800
#define Y_SCREEN 600
#define GROUND_HEIGHT 48
#define GROUND_Y (Y_SCREEN - GROUND_HEIGHT)

int main()
{
    al_init();
    al_install_keyboard();
    al_init_image_addon();
    al_init_font_addon();
    al_init_primitives_addon();

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

    init_background("./imagens/subway_BG.png");

    // Cria o personagem exatamente em cima do chão
    int player_height = 48;
    Character *player = createCharacter(32, player_height, X_SCREEN / 2, GROUND_Y - player_height, X_SCREEN, Y_SCREEN);

    al_start_timer(timer);

    ALLEGRO_EVENT event;

    bool running = true;
    bool redraw = true;

    while (running)
    {
        al_wait_for_event(queue, &event);

        if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
        {
            running = false;
        }
        else if (event.type == ALLEGRO_EVENT_KEY_DOWN)
        {
            switch (event.keyboard.keycode)
            {
            case ALLEGRO_KEY_RIGHT:
                player->control->right = 1;
                break;
            case ALLEGRO_KEY_LEFT:
                player->control->left = 1;
                break;
            case ALLEGRO_KEY_UP:
                if (!player->jumping && !player->crouching && player->y >= GROUND_Y - player_height)
                    player->jumping = 1;
                break;
            case ALLEGRO_KEY_DOWN:
                if (!player->jumping && player->y >= GROUND_Y - player_height)
                {
                    player->control->down = 1;
                    player->crouching = 1;
                }
                break;
            case ALLEGRO_KEY_SPACE:
                player->control->fire = 1;
                shotCharacter(player);
                break;
            }
        }
        else if (event.type == ALLEGRO_EVENT_KEY_UP)
        {
            switch (event.keyboard.keycode)
            {
            case ALLEGRO_KEY_RIGHT:
                player->control->right = 0;
                break;
            case ALLEGRO_KEY_LEFT:
                player->control->left = 0;
                break;
            case ALLEGRO_KEY_DOWN:
                player->control->down = 0;
                player->crouching = 0;
                break;
            case ALLEGRO_KEY_SPACE:
                player->control->fire = 0;
                break;
            }
        }
        else if (event.type == ALLEGRO_EVENT_TIMER)
        {
            // Movimento lateral
            if (player->control->right && !player->crouching)
            {
                moveCharacter(player, caracther_STEP, 0, X_SCREEN, Y_SCREEN);
                update_background(2.0); // Só move o fundo se andar pra direita
            }
            if (player->control->left && !player->crouching)
            {
                moveCharacter(player, caracther_STEP, 1, X_SCREEN, Y_SCREEN);
                // Não move o fundo para trás
            }

            positionUpdate(player);
            bulletUpdate(player);

            redraw = true;
        }
        if (redraw && al_is_event_queue_empty(queue))
        {
            al_clear_to_color(al_map_rgb(0, 0, 0));
            draw_background();
            // Desenha o chão
            al_draw_filled_rectangle(0, GROUND_Y, X_SCREEN, Y_SCREEN, al_map_rgb(80, 60, 40));
            drawCharacter(player, NULL, false);
            al_flip_display();
            redraw = false;
        }
    }

    al_destroy_bitmap(bg1);
    al_destroy_display(display);
    al_destroy_timer(timer);
    al_destroy_event_queue(queue);
    al_destroy_font(font);

    return (0);
}