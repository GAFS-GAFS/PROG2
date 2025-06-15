#include "character.h"
#include "bullet.h"

Character *createCharacter(unsigned char side, unsigned char face, unsigned short x, unsigned short y, unsigned short max_x, unsigned short max_y, int ground_y)
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

    // Use exatamente os valores de x e y recebidos
    newCharacter->side = side;
    newCharacter->face = face;
    newCharacter->hp = PLAYER_INITIAL_HP;
    newCharacter->x = x;
    newCharacter->y = y;
    newCharacter->jumping = 0;
    newCharacter->crouching = 0;
    newCharacter->width = 32;
    newCharacter->height = 48;
    newCharacter->control = createJoystick();
    newCharacter->gun = createPistol();
    newCharacter->fire_cooldown = 0;

    if (!newCharacter->control || !newCharacter->gun)
    {
        free(newCharacter);
        return NULL;
    }

    // Inicializa a hitbox
    newCharacter->hitbox_x = newCharacter->x;
    newCharacter->hitbox_y = newCharacter->y - newCharacter->height;
    newCharacter->hitbox_w = newCharacter->width;
    newCharacter->hitbox_h = newCharacter->height;

    return newCharacter;
}

void moveCharacter(Character *element, char steps, unsigned char trajectory, unsigned short max_x, unsigned short max_y)
{
    if (!element)
        return;

    int character_width = 32;

    if (trajectory == 0)
    {
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

    // Atualize a hitbox após mover
    element->hitbox_x = element->x;
    element->hitbox_y = element->y - element->height;
    element->hitbox_w = element->width;
    element->hitbox_h = element->height;
}

void shotCharacter(Character *element)
{
    if (!element || !element->gun || !element->control)
        return;

    unsigned char trajectory = 0;
    int bullet_width = 16;
    int bullet_height = 16;
    unsigned short bullet_x = element->x + element->width / 2 - bullet_width / 2;
    unsigned short bullet_y = element->y - element->height / 2 - bullet_height / 2;

    if (element->crouching)
    {
        trajectory = 3;
        bullet_y = element->y - bullet_height / 2;
        if ((int)bullet_y < 0)
            bullet_y = 0;
    }
    else if (element->control->up)
    {
        trajectory = 2;
        bullet_y = element->y - element->height - bullet_height / 2;
        if ((int)bullet_y < 0)
            bullet_y = 0;
    }
    else
    {
        trajectory = element->side;
    }

    if (bullet_x > X_SCREEN)
    {
        bullet_x = X_SCREEN - 1;
    }

    if (bullet_y > Y_SCREEN)
    {
        bullet_y = Y_SCREEN - 1;
    }

    if (bullet_x < X_SCREEN && bullet_y < Y_SCREEN)
    {
        bullet *new_bullet = firePistol(bullet_x, bullet_y, trajectory, element->gun);

        if (new_bullet)
        {
            new_bullet->next = element->gun->shots;
            element->gun->shots = new_bullet;
        }
    }
}

void destroyCharacter(Character *element)
{
    if (!element)
        return;

    if (element->control)
        destroyJoystick(element->control);

    if (element->gun)
        destroyPistol(element->gun);

    free(element);
}

void load_frames(ALLEGRO_BITMAP ***arr, const char **src, int count)
{
    if (count <= 0)
    {
        *arr = NULL;
        return;
    }
    *arr = malloc(sizeof(ALLEGRO_BITMAP *) * count);
    for (int i = 0; i < count; ++i)
    {
        (*arr)[i] = al_load_bitmap(src[i]);
    }
}

void destroy_frames(ALLEGRO_BITMAP **arr, int count)
{
    if (!arr)
        return;
    for (int i = 0; i < count; ++i)
    {
        if (arr[i])
        {
            al_destroy_bitmap(arr[i]);
            arr[i] = NULL;
        }
    }
    free(arr);
}

void loadCharacterSprites(
    Character *ch,
    const char **walk_right, int walk_frames,
    const char **jump_right, int jump_frames,
    const char **crouch_right, int crouch_frames,
    const char **idle_right, int idle_frames,
    const char **walk_shoot_right, int walk_shoot_frames,
    const char **jump_shoot_right, int jump_shoot_frames,
    const char **crouch_shoot_right, int crouch_shoot_frames,
    const char **idle_shoot_right, int idle_shoot_frames,
    const char **walk_left, int walk_frames_left,
    const char **jump_left, int jump_frames_left,
    const char **crouch_left, int crouch_frames_left,
    const char **idle_left, int idle_frames_left,
    const char **walk_shoot_left, int walk_shoot_frames_left,
    const char **jump_shoot_left, int jump_shoot_frames_left,
    const char **crouch_shoot_left, int crouch_shoot_frames_left,
    const char **idle_shoot_left, int idle_shoot_frames_left)
{
    load_frames(&ch->walk_frames_arr_right, walk_right, walk_frames);
    load_frames(&ch->jump_frames_arr_right, jump_right, jump_frames);
    load_frames(&ch->crouch_frames_arr_right, crouch_right, crouch_frames);
    load_frames(&ch->idle_frames_arr_right, idle_right, idle_frames);
    load_frames(&ch->walk_shoot_frames_arr_right, walk_shoot_right, walk_shoot_frames);
    load_frames(&ch->jump_shoot_frames_arr_right, jump_shoot_right, jump_shoot_frames);
    load_frames(&ch->crouch_shoot_frames_arr_right, crouch_shoot_right, crouch_shoot_frames);
    load_frames(&ch->idle_shoot_frames_arr_right, idle_shoot_right, idle_shoot_frames);

    load_frames(&ch->walk_frames_arr_left, walk_left, walk_frames_left);
    load_frames(&ch->jump_frames_arr_left, jump_left, jump_frames_left);
    load_frames(&ch->crouch_frames_arr_left, crouch_left, crouch_frames_left);
    load_frames(&ch->idle_frames_arr_left, idle_left, idle_frames_left);
    load_frames(&ch->walk_shoot_frames_arr_left, walk_shoot_left, walk_shoot_frames_left);
    load_frames(&ch->jump_shoot_frames_arr_left, jump_shoot_left, jump_shoot_frames_left);
    load_frames(&ch->crouch_shoot_frames_arr_left, crouch_shoot_left, crouch_shoot_frames_left);
    load_frames(&ch->idle_shoot_frames_arr_left, idle_shoot_left, idle_shoot_frames_left);

    ch->walk_frames_right = walk_frames;
    ch->jump_frames_right = jump_frames;
    ch->crouch_frames_right = crouch_frames;
    ch->idle_frames_right = idle_frames;
    ch->walk_shoot_frames_right = walk_shoot_frames;
    ch->jump_shoot_frames_right = jump_shoot_frames;
    ch->crouch_shoot_frames_right = crouch_shoot_frames;
    ch->idle_shoot_frames_right = idle_shoot_frames;

    ch->walk_frames_left = walk_frames_left;
    ch->jump_frames_left = jump_frames_left;
    ch->crouch_frames_left = crouch_frames_left;
    ch->idle_frames_left = idle_frames_left;
    ch->walk_shoot_frames_left = walk_shoot_frames_left;
    ch->jump_shoot_frames_left = jump_shoot_frames_left;
    ch->crouch_shoot_frames_left = crouch_shoot_frames_left;
    ch->idle_shoot_frames_left = idle_shoot_frames_left;
}

void destroyCharacterSprites(Character *ch)
{
    destroy_frames(ch->walk_frames_arr_right, ch->walk_frames_right);
    destroy_frames(ch->jump_frames_arr_right, ch->jump_frames_right);
    destroy_frames(ch->crouch_frames_arr_right, ch->crouch_frames_right);
    destroy_frames(ch->idle_frames_arr_right, ch->idle_frames_right);
    destroy_frames(ch->walk_shoot_frames_arr_right, ch->walk_shoot_frames_right);
    destroy_frames(ch->jump_shoot_frames_arr_right, ch->jump_shoot_frames_right);
    destroy_frames(ch->crouch_shoot_frames_arr_right, ch->crouch_shoot_frames_right);
    destroy_frames(ch->idle_shoot_frames_arr_right, ch->idle_shoot_frames_right);

    destroy_frames(ch->walk_frames_arr_left, ch->walk_frames_left);
    destroy_frames(ch->jump_frames_arr_left, ch->jump_frames_left);
    destroy_frames(ch->crouch_frames_arr_left, ch->crouch_frames_left);
    destroy_frames(ch->idle_frames_arr_left, ch->idle_frames_left);
    destroy_frames(ch->walk_shoot_frames_arr_left, ch->walk_shoot_frames_left);
    destroy_frames(ch->jump_shoot_frames_arr_left, ch->jump_shoot_frames_left);
    destroy_frames(ch->crouch_shoot_frames_arr_left, ch->crouch_shoot_frames_left);
    destroy_frames(ch->idle_shoot_frames_arr_left, ch->idle_shoot_frames_left);
}

// Função de desenho considerando movimento + tiro e direção
void drawCharacter(Character *ch, ALLEGRO_BITMAP *default_sprite, bool flip)
{
    ALLEGRO_BITMAP *sprite = NULL;
    int is_left = (ch->side == 1);
    int frames_count = 1;
    ALLEGRO_BITMAP **frames_arr = NULL;

    if (ch->shooting)
    {
        switch (ch->state)
        {
        case CHAR_STATE_WALK:
            frames_arr = is_left ? ch->walk_shoot_frames_arr_left : ch->walk_shoot_frames_arr_right;
            frames_count = is_left ? ch->walk_shoot_frames_left : ch->walk_shoot_frames_right;
            break;
        case CHAR_STATE_JUMP:
            frames_arr = is_left ? ch->jump_shoot_frames_arr_left : ch->jump_shoot_frames_arr_right;
            frames_count = is_left ? ch->jump_shoot_frames_left : ch->jump_shoot_frames_right;
            break;
        case CHAR_STATE_CROUCH:
            frames_arr = is_left ? ch->crouch_shoot_frames_arr_left : ch->crouch_shoot_frames_arr_right;
            frames_count = is_left ? ch->crouch_shoot_frames_left : ch->crouch_shoot_frames_right;
            break;
        case CHAR_STATE_IDLE:
        default:
            frames_arr = is_left ? ch->idle_shoot_frames_arr_left : ch->idle_shoot_frames_arr_right;
            frames_count = is_left ? ch->idle_shoot_frames_left : ch->idle_shoot_frames_right;
            break;
        }
    }
    else
    {
        switch (ch->state)
        {
        case CHAR_STATE_WALK:
            frames_arr = is_left ? ch->walk_frames_arr_left : ch->walk_frames_arr_right;
            frames_count = is_left ? ch->walk_frames_left : ch->walk_frames_right;
            break;
        case CHAR_STATE_JUMP:
            frames_arr = is_left ? ch->jump_frames_arr_left : ch->jump_frames_arr_right;
            frames_count = is_left ? ch->jump_frames_left : ch->jump_frames_right;
            break;
        case CHAR_STATE_CROUCH:
            frames_arr = is_left ? ch->crouch_frames_arr_left : ch->crouch_frames_arr_right;
            frames_count = is_left ? ch->crouch_frames_left : ch->crouch_frames_right;
            break;
        case CHAR_STATE_IDLE:
        default:
            frames_arr = is_left ? ch->idle_frames_arr_left : ch->idle_frames_arr_right;
            frames_count = is_left ? ch->idle_frames_left : ch->idle_frames_right;
            break;
        }
    }

    int safe_frame = ch->frame;
    if (frames_count > 0 && safe_frame >= frames_count)
        safe_frame = 0;
    if (frames_arr && frames_arr[safe_frame] && al_get_bitmap_width(frames_arr[safe_frame]) > 0)
    {
        sprite = frames_arr[safe_frame];
        al_draw_bitmap(sprite, ch->x, ch->y - ch->height, 0);
    }
    else if (default_sprite)
    {
        al_draw_bitmap(default_sprite, ch->x, ch->y - ch->height, 0);
    }

    drawBullets(ch->gun->shots);
    al_draw_rectangle(
        ch->hitbox_x, ch->hitbox_y,
        ch->hitbox_x + ch->hitbox_w,
        ch->hitbox_y + ch->hitbox_h,
        al_map_rgb(255, 0, 0), 2);
}

// Atualize o estado e a flag shooting
void updateCharacterState(Character *ch)
{
    static int anim_counter = 0;
    const int anim_speed = 6;

    if (ch->crouching)
        ch->state = CHAR_STATE_CROUCH;
    else if (ch->jumping || ch->y < Y_SCREEN - 150)
        ch->state = CHAR_STATE_JUMP;
    else if (ch->control->left || ch->control->right)
        ch->state = CHAR_STATE_WALK;
    else
        ch->state = CHAR_STATE_IDLE;

    ch->shooting = ch->control->fire;

    int is_left = (ch->side == 1);
    int frames = 1;
    if (ch->shooting)
    {
        switch (ch->state)
        {
        case CHAR_STATE_WALK:
            frames = is_left ? ch->walk_shoot_frames_left : ch->walk_shoot_frames_right;
            break;
        case CHAR_STATE_JUMP:
            frames = is_left ? ch->jump_shoot_frames_left : ch->jump_shoot_frames_right;
            break;
        case CHAR_STATE_CROUCH:
            frames = is_left ? ch->crouch_shoot_frames_left : ch->crouch_shoot_frames_right;
            break;
        case CHAR_STATE_IDLE:
            frames = is_left ? ch->idle_shoot_frames_left : ch->idle_shoot_frames_right;
            break;
        default:
            frames = 1;
            break;
        }
    }
    else
    {
        switch (ch->state)
        {
        case CHAR_STATE_WALK:
            frames = is_left ? ch->walk_frames_left : ch->walk_frames_right;
            break;
        case CHAR_STATE_JUMP:
            frames = is_left ? ch->jump_frames_left : ch->jump_frames_right;
            break;
        case CHAR_STATE_CROUCH:
            frames = is_left ? ch->crouch_frames_left : ch->crouch_frames_right;
            break;
        case CHAR_STATE_IDLE:
            frames = is_left ? ch->idle_frames_left : ch->idle_frames_right;
            break;
        default:
            frames = 1;
            break;
        }
    }

    if (ch->state == CHAR_STATE_JUMP)
    {
        ch->frame = 0;
        anim_counter = 0;
    }
    else if (frames > 1)
    {
        anim_counter++;
        if (anim_counter >= anim_speed)
        {
            ch->frame = (ch->frame + 1) % frames;
            anim_counter = 0;
        }
    }
    else
    {
        ch->frame = 0;
        anim_counter = 0;
    }
}

void positionUpdate(Character *player, int ground_y, int ground_height)
{
    if (!player)
        return;

    const int normal_height = 48;
    const int crouch_height = 28;
    const int jump_height = 120;
    const int jump_speed = 10;
    const int gravity = 9;

    int target_ground_y = ground_y;

    if (player->jumping)
    {
        player->y -= jump_speed;
        if (player->y <= ground_y - jump_height)
        {
            player->y = ground_y - jump_height;
            player->jumping = 0;
        }
    }
    else if (player->y < target_ground_y)
    {
        player->y += gravity;
        if (player->y > target_ground_y)
            player->y = target_ground_y;
    }

    // Ajuste de altura ao agachar/levantar (mantendo os pés no chão)
    if (player->crouching && player->height != crouch_height)
    {
        player->height = crouch_height;
    }
    else if (!player->crouching && player->height != normal_height)
    {
        player->height = normal_height;
    }

    // Atualize a hitbox após atualizar posição/altura
    player->hitbox_x = player->x;
    player->hitbox_y = player->y - player->height;
    player->hitbox_w = player->width;
    player->hitbox_h = player->height;
}

void bulletUpdate(Character *player)
{
    if (!player || !player->gun)
        return;

    bullet *prev = NULL;
    bullet *curr = player->gun->shots;

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
        case 3: // Para baixo (agachado)
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
                player->gun->shots = curr->next;
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

void updateCharacterHp(Character *player, int delta_hp)
{
    if (!player)
        return;

    player->hp += delta_hp;
    if (player->hp > PLAYER_INITIAL_HP)
        player->hp = PLAYER_INITIAL_HP;
    if (player->hp < 0)
        player->hp = 0;

    printf("HP do jogador atualizado para: %d\n", player->hp);
}

void draw_life_bar(Character *player)
{
    if (!player)
        return;

    int bar_x = 20;
    int bar_y = 20;
    int bar_width = 100;
    int bar_height = 10;

    float percent = (float)player->hp / PLAYER_INITIAL_HP;
    if (percent < 0)
        percent = 0;
    int filled = (int)(bar_width * percent);

    al_draw_filled_rectangle(bar_x, bar_y, bar_x + filled, bar_y + bar_height, al_map_rgb(0, 200, 0));
    al_draw_filled_rectangle(bar_x + filled, bar_y, bar_x + bar_width, bar_y + bar_height, al_map_rgb(100, 0, 0));
    al_draw_rectangle(bar_x, bar_y, bar_x + bar_width, bar_y + bar_height, al_map_rgb(255, 255, 255), 2);
}

// Retorna 1 se houver colisão, 0 caso contrário
int checkCharacterCollision(Character *a, Character *b)
{
    if (!a || !b)
        return 0;
    return (a->hitbox_x < b->hitbox_x + b->hitbox_w &&
            a->hitbox_x + a->hitbox_w > b->hitbox_x &&
            a->hitbox_y < b->hitbox_y + b->hitbox_h &&
            a->hitbox_y + a->hitbox_h > b->hitbox_y);
}
