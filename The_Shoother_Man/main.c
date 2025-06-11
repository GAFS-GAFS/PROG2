#include "menu.h"
#include "background.h"
#include "character.h"
#include "bullet.h"

#define X_SCREEN 800
#define Y_SCREEN 600

int main()
{
    int ground_height = 150;
    int ground_y = Y_SCREEN - ground_height;

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

    load_bullet_sprite("./imagens/bullet.png");

    al_register_event_source(queue, al_get_keyboard_event_source());
    al_register_event_source(queue, al_get_display_event_source(display));
    al_register_event_source(queue, al_get_timer_event_source(timer));

    menuDisplay(display, font, queue);

    init_background("./imagens/subway_BG.png");

    // Cria o personagem na esquerda (mas não na extrema esquerda) e perfeitamente em cima do chão
    int player_height = 48;
    int player_x = 32;
    Character *player = createCharacter(32, player_height, player_x, ground_y, X_SCREEN, Y_SCREEN, ground_y);

    loadCharacterSprites(player,
                         "./imagens/walking.png", 6,
                         "./imagens/jumping.png", 1,
                         "./imagens/crouching.png", 1,
                         "./imagens/stop.png", 1,
                         "./imagens/shootingw.png", 8,
                         "./imagens/shootingj.png", 2,
                         "./imagens/shootingc.png", 2,
                         "./imagens/shootings.png", 2);

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
                if (!player->jumping && !player->crouching && player->y == ground_y)
                    player->jumping = 1;
                break;
            case ALLEGRO_KEY_DOWN:
                if (!player->jumping && player->y == ground_y)
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
            if (player->control->right && !player->crouching)
            {
                moveCharacter(player, caracther_STEP, 0, X_SCREEN, Y_SCREEN);
                update_background(2.0);
            }
            if (player->control->left && !player->crouching)
            {
                moveCharacter(player, caracther_STEP, 1, X_SCREEN, Y_SCREEN);
            }

            positionUpdate(player, ground_y, ground_height);
            bulletUpdate(player);

            updateCharacterState(player);

            redraw = true;
        }
        if (redraw && al_is_event_queue_empty(queue))
        {
            al_clear_to_color(al_map_rgb(0, 0, 0));
            draw_background();
            drawCharacter(player, NULL, false);
            draw_life_bar(player); // Desenha a barra de vida
            al_flip_display();
            redraw = false;
        }
    }

    al_destroy_bitmap(bg1);
    al_destroy_display(display);
    al_destroy_timer(timer);
    al_destroy_event_queue(queue);
    al_destroy_font(font);
    destroyCharacterSprites(player);
    destroy_bullet_sprite();

    return (0);
}