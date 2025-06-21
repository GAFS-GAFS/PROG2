#include "boss.h"

// --- Robust load_frames for boss sprites ---
static void load_frames_debug(ALLEGRO_BITMAP ***arr, const char **src, int count, const char *type)
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
        if (!(*arr)[i])
        {
            fprintf(stderr, "[BOSS SPRITE] Falha ao carregar %s frame %d: %s\n", type, i, src[i]);
        }
    }
}

static void destroy_frames(ALLEGRO_BITMAP **arr, int count)
{
    if (!arr)
        return;
    for (int i = 0; i < count; ++i)
    {
        if (arr[i])
            al_destroy_bitmap(arr[i]);
    }
    free(arr);
}

Boss *createBoss(int x, int y)
{
    Boss *boss = (Boss *)malloc(sizeof(Boss));
    if (!boss)
    {
        fprintf(stderr, "Failed to allocate memory for Boss\n");
        return NULL;
    }
    boss->x = x;
    boss->y = y;
    boss->width = BOSS_WIDTH;
    boss->height = BOSS_HEIGHT;
    boss->hp = BOSS_HP;
    boss->hitbox_x = x;
    boss->hitbox_y = y - BOSS_HEIGHT;
    boss->hitbox_w = BOSS_WIDTH;
    boss->hitbox_h = BOSS_HEIGHT;
    boss->fire_cooldown = 0;
    boss->direction = 1;
    boss->state = 0;
    boss->frame = 0;
    boss->walk_frames_arr = NULL;
    boss->idle_frames_arr = NULL;
    boss->shoot_frames_arr = NULL;
    boss->walk_frames = 0;
    boss->idle_frames = 0;
    boss->shoot_frames = 0;
    boss->gun = createPistol();
    // boss->armor removido, não existe mais armadura
    boss->special_cooldown = 0;
    return boss;
}

void destroyBoss(Boss *boss)
{
    if (!boss)
        return;
    if (boss->gun)
    {
        destroyPistol(boss->gun);
        boss->gun = NULL;
    }
    // Do NOT call destroyBossSprites here anymore!
    free(boss);
}

void loadBossSprites(Boss *boss,
                     const char **walk_frames, int walk_frames_count,
                     const char **idle_frames, int idle_frames_count,
                     const char **shoot_frames, int shoot_frames_count)
{
    if (!boss)
        return;
    boss->walk_frames_arr = NULL;
    boss->idle_frames_arr = NULL;
    boss->shoot_frames_arr = NULL;
    boss->walk_frames = 0;
    boss->idle_frames = 0;
    boss->shoot_frames = 0;
    load_frames_debug(&boss->walk_frames_arr, walk_frames, walk_frames_count, "walk");
    load_frames_debug(&boss->idle_frames_arr, idle_frames, idle_frames_count, "idle");
    load_frames_debug(&boss->shoot_frames_arr, shoot_frames, shoot_frames_count, "shoot");
    boss->walk_frames = walk_frames_count;
    boss->idle_frames = idle_frames_count;
    boss->shoot_frames = shoot_frames_count;
}

void destroyBossSprites(Boss *boss)
{
    if (!boss)
        return;
    if (boss->walk_frames_arr)
    {
        destroy_frames(boss->walk_frames_arr, boss->walk_frames);
        boss->walk_frames_arr = NULL;
        boss->walk_frames = 0;
    }
    if (boss->idle_frames_arr)
    {
        destroy_frames(boss->idle_frames_arr, boss->idle_frames);
        boss->idle_frames_arr = NULL;
        boss->idle_frames = 0;
    }
    if (boss->shoot_frames_arr)
    {
        destroy_frames(boss->shoot_frames_arr, boss->shoot_frames);
        boss->shoot_frames_arr = NULL;
        boss->shoot_frames = 0;
    }
}

void bossShoot(Boss *boss, Character *player)
{
    if (!boss || !boss->gun || boss->fire_cooldown > 0 || !player)
        return;
    int dx = (player->x + player->width / 2) - (boss->x + boss->width / 2);
    int dy = (player->y - player->height / 2) - (boss->y - boss->height / 2);
    int trajectory;
    if (abs(dx) > abs(dy))
        trajectory = (dx > 0) ? 0 : 1;
    else
        trajectory = (dy < 0) ? 2 : 3;

    // Ajuste para o tiro sair do final da sprite (cano), 4 pixels para cima
    int bullet_x, bullet_y;
    if (trajectory == 0)
    { // Direita
        bullet_x = boss->x + boss->width - 8;
        bullet_y = boss->y - boss->height / 2 - 4;
    }
    else if (trajectory == 1)
    { // Esquerda
        bullet_x = boss->x + 8;
        bullet_y = boss->y - boss->height / 2 - 4;
    }
    else if (trajectory == 2)
    { // Cima
        bullet_x = boss->x + boss->width / 2;
        bullet_y = boss->y - boss->height + 8 - 4;
    }
    else
    { // Baixo
        bullet_x = boss->x + boss->width / 2;
        bullet_y = boss->y - 8 - 4;
    }

    bullet *new_bullet = firePistol(bullet_x, bullet_y, trajectory, boss->gun);
    if (new_bullet)
    {
        new_bullet->next = boss->gun->shots;
        boss->gun->shots = new_bullet;
        boss->fire_cooldown = 20; // Boss fires less frequently
    }
}

void updateBoss(Boss *boss, Character *player, int ground_y)
{
    if (!boss || !player)
        return;
    if (boss->fire_cooldown > 0)
        boss->fire_cooldown--;

    // Parâmetros de movimentação
    int safe_distance = 180; // distância mínima do player
    int boss_speed = BOSS_STEP;
    int boss_center = boss->x + boss->width / 2;
    int player_center = player->x + player->width / 2;
    int dx = player_center - boss_center;

    // Movimentação: anda para o player se estiver longe, para se afastar se estiver muito perto
    if (abs(dx) > safe_distance)
    {
        if (dx > 0)
        {
            // Player está à direita, anda para a direita
            boss->x += boss_speed;
            boss->direction = 0; // 0 = direita
        }
        else
        {
            // Player está à esquerda, anda para a esquerda
            boss->x -= boss_speed;
            boss->direction = 1; // 1 = esquerda
        }
        boss->state = 1; // walk
    }
    else if (abs(dx) < safe_distance - 40) // se chegar muito perto, afasta
    {
        if (dx > 0)
        {
            boss->x -= boss_speed;
            boss->direction = 1;
        }
        else
        {
            boss->x += boss_speed;
            boss->direction = 0;
        }
        boss->state = 1; // walk
    }
    else
    {
        boss->state = 0; // idle
    }

    // Limita o boss à tela
    if (boss->x < 0)
        boss->x = 0;
    if (boss->x + boss->width > X_SCREEN)
        boss->x = X_SCREEN - boss->width;

    // Atira sempre que possível na direção do player
    boss->state = 2; // shoot
    bossShoot(boss, player);

    // Atualiza hitbox
    boss->hitbox_x = boss->x;
    boss->hitbox_y = boss->y - boss->height;
    boss->hitbox_w = boss->width;
    boss->hitbox_h = boss->height;

    // Atualiza balas do boss
    if (boss->gun)
    {
        bullet *prev = NULL;
        bullet *curr = boss->gun->shots;
        while (curr)
        {
            bullet *next = curr->next;
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
                if (prev)
                    prev->next = next;
                else
                    boss->gun->shots = next;
                free(curr);
                curr = next;
                continue;
            }
            prev = curr;
            curr = next;
        }
    }
}

static ALLEGRO_BITMAP *boss_bullet_sprite = NULL;

// Função para carregar a sprite da bala do boss
void load_boss_bullet_sprite(const char *path)
{
    if (boss_bullet_sprite)
        al_destroy_bitmap(boss_bullet_sprite);
    boss_bullet_sprite = al_load_bitmap(path);
    if (!boss_bullet_sprite)
        fprintf(stderr, "Erro ao carregar bossBullet.png\n");
}

// Função para liberar a sprite da bala do boss
void destroy_boss_bullet_sprite()
{
    if (boss_bullet_sprite)
    {
        al_destroy_bitmap(boss_bullet_sprite);
        boss_bullet_sprite = NULL;
    }
}

void drawBoss(Boss *boss)
{
    if (!boss)
        return;
    ALLEGRO_BITMAP *sprite = NULL;
    int frame = boss->frame;
    int frames = 1;

    // Proteja o acesso aos arrays de frames
    switch (boss->state)
    {
    case 1: // walk
        if (boss->walk_frames_arr && boss->walk_frames > 0)
        {
            if (boss->walk_frames_arr[frame % boss->walk_frames])
                sprite = boss->walk_frames_arr[frame % boss->walk_frames];
            frames = boss->walk_frames;
        }
        break;
    case 2: // shoot
    case 3: // special
        if (boss->shoot_frames_arr && boss->shoot_frames > 0)
        {
            if (boss->shoot_frames_arr[frame % boss->shoot_frames])
                sprite = boss->shoot_frames_arr[frame % boss->shoot_frames];
            frames = boss->shoot_frames;
        }
        break;
    case 0:
    default:
        if (boss->idle_frames_arr && boss->idle_frames > 0)
        {
            if (boss->idle_frames_arr[frame % boss->idle_frames])
                sprite = boss->idle_frames_arr[frame % boss->idle_frames];
            frames = boss->idle_frames;
        }
        break;
    }
    static int anim_counter = 0;
    const int anim_speed = 8;
    if (frames > 1)
    {
        anim_counter++;
        if (anim_counter >= anim_speed)
        {
            boss->frame = (boss->frame + 1) % frames;
            anim_counter = 0;
        }
    }
    else
    {
        boss->frame = 0;
    }
    if (sprite)
        al_draw_bitmap(sprite, boss->x, boss->y - boss->height, 0);
    else
        al_draw_filled_rectangle(boss->x, boss->y - boss->height, boss->x + boss->width, boss->y, al_map_rgb(80, 80, 80));
    // Draw hitbox for debug
    al_draw_rectangle(boss->hitbox_x, boss->hitbox_y, boss->hitbox_x + boss->hitbox_w, boss->hitbox_y + boss->hitbox_h, al_map_rgb(255, 0, 255), 2);

    // Draw boss bullets
    if (boss->gun)
    {
        bullet *curr = boss->gun->shots;
        while (curr)
        {
            if (boss_bullet_sprite)
                al_draw_bitmap(boss_bullet_sprite, curr->x - al_get_bitmap_width(boss_bullet_sprite) / 2, curr->y - al_get_bitmap_height(boss_bullet_sprite) / 2, 0);
            else
                al_draw_filled_circle(curr->x, curr->y, 8, al_map_rgb(255, 0, 0));
            curr = curr->next;
        }
    }
}

void drawBossLifeBar(Boss *boss)
{
    if (!boss)
        return;
    int bar_width = 300;
    int bar_height = 18;
    int bar_x = (X_SCREEN - bar_width) / 2;
    int bar_y = Y_SCREEN - bar_height - 20;
    float percent = (float)boss->hp / (float)BOSS_HP;
    if (percent < 0)
        percent = 0;
    int filled = (int)(bar_width * percent);

    al_draw_filled_rectangle(bar_x, bar_y, bar_x + filled, bar_y + bar_height, al_map_rgb(200, 0, 0));
    al_draw_filled_rectangle(bar_x + filled, bar_y, bar_x + bar_width, bar_y + bar_height, al_map_rgb(60, 0, 0));
    al_draw_rectangle(bar_x, bar_y, bar_x + bar_width, bar_y + bar_height, al_map_rgb(255, 255, 255), 3);

    // Opcional: texto com valor de HP
    char txt[64];
    snprintf(txt, sizeof(txt), "BOSS HP: %d", boss->hp);
    ALLEGRO_FONT *font = al_create_builtin_font();
    al_draw_text(font, al_map_rgb(255, 255, 255), X_SCREEN / 2, bar_y - 22, ALLEGRO_ALIGN_CENTER, txt);
    al_destroy_font(font);
}

int checkBossPlayerCollision(Boss *boss, Character *player)
{
    if (!boss || !player)
        return 0;
    return (boss->hitbox_x < player->hitbox_x + player->hitbox_w &&
            boss->hitbox_x + boss->hitbox_w > player->hitbox_x &&
            boss->hitbox_y < player->hitbox_y + player->hitbox_h &&
            boss->hitbox_y + boss->hitbox_h > player->hitbox_y);
}

void checkPlayerBulletHitsBoss(Character *player, Boss *boss)
{
    if (!player || !boss || !player->gun)
        return;
    bullet *prev = NULL;
    bullet *curr = player->gun->shots;
    while (curr)
    {
        // Checa colisão da bala com a hitbox do boss
        if (curr->x >= boss->hitbox_x && curr->x <= boss->hitbox_x + boss->hitbox_w &&
            curr->y >= boss->hitbox_y && curr->y <= boss->hitbox_y + boss->hitbox_h)
        {
            // Dano direto, sem armadura
            boss->hp -= curr->damage;
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

void checkBossBulletHitsPlayer(Boss *boss, Character *player)
{
    if (!boss || !player || !boss->gun)
        return;
    bullet *prev = NULL;
    bullet *curr = boss->gun->shots;
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
                boss->gun->shots = curr->next;
            curr = curr->next;
            free(to_remove);
            continue;
        }
        prev = curr;
        curr = curr->next;
    }
}
