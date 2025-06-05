#include "menu.h"
#include "controls.h"

void menuDisplay(ALLEGRO_DISPLAY *display, ALLEGRO_FONT *font, ALLEGRO_EVENT_QUEUE *queue)
{
    if (!al_init_image_addon())
    {
        fprintf(stderr, "Falha ao inicializar addon de imagem!\n");
        exit(1);
    }

    if (!al_init_font_addon())
    {
        fprintf(stderr, "Falha ao inicializar addon de fonte!\n");
        exit(1);
    }
    if (!al_init_ttf_addon())
    {
        fprintf(stderr, "Falha ao inicializar addon de fonte TTF!\n");
        exit(1);
    }

    ALLEGRO_BITMAP *bgm = al_load_bitmap("./imagens/tsm.png"); // carrega imagem do background do menu;

    if (!bgm)
    {
        fprintf(stderr, "Erro ao carregar imagem do menu!\n");
        exit(1);
    }

    int display_width = al_get_display_width(display);
    int display_height = al_get_display_height(display);
    int background_width = al_get_bitmap_width(bgm);
    int background_height = al_get_bitmap_height(bgm);

    float x_pos = (display_width - background_width) / 2;
    float y_pos = (display_height - background_height) / 2;

    al_clear_to_color(al_map_rgb(255, 255, 255));
    al_draw_bitmap(bgm, x_pos, y_pos, 0);

    menuOption option = MENU_START;

    ALLEGRO_EVENT event;

    while (1)
    {
        al_clear_to_color(al_map_rgb(255, 255, 255));
        al_draw_bitmap(bgm, x_pos, y_pos, 0);

        al_draw_text(font, option == MENU_START ? al_map_rgb(255, 0, 0) : al_map_rgb(0, 0, 0), display_width / 2, display_height / 2 + 90, ALLEGRO_ALIGN_CENTER, "Iniciar");
        al_draw_text(font, option == MENU_CONTROLS ? al_map_rgb(255, 0, 0) : al_map_rgb(0, 0, 0), display_width / 2, display_height / 2 + 120, ALLEGRO_ALIGN_CENTER, "Controles");
        al_draw_text(font, option == MENU_EXIT ? al_map_rgb(255, 0, 0) : al_map_rgb(0, 0, 0), display_width / 2, display_height / 2 + 150, ALLEGRO_ALIGN_CENTER, "Sair");

        al_flip_display();
        al_wait_for_event(queue, &event);

        if (event.type == ALLEGRO_EVENT_KEY_DOWN)
        {
            if (event.keyboard.keycode == ALLEGRO_KEY_UP)
            {
                option = (option == MENU_START) ? MENU_EXIT : option - 1;
            }

            if (event.keyboard.keycode == ALLEGRO_KEY_DOWN)
            {
                option = (option == MENU_EXIT) ? MENU_START : option + 1;
            }

            if (event.keyboard.keycode == ALLEGRO_KEY_ENTER)
            {
                if (option == MENU_START)
                {
                    // Aqui você pode iniciar o jogo
                    break; // Sai do loop do menu
                }
                else if (option == MENU_CONTROLS)
                {
                    controlsDisplay(display, font, queue);
                }
                else if (option == MENU_EXIT)
                {

                    al_destroy_bitmap(bgm);
                    al_destroy_event_queue(queue);
                    al_destroy_font(font);
                    al_destroy_display(display);
                    exit(0);
                }
                if (event.keyboard.keycode == ALLEGRO_KEY_ESCAPE)
                {
                    al_destroy_bitmap(bgm);
                    al_destroy_event_queue(queue);
                    al_destroy_font(font);
                    al_destroy_display(display);
                    exit(0);
                }
            }
        }
        if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
        {
            al_destroy_bitmap(bgm);
            al_destroy_event_queue(queue);
            al_destroy_font(font);
            al_destroy_display(display);
            exit(0); // Sai do programa
        }
    }
}