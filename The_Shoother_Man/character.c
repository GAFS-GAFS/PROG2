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

    if (!newCharacter->control || !newCharacter->gun)
    {
        free(newCharacter);
        return NULL;
    }

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

void loadCharacterSprites(
    Character *ch,
    const char *walk, int walk_frames,
    const char *jump, int jump_frames,
    const char *crouch, int crouch_frames,
    const char *idle, int idle_frames,
    const char *walk_shoot, int walk_shoot_frames,
    const char *jump_shoot, int jump_shoot_frames,
    const char *crouch_shoot, int crouch_shoot_frames,
    const char *idle_shoot, int idle_shoot_frames)
{
    ch->walk_spritesheet = al_load_bitmap(walk);
    ch->jump_spritesheet = al_load_bitmap(jump);
    ch->crouch_spritesheet = al_load_bitmap(crouch);
    ch->idle_spritesheet = al_load_bitmap(idle);
    ch->walk_shoot_spritesheet = al_load_bitmap(walk_shoot);
    ch->jump_shoot_spritesheet = al_load_bitmap(jump_shoot);
    ch->crouch_shoot_spritesheet = al_load_bitmap(crouch_shoot);
    ch->idle_shoot_spritesheet = al_load_bitmap(idle_shoot);

    ch->walk_frames = walk_frames;
    ch->jump_frames = jump_frames;
    ch->crouch_frames = crouch_frames;
    ch->idle_frames = idle_frames;
    ch->walk_shoot_frames = walk_shoot_frames;
    ch->jump_shoot_frames = jump_shoot_frames;
    ch->crouch_shoot_frames = crouch_shoot_frames;
    ch->idle_shoot_frames = idle_shoot_frames;
}

void destroyCharacterSprites(Character *ch)
{
    if (ch->walk_spritesheet)
        al_destroy_bitmap(ch->walk_spritesheet);
    if (ch->jump_spritesheet)
        al_destroy_bitmap(ch->jump_spritesheet);
    if (ch->crouch_spritesheet)
        al_destroy_bitmap(ch->crouch_spritesheet);
    if (ch->idle_spritesheet)
        al_destroy_bitmap(ch->idle_spritesheet);
    if (ch->walk_shoot_spritesheet)
        al_destroy_bitmap(ch->walk_shoot_spritesheet);
    if (ch->jump_shoot_spritesheet)
        al_destroy_bitmap(ch->jump_shoot_spritesheet);
    if (ch->crouch_shoot_spritesheet)
        al_destroy_bitmap(ch->crouch_shoot_spritesheet);
    if (ch->idle_shoot_spritesheet)
        al_destroy_bitmap(ch->idle_shoot_spritesheet);
}

// Função de desenho considerando movimento + tiro
void drawCharacter(Character *ch, ALLEGRO_BITMAP *default_sprite, bool flip)
{
    ALLEGRO_BITMAP *sprite = NULL;
    int frame_w = ch->width, frame_h = ch->height;
    int frame = ch->frame;

    if (ch->shooting)
    {
        switch (ch->state)
        {
        case CHAR_STATE_WALK:
            sprite = ch->walk_shoot_spritesheet;
            break;
        case CHAR_STATE_JUMP:
            sprite = ch->jump_shoot_spritesheet;
            break;
        case CHAR_STATE_CROUCH:
            sprite = ch->crouch_shoot_spritesheet;
            break;
        case CHAR_STATE_IDLE:
        default:
            sprite = ch->idle_shoot_spritesheet;
            break;
        }
    }
    else
    {
        switch (ch->state)
        {
        case CHAR_STATE_WALK:
            sprite = ch->walk_spritesheet;
            break;
        case CHAR_STATE_JUMP:
            sprite = ch->jump_spritesheet;
            break;
        case CHAR_STATE_CROUCH:
            sprite = ch->crouch_spritesheet;
            break;
        case CHAR_STATE_IDLE:
        default:
            sprite = ch->idle_spritesheet;
            break;
        }
    }

    if (sprite)
    {
        // Desenha o personagem com o ponto de origem nos pés
        al_draw_bitmap_region(sprite, frame * frame_w, 0, frame_w, frame_h, ch->x, ch->y - ch->height, flip ? ALLEGRO_FLIP_HORIZONTAL : 0);
    }
    else if (default_sprite)
    {
        al_draw_bitmap(default_sprite, ch->x, ch->y - ch->height, flip ? ALLEGRO_FLIP_HORIZONTAL : 0);
    }

    // Desenhar as balas do personagem
    drawBullets(ch->gun->shots);
}

// Atualize o estado e a flag shooting
void updateCharacterState(Character *ch)
{
    static int anim_counter = 0;
    const int anim_speed = 6; // Aumente para deixar mais lento (ex: 4 = troca a cada 4 frames do jogo)

    if (ch->crouching)
        ch->state = CHAR_STATE_CROUCH;
    else if (ch->jumping)
        ch->state = CHAR_STATE_JUMP;
    else if (ch->control->left || ch->control->right)
        ch->state = CHAR_STATE_WALK;
    else
        ch->state = CHAR_STATE_IDLE;

    ch->shooting = ch->control->fire;

    int frames = 1;
    if (ch->shooting)
    {
        switch (ch->state)
        {
        case CHAR_STATE_WALK:
            frames = ch->walk_shoot_frames;
            break;
        case CHAR_STATE_JUMP:
            frames = ch->jump_shoot_frames;
            break;
        case CHAR_STATE_CROUCH:
            frames = ch->crouch_shoot_frames;
            break;
        case CHAR_STATE_IDLE:
            frames = ch->idle_shoot_frames;
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
            frames = ch->walk_frames;
            break;
        case CHAR_STATE_JUMP:
            frames = ch->jump_frames;
            break;
        case CHAR_STATE_CROUCH:
            frames = ch->crouch_frames;
            break;
        case CHAR_STATE_IDLE:
            frames = ch->idle_frames;
            break;
        default:
            frames = 1;
            break;
        }
    }

    // Controle de velocidade da animação
    if (frames > 1)
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
}

void bulletUpdate(Character *player)
{
    if (!player || !player->gun)
        return;

    bullet *prev = NULL;
    bullet *curr = player->gun->shots;

    while (curr)
    {
        switch (curr->trajectory)
        {
        case 0:
            curr->x += BULLET_MOVE;
            break;
        case 1:
            curr->x -= BULLET_MOVE;
            break;
        case 2:
            curr->y -= BULLET_MOVE;
            break;
        case 3:
            curr->y += BULLET_MOVE;
            break;
        }

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

void draw_life_bar(Character *player)
{
    int bar_x = 20;
    int bar_y = 20;
    int bar_width = 100; // barra menor
    int bar_height = 10; // barra menor

    float percent = (float)player->hp / PLAYER_INITIAL_HP;
    if (percent < 0)
        percent = 0;
    int filled = (int)(bar_width * percent);

    // Barra verde (vida atual)
    al_draw_filled_rectangle(bar_x, bar_y, bar_x + filled, bar_y + bar_height, al_map_rgb(0, 200, 0));
    // Barra vermelha (vida perdida)
    al_draw_filled_rectangle(bar_x + filled, bar_y, bar_x + bar_width, bar_y + bar_height, al_map_rgb(100, 0, 0));
    // Contorno branco
    al_draw_rectangle(bar_x, bar_y, bar_x + bar_width, bar_y + bar_height, al_map_rgb(255, 255, 255), 2);
}
