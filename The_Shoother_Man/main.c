#include "menu.h"
#include "background.h"
#include "character.h"
#include "bullet.h"
#include "enemy.h"

#define X_SCREEN 800
#define Y_SCREEN 600

// Variável global para controle de tiro para cima
int shooting_up = 0;

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
    load_lbullet_sprite("./imagens/Lbullet.png");

    al_register_event_source(queue, al_get_keyboard_event_source());
    al_register_event_source(queue, al_get_display_event_source(display));
    al_register_event_source(queue, al_get_timer_event_source(timer));

    menuDisplay(display, font, queue);

    init_background("./imagens/subway_BG.png");

    // Cria o personagem na esquerda (mas não na extrema esquerda) e perfeitamente em cima do chão
    int player_height = 48;
    int player_x = 32;
    Character *player = createCharacter(32, player_height, player_x, ground_y, X_SCREEN, Y_SCREEN, ground_y);

    // Verificação e alocação de player e player->control
    if (!player)
    {
        fprintf(stderr, "Erro ao criar personagem!\n");
        return -1;
    }
    if (!player->control)
    {
        player->control = malloc(sizeof(*(player->control)));
        if (!player->control)
        {
            fprintf(stderr, "Erro ao alocar controles!\n");
            return -1;
        }
        memset(player->control, 0, sizeof(*(player->control)));
    }

    const char *walk_imgs_right[] = {
        "./imagens/Character3.png", "./imagens/Character4.png", "./imagens/Character5.png",
        "./imagens/Character6.png", "./imagens/Character7.png", "./imagens/Character8.png", "./imagens/Character9.png"};
    const char *jump_imgs_right[] = {"./imagens/Character11.png"};
    const char *crouch_imgs_right[] = {"./imagens/Character10.png"};
    const char *idle_imgs_right[] = {"./imagens/Character1.png", "./imagens/Character2.png"};
    const char *walk_shoot_imgs_right[] = {"./imagens/Character14.png", "./imagens/Character17.png", "./imagens/Character18.png",
                                           "./imagens/Character19.png", "./imagens/Character20.png", "./imagens/Character21.png",
                                           "./imagens/Character22.png", "./imagens/Character23.png", "./imagens/Character24.png"};
    const char *jump_shoot_imgs_right[] = {"./imagens/Character25.png", "./imagens/Character26.png"};
    const char *crouch_shoot_imgs_right[] = {"./imagens/Character15.png", "./imagens/Character16.png"};
    const char *idle_shoot_imgs_right[] = {
        "./imagens/Character12.png",
        "./imagens/Character13.png",
    };

    const char *walk_imgs_left[] = {
        "./imagens/LCharacter3.png", "./imagens/LCharacter4.png", "./imagens/LCharacter5.png",
        "./imagens/LCharacter6.png", "./imagens/LCharacter7.png", "./imagens/LCharacter8.png", "./imagens/LCharacter9.png"};
    const char *jump_imgs_left[] = {"./imagens/LCharacter11.png"};
    const char *crouch_imgs_left[] = {"./imagens/LCharacter10.png"};
    const char *idle_imgs_left[] = {"./imagens/LCharacter1.png", "./imagens/LCharacter2.png"};
    const char *walk_shoot_imgs_left[] = {"./imagens/LCharacter14.png", "./imagens/LCharacter17.png", "./imagens/LCharacter18.png",
                                          "./imagens/LCharacter19.png", "./imagens/LCharacter20.png", "./imagens/LCharacter21.png",
                                          "./imagens/LCharacter22.png", "./imagens/LCharacter23.png", "./imagens/LCharacter24.png"};
    const char *jump_shoot_imgs_left[] = {"./imagens/LCharacter25.png", "./imagens/LCharacter26.png"};
    const char *crouch_shoot_imgs_left[] = {"./imagens/LCharacter15.png", "./imagens/LCharacter16.png"};
    const char *idle_shoot_imgs_left[] = {
        "./imagens/LCharacter12.png",
        "./imagens/LCharacter13.png",
    };

    const char *walk_shoot_up_imgs_right[] = {
        "./imagens/Character27.png",
        "./imagens/Character28.png",
        "./imagens/Character29.png",
        "./imagens/Character30.png",
        "./imagens/Character31.png",
        "./imagens/Character32.png",
        "./imagens/Character33.png",
        "./imagens/Character34.png",
        "./imagens/Character35.png"};

    const char *idle_shoot_up_imgs_right[] = {
        "./imagens/Character38.png", "./imagens/Character39.png"};
    const char *jump_shoot_up_imgs_right[] = {
        "./imagens/Character36.png", "./imagens/Character37.png"};

    const char *walk_shoot_up_imgs_left[] = {
        "./imagens/LCharacter27.png",
        "./imagens/LCharacter28.png",
        "./imagens/LCharacter29.png",
        "./imagens/LCharacter30.png",
        "./imagens/LCharacter31.png",
        "./imagens/LCharacter32.png",
        "./imagens/LCharacter33.png",
        "./imagens/LCharacter34.png",
        "./imagens/LCharacter35.png"};
    const char *idle_shoot_up_imgs_left[] = {
        "./imagens/LCharacter38.png", "./imagens/LCharacter39.png"};
    const char *jump_shoot_up_imgs_left[] = {
        "./imagens/LCharacter36.png", "./imagens/LCharacter37.png"};

    loadCharacterSprites(player,
                         walk_imgs_right, 7,
                         jump_imgs_right, 1,
                         crouch_imgs_right, 1,
                         idle_imgs_right, 2,
                         walk_shoot_imgs_right, 9,
                         jump_shoot_imgs_right, 2,
                         crouch_shoot_imgs_right, 2,
                         idle_shoot_imgs_right, 2,
                         walk_shoot_up_imgs_right, 9,
                         jump_shoot_up_imgs_right, 2,
                         idle_shoot_up_imgs_right, 2,
                         walk_imgs_left, 7,
                         jump_imgs_left, 1,
                         crouch_imgs_left, 1,
                         idle_imgs_left, 2,
                         walk_shoot_imgs_left, 9,
                         jump_shoot_imgs_left, 2,
                         crouch_shoot_imgs_left, 2,
                         idle_shoot_imgs_left, 2,
                         walk_shoot_up_imgs_left, 9,
                         jump_shoot_up_imgs_left, 2,
                         idle_shoot_up_imgs_left, 2);

    // Arrays de caminhos para cada frame de cada animação do inimigo (direita)
    const char *enemy_walk_imgs_right[] = {"./imagens/Enemy4.png", "./imagens/Enemy5.png",
                                           "./imagens/Enemy6.png", "./imagens/Enemy7.png", "./imagens/Enemy8.png",
                                           "./imagens/Enemy9.png", "./imagens/Enemy10.png", "./imagens/Enemy11.png"};
    const char *enemy_idle_imgs_right[] = {"./imagens/Enemy1.png"};
    const char *enemy_idle_shoot_imgs_right[] = {"./imagens/Enemy2.png", "./imagens/Enemy3.png"};
    // Arrays de caminhos para cada frame de cada animação do inimigo (esquerda)
    const char *enemy_walk_imgs_left[] = {"./imagens/LEnemy4.png", "./imagens/LEnemy5.png",
                                          "./imagens/LEnemy6.png", "./imagens/LEnemy7.png", "./imagens/LEnemy8.png",
                                          "./imagens/LEnemy9.png", "./imagens/LEnemy10.png", "./imagens/LEnemy11.png"};
    const char *enemy_idle_imgs_left[] = {"./imagens/LEnemy1.png"};
    const char *enemy_idle_shoot_imgs_left[] = {"./imagens/LEnemy2.png", "./imagens/LEnemy3.png"};

    int enemies_spawned = 0;
    int max_enemies = 6;
    Enemy *enemy = createEnemy(600, ground_y, 32, 48, 50);
    // Carrega as sprites do inimigo APÓS criar o Enemy
    loadEnemySprites(enemy,
                     enemy_walk_imgs_right, 8,
                     enemy_idle_imgs_right, 1,
                     NULL, 0, // walk_shoot_right
                     enemy_idle_shoot_imgs_right, 2,
                     enemy_walk_imgs_left, 8,
                     enemy_idle_imgs_left, 1,
                     NULL, 0, // walk_shoot_left
                     enemy_idle_shoot_imgs_left, 1);

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
                break;
            case ALLEGRO_KEY_V:
                shooting_up = 1;
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
            case ALLEGRO_KEY_V:
                shooting_up = 0;
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
            // Disparo automático enquanto segura espaço ou V
            if ((player->control->fire || shooting_up))
            {
                if (player->fire_cooldown <= 0)
                {
                    if (shooting_up)
                    {
                        shotCharacterUp(player);
                        // printf("Atirando para cima!\n");
                    }
                    else
                    {
                        shotCharacter(player);
                        // printf("Atirando normal!\n");
                    }
                    player->fire_cooldown = 7;
                }
            }
            if (player->fire_cooldown > 0)
                player->fire_cooldown--;
            positionUpdate(player, ground_y, ground_height);
            bulletUpdate(player);
            updateCharacterState(player);
            if (enemy)
            {
                updateEnemy(enemy, player, ground_y);
                updateEnemyStateAndFrame(enemy);
                checkPlayerBulletHitsEnemy(player, enemy);
                checkEnemyBulletHitsPlayer(enemy, player);
                if (checkEnemyPlayerCollision(enemy, player))
                {
                    updateCharacterHp(player, -2); // Dano por contato
                }
                if (enemy->hp <= 0)
                {
                    destroyEnemySprites(enemy);
                    destroyEnemy(enemy);
                    enemy = NULL;
                }
            }
            // Spawna novo inimigo se o anterior morreu e não atingiu o limite
            if (!enemy && enemies_spawned < max_enemies)
            {
                int min_x = X_SCREEN / 2;
                int max_x = X_SCREEN - 32;
                int spawn_x = min_x + rand() % (max_x - min_x + 1);
                enemy = createEnemy(spawn_x, ground_y, 32, 48, 50);
                loadEnemySprites(enemy,
                                 enemy_walk_imgs_right, 8,
                                 enemy_idle_imgs_right, 1,
                                 NULL, 0, // walk_shoot_right
                                 enemy_idle_shoot_imgs_right, 2,
                                 enemy_walk_imgs_left, 8,
                                 enemy_idle_imgs_left, 1,
                                 NULL, 0, // walk_shoot_left
                                 enemy_idle_shoot_imgs_left, 1);
                printf("personagem nascido\n");
                enemies_spawned++;
            }
            if (player->hp <= 0)
            {
                ALLEGRO_BITMAP *gameover_img = al_load_bitmap("./imagens/tsmGO.png");
                al_clear_to_color(al_map_rgb(255, 255, 255));
                if (gameover_img)
                {
                    int img_w = al_get_bitmap_width(gameover_img);
                    int img_h = al_get_bitmap_height(gameover_img);
                    al_draw_bitmap(gameover_img, (X_SCREEN - img_w) / 2, (Y_SCREEN - img_h) / 2, 0);
                    al_destroy_bitmap(gameover_img);
                }
                else
                {
                    al_draw_text(font, al_map_rgb(255, 0, 0), X_SCREEN / 2, Y_SCREEN / 2 - 20, ALLEGRO_ALIGN_CENTER, "GAME OVER");
                }
                al_flip_display();
                al_rest(2.0);
                running = false;
                continue;
            }
            redraw = true;
        }
        if (redraw && al_is_event_queue_empty(queue))
        {
            al_clear_to_color(al_map_rgb(0, 0, 0));
            draw_background();
            drawCharacter(player, NULL, false);
            if (enemy)
                drawEnemy(enemy);  // Desenha o inimigo
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
    // Libera as sprites do inimigo antes de destruir o Enemy
    if (enemy)
        destroyEnemySprites(enemy);
    destroyEnemy(enemy);

    return (0);
}