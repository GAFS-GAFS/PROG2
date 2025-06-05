#include "controls.h"

void controlsDisplay(ALLEGRO_DISPLAY *display, ALLEGRO_FONT *font, ALLEGRO_EVENT_QUEUE *queue)
{
    ALLEGRO_EVENT event;

    al_clear_to_color(al_map_rgb(0, 0, 0)); // Limpa a tela com cor preta

    al_draw_text(font, al_map_rgb(255, 255, 255), al_get_display_width(display) / 2, al_get_display_height(display) / 2 - 120, ALLEGRO_ALIGN_CENTER, "Pressione 'ENTER' para voltar");
    al_draw_text(font, al_map_rgb(255, 255, 255), al_get_display_width(display) / 2, al_get_display_height(display) / 2 - 90, ALLEGRO_ALIGN_CENTER, "Controles");
    al_draw_text(font, al_map_rgb(255, 255, 255), al_get_display_width(display) / 2, al_get_display_height(display) / 2 - 60, ALLEGRO_ALIGN_CENTER, "Tecla W - Pula");
    al_draw_text(font, al_map_rgb(255, 255, 255), al_get_display_width(display) / 2, al_get_display_height(display) / 2 - 30, ALLEGRO_ALIGN_CENTER, "Tecla A - Esquerda");
    al_draw_text(font, al_map_rgb(255, 255, 255), al_get_display_width(display) / 2, al_get_display_height(display) / 2, ALLEGRO_ALIGN_CENTER, "Tecla S - Agacha");
    al_draw_text(font, al_map_rgb(255, 255, 255), al_get_display_width(display) / 2, al_get_display_height(display) / 2 + 30, ALLEGRO_ALIGN_CENTER, "Tecla D - Direita");
    al_draw_text(font, al_map_rgb(255, 255, 255), al_get_display_width(display) / 2, al_get_display_height(display) / 2 + 60, ALLEGRO_ALIGN_CENTER, "Tecla C - Disparar");

    al_flip_display(); // Atualiza a tela para mostrar as instruções

    while (1)
    {
        al_wait_for_event(queue, &event);
        if (event.type == ALLEGRO_EVENT_KEY_DOWN)
        {
            if (event.keyboard.keycode == ALLEGRO_KEY_ESCAPE)
            {
                al_clear_to_color(al_map_rgb(0, 0, 0)); // Limpa a tela com cor preta
                break;
            }

            if (event.keyboard.keycode == ALLEGRO_KEY_ENTER)
            {
                al_clear_to_color(al_map_rgb(0, 0, 0)); // Limpa a tela com cor preta
                return;
            }
        }

        else if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
        {
            break; // sai e fecha o display
        }
    }
}