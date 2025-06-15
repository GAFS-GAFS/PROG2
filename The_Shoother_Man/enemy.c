#include "enemy.h"
#include "character.h"
#include <stdlib.h>
#include <time.h>

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

    // Remove inimigo se sair totalmente do lado esquerdo
    if (enemy->x + enemy->width < 0)
    {
        enemy->hp = 0; // Marca para remoção
        return;
    }

    // Atualiza cooldown do tiro
    if (enemy->fire_cooldown > 0)
        enemy->fire_cooldown--;

    // Atira mirando no player
    enemyShoot(enemy, player);

    // --- Lógica de movimentação: anda da direita para a esquerda ---
    static int move_timer = 0;
    static int move_state = 0; // 0 = parado, 1 = andando, 2 = pulando
    static int jump_timer = 0;

    if (move_timer <= 0)
    {
        int r = rand() % 100;
        if (r < 60)
            move_state = 1; // 60% chance de andar
        else if (r < 80)
            move_state = 0; // 20% parado
        else
            move_state = 2; // 20% pular
        move_timer = 60;
    }
    else
    {
        move_timer--;
    }
    if (move_state == 1)
    {
        // Anda para a esquerda
        if (enemy->x > -enemy->width)
            moveEnemy(enemy, -2, 0, X_SCREEN, ground_y);
    }
    else if (move_state == 2 && jump_timer == 0)
    {
        jump_timer = 30;
    }
    if (jump_timer > 0)
    {
        if (jump_timer > 15)
            moveEnemy(enemy, 0, -4, X_SCREEN, enemy->y - 4);
        else
            moveEnemy(enemy, 0, 4, X_SCREEN, ground_y);
        jump_timer--;
    }
    // Atualiza balas do inimigo
    bulletUpdateEnemy(enemy);
}

void drawEnemy(Enemy *enemy)
{
    if (!enemy)
        return;

    // Desenha o inimigo como um retângulo simples
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
