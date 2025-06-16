#include "enemy.h"
#include "character.h"
#include <stdlib.h>
#include <time.h>

// Prototipos das funções utilitárias de frames
void load_frames(ALLEGRO_BITMAP ***arr, const char **src, int count);
void destroy_frames(ALLEGRO_BITMAP **arr, int count);

Enemy *createEnemy(int x, int y, int width, int height, int hp)
{
    Enemy *enemy = malloc(sizeof(Enemy));
    if (!enemy)
    {
        fprintf(stderr, "Failed to allocate memory for Enemy\n");
        return NULL;
    }
    enemy->x = x;
    enemy->y = y;
    enemy->width = width;
    enemy->height = height;
    enemy->hp = hp;
    enemy->hitbox_x = x;
    enemy->hitbox_y = y - height;
    enemy->hitbox_w = width;
    enemy->hitbox_h = height;
    enemy->fire_cooldown = 0;
    enemy->direction = 1; // Direção inicial padrão (esquerda)
    enemy->gun = createPistol();
    enemy->sprite = NULL; // Não usa sprite
    return enemy;
}

void destroyEnemy(Enemy *enemy)
{
    if (!enemy)
        return;

    if (enemy->gun)
        destroyPistol(enemy->gun);

    if (enemy->sprite)
        al_destroy_bitmap(enemy->sprite);

    free(enemy);
}

void moveEnemy(Enemy *enemy, int dx, int dy, int max_x, int ground_y)
{
    if (!enemy)
        return;

    enemy->x += dx;
    enemy->y += dy;
    // Mantém o inimigo sempre sobre o chão
    enemy->y = ground_y;
    // Atualiza hitbox para base do inimigo
    enemy->hitbox_x = enemy->x;
    enemy->hitbox_y = enemy->y - enemy->height;
    enemy->hitbox_w = enemy->width;
    enemy->hitbox_h = enemy->height;
}

void bulletUpdateEnemy(Enemy *enemy)
{
    if (!enemy || !enemy->gun)
        return;
    bullet *prev = NULL;
    bullet *curr = enemy->gun->shots;
    while (curr)
    {
        // Atualiza posição da bala conforme a trajetória
        switch (curr->trajectory)
        {
        case 0: // Direita
            curr->x += BULLET_MOVE;
            break;
        case 1: // Esquerda
            curr->x -= BULLET_MOVE;
            break;
        case 2: // Para cima
            curr->y -= BULLET_MOVE;
            break;
        case 3: // Para baixo
            curr->y += BULLET_MOVE;
            break;
        }
        // Remove balas fora da tela
        if (curr->x < 0 || curr->x > X_SCREEN || curr->y < 0 || curr->y > Y_SCREEN)
        {
            bullet *to_remove = curr;
            if (prev)
                prev->next = curr->next;
            else
                enemy->gun->shots = curr->next;
            curr = curr->next;
            free(to_remove);
        }
        else
        {
            prev = curr;
            curr = curr->next;
        }
    }
}

void enemyShoot(Enemy *enemy, Character *player)
{
    if (!enemy || !enemy->gun || enemy->fire_cooldown > 0 || !player)
        return;

    // Calcula direção do tiro em relação ao personagem
    int dx = (player->x + player->width / 2) - (enemy->x + enemy->width / 2);
    int dy = (player->y - player->height / 2) - (enemy->y - enemy->height / 2);

    // Decide a trajetória: 0 = direita, 1 = esquerda, 2 = cima, 3 = baixo
    int trajectory;
    if (abs(dx) > abs(dy))
    {
        trajectory = (dx > 0) ? 0 : 1; // Direita ou esquerda
    }
    else
    {
        trajectory = (dy < 0) ? 2 : 3; // Cima ou baixo
    }

    // Posição inicial do tiro (centro do inimigo)
    int bullet_x = enemy->x + enemy->width / 2;
    int bullet_y = enemy->y - enemy->height / 2;

    bullet *new_bullet = firePistol(bullet_x, bullet_y, trajectory, enemy->gun);
    if (new_bullet)
    {
        new_bullet->next = enemy->gun->shots;
        enemy->gun->shots = new_bullet;
        enemy->fire_cooldown = 30; // Cooldown de 30 frames
    }
}

void updateEnemy(Enemy *enemy, Character *player, int ground_y)
{
    if (!enemy)
        return;

    // Remove inimigo se sair totalmente do lado esquerdo ou direito
    if (enemy->x + enemy->width < 0 || enemy->x > X_SCREEN)
    {
        enemy->hp = 0; // Marca para remoção
        return;
    }

    // Atualiza cooldown do tiro
    if (enemy->fire_cooldown > 0)
        enemy->fire_cooldown--;

    // --- Lógica de movimentação: anda para esquerda e direita, para para atirar, não pula mais ---
    static int move_timer = 0;
    static int move_state = 0; // 0 = parado, 1 = andando esquerda, 2 = andando direita

    // Decide se vai andar ou parar
    if (move_timer <= 0)
    {
        int r = rand() % 100;
        if (r < 40)
            move_state = 1; // 40% chance de andar para esquerda
        else if (r < 80)
            move_state = 2; // 40% chance de andar para direita
        else
            move_state = 0; // 20% parado
        move_timer = 60;
    }
    else
    {
        move_timer--;
    }

    // Se for atirar, para de andar
    int will_shoot = 0;
    if (enemy->fire_cooldown == 0)
    {
        // Calcula direção do tiro em relação ao personagem
        int dx = (player->x + player->width / 2) - (enemy->x + enemy->width / 2);
        // int dy = (player->y - player->height / 2) - (enemy->y - enemy->height / 2); // Removido unused
        //  Só atira se o player estiver razoavelmente próximo na horizontal
        if (abs(dx) < 400)
        {
            enemyShoot(enemy, player);
            will_shoot = 1;
        }
    }

    // Se for atirar, fica parado
    if (will_shoot)
    {
        move_state = 0;
    }

    // Anda para esquerda ou direita
    if (move_state == 1)
    {
        if (enemy->x > 0)
        {
            moveEnemy(enemy, -2, 0, X_SCREEN, ground_y);
            enemy->direction = 1;
        }
    }
    else if (move_state == 2)
    {
        if (enemy->x + enemy->width < X_SCREEN)
        {
            moveEnemy(enemy, 2, 0, X_SCREEN, ground_y);
            enemy->direction = 0;
        }
    }
    // Atualiza balas do inimigo
    bulletUpdateEnemy(enemy);
}

void drawEnemy(Enemy *enemy)
{
    if (!enemy)
        return;
    // Seleciona animação e frame conforme estado e direção
    ALLEGRO_BITMAP *sprite = NULL;
    int frame = enemy->frame;
    if (enemy->direction == 1)
    {                                                                                        // Esquerda
        if (enemy->state == 1 && enemy->walk_frames_arr_left && enemy->walk_frames_left > 0) // walk
            sprite = enemy->walk_frames_arr_left[frame % enemy->walk_frames_left];
        else if (enemy->idle_frames_arr_left && enemy->idle_frames_left > 0) // idle
            sprite = enemy->idle_frames_arr_left[frame % enemy->idle_frames_left];
    }
    else
    { // Direita
        if (enemy->state == 1 && enemy->walk_frames_arr_right && enemy->walk_frames_right > 0)
            sprite = enemy->walk_frames_arr_right[frame % enemy->walk_frames_right];
        else if (enemy->idle_frames_arr_right && enemy->idle_frames_right > 0)
            sprite = enemy->idle_frames_arr_right[frame % enemy->idle_frames_right];
    }
    if (sprite)
        al_draw_bitmap(sprite, enemy->x, enemy->y - enemy->height, 0);
    else
        al_draw_filled_rectangle(
            enemy->x, enemy->y - enemy->height,
            enemy->x + enemy->width, enemy->y,
            al_map_rgb(200, 50, 50));
    // Desenha a hitbox para debug
    al_draw_rectangle(
        enemy->hitbox_x, enemy->hitbox_y,
        enemy->hitbox_x + enemy->hitbox_w,
        enemy->hitbox_y + enemy->hitbox_h,
        al_map_rgb(255, 255, 0), 2);
    // Desenha as balas do inimigo
    if (enemy->gun)
        drawBullets(enemy->gun->shots);
}

void checkPlayerBulletHitsEnemy(Character *player, Enemy *enemy)
{
    if (!player || !enemy || !player->gun)
        return;
    bullet *prev = NULL;
    bullet *curr = player->gun->shots;
    while (curr)
    {
        // Checa colisão da bala com a hitbox do inimigo
        if (curr->x >= enemy->hitbox_x && curr->x <= enemy->hitbox_x + enemy->hitbox_w &&
            curr->y >= enemy->hitbox_y && curr->y <= enemy->hitbox_y + enemy->hitbox_h)
        {
            // Dano
            enemy->hp -= curr->damage;
            // Remove bala
            bullet *to_remove = curr;
            if (prev)
                prev->next = curr->next;
            else
                player->gun->shots = curr->next;
            curr = curr->next;
            free(to_remove);
            continue;
        }
        prev = curr;
        curr = curr->next;
    }
}

void checkEnemyBulletHitsPlayer(Enemy *enemy, Character *player)
{
    if (!enemy || !player || !enemy->gun)
        return;
    bullet *prev = NULL;
    bullet *curr = enemy->gun->shots;
    while (curr)
    {
        if (curr->x >= player->hitbox_x && curr->x <= player->hitbox_x + player->hitbox_w &&
            curr->y >= player->hitbox_y && curr->y <= player->hitbox_y + player->hitbox_h)
        {
            updateCharacterHp(player, -curr->damage);
            bullet *to_remove = curr;
            if (prev)
                prev->next = curr->next;
            else
                enemy->gun->shots = curr->next;
            curr = curr->next;
            free(to_remove);
            continue;
        }
        prev = curr;
        curr = curr->next;
    }
}

int checkEnemyPlayerCollision(Enemy *enemy, Character *player)
{
    if (!enemy || !player)
        return 0;
    return (enemy->hitbox_x < player->hitbox_x + player->hitbox_w &&
            enemy->hitbox_x + enemy->hitbox_w > player->hitbox_x &&
            enemy->hitbox_y < player->hitbox_y + player->hitbox_h &&
            enemy->hitbox_y + enemy->hitbox_h > player->hitbox_y);
}

void loadEnemySprites(Enemy *enemy,
                      const char **walk_right, int walk_frames_right,
                      const char **idle_right, int idle_frames_right,
                      const char **walk_shoot_right, int walk_shoot_frames_right,
                      const char **idle_shoot_right, int idle_shoot_frames_right,
                      const char **walk_left, int walk_frames_left,
                      const char **idle_left, int idle_frames_left,
                      const char **walk_shoot_left, int walk_shoot_frames_left,
                      const char **idle_shoot_left, int idle_shoot_frames_left)
{
    load_frames(&enemy->walk_frames_arr_right, walk_right, walk_frames_right);
    load_frames(&enemy->idle_frames_arr_right, idle_right, idle_frames_right);
    load_frames(&enemy->walk_shoot_frames_arr_right, walk_shoot_right, walk_shoot_frames_right);
    load_frames(&enemy->idle_shoot_frames_arr_right, idle_shoot_right, idle_shoot_frames_right);
    load_frames(&enemy->walk_frames_arr_left, walk_left, walk_frames_left);
    load_frames(&enemy->idle_frames_arr_left, idle_left, idle_frames_left);
    load_frames(&enemy->walk_shoot_frames_arr_left, walk_shoot_left, walk_shoot_frames_left);
    load_frames(&enemy->idle_shoot_frames_arr_left, idle_shoot_left, idle_shoot_frames_left);
    enemy->walk_frames_right = walk_frames_right;
    enemy->idle_frames_right = idle_frames_right;
    enemy->walk_shoot_frames_right = walk_shoot_frames_right;
    enemy->idle_shoot_frames_right = idle_shoot_frames_right;
    enemy->walk_frames_left = walk_frames_left;
    enemy->idle_frames_left = idle_frames_left;
    enemy->walk_shoot_frames_left = walk_shoot_frames_left;
    enemy->idle_shoot_frames_left = idle_shoot_frames_left;
}

void destroyEnemySprites(Enemy *enemy)
{
    destroy_frames(enemy->walk_frames_arr_right, enemy->walk_frames_right);
    destroy_frames(enemy->idle_frames_arr_right, enemy->idle_frames_right);
    destroy_frames(enemy->walk_shoot_frames_arr_right, enemy->walk_shoot_frames_right);
    destroy_frames(enemy->idle_shoot_frames_arr_right, enemy->idle_shoot_frames_right);
    destroy_frames(enemy->walk_frames_arr_left, enemy->walk_frames_left);
    destroy_frames(enemy->idle_frames_arr_left, enemy->idle_frames_left);
    destroy_frames(enemy->walk_shoot_frames_arr_left, enemy->walk_shoot_frames_left);
    destroy_frames(enemy->idle_shoot_frames_arr_left, enemy->idle_shoot_frames_left);
}
