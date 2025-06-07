#include "character.h"

Character *createCharacter(unsigned char side, unsigned char face, unsigned short x, unsigned short y, unsigned short max_x, unsigned short max_y)
{
    if ((x - side / 2 < 0) || (x + side / 2 > max_x) || (y - face / 2 < 0) || (y + face / 2 > max_y))
    {
        fprintf(stderr, "Character position out of bounds.\n");
        return NULL;
    }

    Character *newCharacter = (Character *)malloc(sizeof(Character));

    if (!newCharacter)
    {
        fprintf(stderr, "Memory allocation for Character failed.\n");
        return NULL;
    }

    // Garante que o personagem nasce em cima do chão
    unsigned short ground_y = Y_SCREEN - 48; // 48 é a altura do chão (GROUND_HEIGHT)
    if (y > ground_y)
        y = ground_y;

    newCharacter->side = side;
    newCharacter->face = face;
    newCharacter->hp = PLAYER_INITIAL_HP;
    newCharacter->x = x < max_x ? x : max_x - 1;
    newCharacter->y = y < max_y ? y : max_y - 1;
    newCharacter->jumping = 0;
    newCharacter->crouching = 0;
    newCharacter->width = 32;
    newCharacter->height = 48;
    newCharacter->control = createJoystick();
    newCharacter->gun = createPistol();

    if (!newCharacter->control || !newCharacter->gun)
    {
        free(newCharacter);
        return (NULL);
    }

    return (newCharacter);
}

void moveCharacter(Character *element, char steps, unsigned char trajectory, unsigned short max_x, unsigned short max_y)
{
    if (!element)
        return;

    int character_width = 32; // Largura real usada no drawCharacter()

    if (trajectory == 0)
    {
        // Direita
        if (element->x + steps + character_width <= max_x)
        {
            element->x += steps;
        }
        else
        {
            element->x = max_x - character_width;
        }
        element->side = 0;
    }
    else if (trajectory == 1)
    {
        // Esquerda
        if (element->x - steps >= 0)
        {
            element->x -= steps;
        }
        else
        {
            element->x = 0;
        }
        element->side = 1;
    }
}

void shotCharacter(Character *element)
{
    if (!element || !element->gun || !element->control)
        return;

    unsigned char trajectory = 0;
    unsigned short bullet_x = element->x + 16; // centro do personagem
    unsigned short bullet_y = element->y + 24; // centro vertical

    // Atira para baixo se agachado e no chão
    if (element->crouching && element->y >= Y_SCREEN - 48)
    {
        trajectory = 3; // baixo
    }
    // Atira para cima se o botão "up" estiver pressionado
    else if (element->control->up)
    {
        trajectory = 2; // cima
    }
    // Direita/esquerda normalmente
    else
    {
        trajectory = element->side; // 0 = direita, 1 = esquerda
    }

    // Garante que a bala sempre nasce dentro da tela
    if (bullet_x > X_SCREEN)
        bullet_x = X_SCREEN - 1;
    if (bullet_y > Y_SCREEN)
        bullet_y = Y_SCREEN - 1;

    bullet *new_bullet = firePistol(bullet_x, bullet_y, trajectory, element->gun);

    if (new_bullet)
    {
        new_bullet->next = element->gun->shots;
        element->gun->shots = new_bullet;
    }
}

void destroyCharacter(Character *element)
{
    if (!element)
    {
        return;
    }

    if (element->control)
    {
        destroyJoystick(element->control);
    }

    if (element->gun)
    {
        destroyPistol(element->gun);
    }

    free(element);
}

void drawCharacter(Character *player, ALLEGRO_BITMAP *spaceship_image, bool debug_mode)
{
    if (!player)
        return;

    // Usa a altura e largura atuais do personagem
    al_draw_filled_rectangle(player->x, player->y, player->x + player->width, player->y + player->height, al_map_rgb(0, 128, 255));

    if (debug_mode)
    {
        al_draw_rectangle(player->x, player->y, player->x + player->width, player->y + player->height, al_map_rgb(255, 0, 0), 2);
    }

    // Desenhe as balas
    bullet *b = player->gun->shots;

    while (b)
    {
        al_draw_filled_circle(b->x, b->y, 4, al_map_rgb(255, 255, 0));
        b = b->next;
    }
}

void positionUpdate(Character *player)
{
    if (!player)
        return;

    const int normal_height = 48;
    const int crouch_height = 28;
    const int jump_height = 120;
    const int ground_y = GROUND_Y - normal_height;
    const int jump_speed = 18;
    const int gravity = 13;

    // Pulo
    if (player->jumping)
    {
        player->y -= jump_speed;
        if (player->y <= ground_y - jump_height)
        {
            player->y = ground_y - jump_height;
            player->jumping = 0; // atingiu o topo do pulo, começa a cair
        }
    }
    else if (player->y < ground_y)
    {
        player->y += gravity;
        if (player->y > ground_y)
            player->y = ground_y;
    }

    // Agachar/desagachar: ajusta altura e topo mantendo os pés no chão
    if (player->crouching && player->height != crouch_height && player->y + player->height == GROUND_Y)
    {
        player->y += (player->height - crouch_height);
        player->height = crouch_height;
    }
    else if (!player->crouching && player->height != normal_height && player->y + player->height == GROUND_Y)
    {
        player->y -= (normal_height - player->height);
        player->height = normal_height;
    }
}

void bulletUpdate(Character *player)
{
    if (!player || !player->gun)
        return;

    bullet *prev = NULL;
    bullet *curr = player->gun->shots;

    while (curr)
    {
        // Atualiza a posição da bala conforme a trajetória
        switch (curr->trajectory)
        {
        case 0:
            curr->x += BULLET_MOVE;
            break; // direita
        case 1:
            curr->x -= BULLET_MOVE;
            break; // esquerda
        case 2:
            curr->y -= BULLET_MOVE;
            break; // cima
        case 3:
            curr->y += BULLET_MOVE;
            break; // baixo
        default:
            break;
        }

        // Remove se saiu da tela
        if (curr->x < 0 || curr->x > X_SCREEN || curr->y < 0 || curr->y > Y_SCREEN)
        {
            bullet *to_remove = curr;
            if (prev)
            {
                prev->next = curr->next;
                curr = curr->next;
            }
            else
            {
                player->gun->shots = curr->next;
                curr = player->gun->shots;
            }
            free(to_remove);
        }
        else
        {
            prev = curr;
            curr = curr->next;
        }
    }
}

void updateCharacterHp(Character *player, int delta_hp)
{
    if (!player)
        return;

    player->hp += delta_hp;

    if (player->hp > PLAYER_INITIAL_HP)
    {
        player->hp = PLAYER_INITIAL_HP;
    }
    if (player->hp < 0)
    {
        player->hp = 0;
    }

    printf("HP do jogador atualizado para: %d\n", player->hp);
}
