#include "bn_core.h"
#include "bn_audio.h"
#include "bn_keypad.h"
#include "bn_string.h"
#include "bn_format.h"
#include "bn_bg_palettes.h"
#include "bn_music_actions.h"
#include "bn_sound_actions.h"
#include "bn_jingle_actions.h"
#include "bn_sprite_actions.h"
#include "bn_sprite_text_generator.h"
#include <bn_regular_bg_ptr.h>
#include "bn_regular_bg_map_cell_info.h"
#include "bn_regular_bg_map_ptr.h"
#include "bn_rumble.h"
#include "bn_music_items.h"
#include "bn_sound_items.h"
#include "bn_random.h"
#include "bn_blending_actions.h"
#include "bn_blending_fade_alpha.h"
#include "bn_sprite_text_generator.h"
#include "bn_blending_fade_alpha_hbe_ptr.h"
#include "bn_blending_transparency_attributes.h"
#include "bn_blending_transparency_attributes_hbe_ptr.h"
#include "bn_regular_bg_items_bg_country.h"
#include "bn_regular_bg_items_country.h"
#include "bn_regular_bg_items_dayplatf.h"
#include "common_info.h"
#include "common_variable_8x16_sprite_font.h"
#include <bn_sprite_ptr.h>
#include "bn_sprite_items_food.h"
#include "bn_sprite_items_dog.h"
#include "bn_sprite_items_dog2.h"
#include "bn_sprite_items_dog3.h"
#include "bn_sprite_items_dog4.h"
#include "bn_sprite_items_dog5.h"
#include "bn_sprite_items_bau.h"
#include "bn_sprite_items_fox.h"
#include "bn_sprite_items_cat.h"
#include "bn_sprite_items_bars.h"
#include "bn_sprite_items_zombie.h"
#include "bn_sprite_items_ball.h"
#include "bn_sprite_animate_actions.h"
#include "bn_log.h"
#include <bn_fixed_rect.h>

#include "bn_regular_bg_ptr.h"
#include "bn_string_view.h"

#define DIR_RIGHT 1
#define DIR_LEFT -1
#define HALF_SCREEN_W 120
#define HIDE 0
#define APPROACH 1
#define KILL 2
#define WAIT_THEN_APPROACH 3
#define WAIT_THEN_HIDE 4
#define STUNNED 5
#define SEARCH_AMMO 6
#define DEAD 7

bn::random random1Instance;

class dog;
class enemy;
class ball;
class human;
class bau;
class bonus;

/*
int get_tile_at(const bn::regular_bg_map_item &map_item, int tile_x, int tile_y)
{
    const int MAP_WIDTH = map_item.dimensions().width();
    const int MAP_HEIGHT = map_item.dimensions().height();

    // tile_x = ((tile_x % MAP_WIDTH) + MAP_WIDTH) % MAP_WIDTH;
    // tile_y = ((tile_y % MAP_HEIGHT) + MAP_HEIGHT) % MAP_HEIGHT;

    bn::regular_bg_map_cell cell = map_item.cell(tile_x, tile_y);
    bn::regular_bg_map_cell_info cell_info(cell);
    return cell_info.tile_index();
}
*/

bool check_collision(const bn::sprite_ptr& s1, const bn::sprite_ptr& s2)
{

    bn::fixed dx = s1.x() - s2.x();
    if (dx > s1.dimensions().width() ||
        dx < -s1.dimensions().width())
        return false;

    // Creiamo un rettangolo per il primo sprite (es. 12x12 pixel per una hitbox più stretta)
    bn::fixed_rect rect1(s1.x(), s1.y(),
        bn::fixed(0.8).multiplication(s1.dimensions().width()),
        bn::fixed(0.8).multiplication(s1.dimensions().height()));

    // Creiamo un rettangolo per il secondo sprite
    bn::fixed_rect rect2(s2.x(), s2.y(),
        bn::fixed(0.8).multiplication(s2.dimensions().width()),
        bn::fixed(0.8).multiplication(s2.dimensions().height()));

    // Il metodo intersects gestisce internamente tutta la logica AABB
    return rect1.intersects(rect2);
}

// Costanti salto
#define JUMP_VY bn::fixed(-2.7) // impulso verticale iniziale

#define DOUBLE_JUMP_VY bn::fixed(-3.0) // impulso verticale aggiuntivo per il doppio salto
#define GRAVITY bn::fixed(.2)          // gravita' per frame
#define FRICTION bn::fixed(-.2)
#define FRICTION_GROUND bn::fixed(-.1)
#define FRICTION_AIR bn::fixed(-.02)
#define MAX_FALL bn::fixed(5.5) // velocita' caduta massima
#define CHR_FLOOR bn::fixed(224)
#define CHR_FLOOR_HEIGHT bn::fixed(32)
#define MAX_LIFE 100
#define MAX_ENEMY_BULLETS 16
#define MAX_BULLETS 8
#define MAX_ENEMY_BOMBS 4
#define MAX_ENEMIES 12
#define MAX_BONUSES 3
#define FRESH_COOLDOWN_COUNTER 30

class ball;
class dog;
class human;
class enemy;

static constexpr int SHOW_FRAMES = 120; // frame prima della dissolvenza
static constexpr int FADE_FRAMES = 60;  // frame di dissolvenza

static bn::sprite_text_generator* s_generator = nullptr;
static bn::vector<bn::sprite_ptr, 24> s_sprites;
static int s_timer = 0;
static int s_frame = 0; // contatore frame globale

static const bn::string_view testi[] = {
    "PREMI START",
    // dopo il riporto
    "BRAVO!",
    "BRAVO CANE!",
    "ALLA GRANDE!",
    "QUA LA ZAMPA!",
    "OTTIMO!",
    "CHE CAMPIONE!",
    "BRAVISSIMO!",
    "SEI IL MIGLIORE!",
    "INCREDIBILE!",
    "CHE SALTO!",
    "VELOCISSIMO!",
    "INARRESTABILE!",
    "FANTASTICO!",
    "GRAZIE!",
    "BRAVO AMICO!",
    "PERFETTO!",
    "CHE NASO!",
    "SEI IN FORMA!",
    "ANCORA ANCORA!",
    "HAI VINTO!",
    "SUPER CANE!",
    "MITICO!",
    "CHE CLASSE!",
    "PROPRIO BRAVO!", // 24
    // --- prima del lancio ---
    "RIPORTAMELA!",
    "TRE...DUE...UNO...",
    "DAI VELOCE!",
    "E ORA...VAI!",
    "HOP HOP HOP!",
    "PRONTO?",
    "CI SEI?",
    "STAI FERMO!",
    "OCCHIO LA!",
    "GUARDA BENE!",
    "ATTENZIONE!",
    "CONCENTRATI!",
    "TIENITI PRONTO!",
    "VIA!!!",
    "ADESSO!",
    "ORA!!!",
    "PRENDILA!",
    "ECCOLA!",
    "ZOMPACI SU!",
    "NON PERDERLA!",
    "DRITTO LI!",
    "FORZA!!!",
    "SALTA!!!",
    "CORRI!!!",
    "LA VEDI?" // 48
};

static constexpr int NUM_TESTI = sizeof(testi) / sizeof(testi[0]);

void update_text(int status, bn::sprite_text_generator* gen = nullptr)
{
    if (gen)
    {
        s_generator = gen;
        return;
    }
    s_sprites.clear();
    if (!s_generator || status < 0 || status >= NUM_TESTI)
        return;

    s_generator->set_left_alignment();
    s_generator->generate(-100, 10, testi[status], s_sprites);

    s_timer = SHOW_FRAMES + FADE_FRAMES;
}
void update_text_tick()
{
    s_frame++;

    if (s_timer <= 0 || s_sprites.empty())
        return;

    s_timer--;

    if (s_timer <= 0)
    {
        s_sprites.clear();
        return;
    }

    if (s_timer <= FADE_FRAMES)
    {
        int soglia = (s_timer * 4) / FADE_FRAMES; // 4→0
        bool visibile = (s_frame % 4) < soglia;
        for (auto& s : s_sprites)
            s.set_visible(visibile);
    }
}

class ball
{
public:
    bn::optional<bn::sprite_ptr> player;
    bn::optional<bn::sprite_ptr> shadowRed;
    bn::optional<bn::sprite_ptr> shadow;
    bn::fixed chr_y = CHR_FLOOR;
    bn::fixed chr_x = 40;
    bn::fixed chr_vy = bn::fixed(0); // velocita' y del personaggio
    bn::fixed chr_vx = 0;

    bool rolling = false, pickedup = false, inHand = false;
    int dir = 1;
    bn::fixed max_vx = bn::fixed(2.0), angle = bn::fixed(0);

    bn::fixed valore;

    ball()
    {
        player = bn::sprite_items::ball.create_sprite(chr_x - HALF_SCREEN_W, chr_y - CHR_FLOOR + CHR_FLOOR_HEIGHT, 0);
        shadowRed = bn::sprite_items::ball.create_sprite(chr_x - HALF_SCREEN_W, chr_y - CHR_FLOOR + CHR_FLOOR_HEIGHT, 2);
        shadow = bn::sprite_items::ball.create_sprite(chr_x - HALF_SCREEN_W, chr_y - CHR_FLOOR + CHR_FLOOR_HEIGHT, 1);
        player->set_bg_priority(2);
    }

    void update()
    {

        if (rolling)
        {
            chr_vx -= bn::fixed(0.01);
            if (chr_vx < bn::fixed(.0))
                chr_vx = bn::fixed(.0);
        }

        chr_x = chr_x + bn::fixed(dir).multiplication(chr_vx);
        if (chr_x < 0)
            chr_x = 0;
        if (chr_x > 1791)
            chr_x = 1791;

        if (rolling)
        {
            chr_vy += GRAVITY;
            if (chr_vy > MAX_FALL)
                chr_vy = MAX_FALL;

            if (chr_y > CHR_FLOOR)
            {
                chr_vy = chr_vy.multiplication(bn::fixed(-0.7));
                chr_vx = chr_vx.multiplication(bn::fixed(0.9));
                chr_y = CHR_FLOOR;
                if (bn::abs(chr_vy) < bn::fixed(1.0))
                    chr_vy = 0;
            }
            chr_y += chr_vy;
        }

        if (chr_vx == 0 && chr_vy == 0)
            rolling = false;

        // aggiorna posizione del personaggio
        player->set_x(chr_x - HALF_SCREEN_W);
        player->set_y(chr_y - CHR_FLOOR + CHR_FLOOR_HEIGHT + 12);

        shadow->set_x(chr_x - HALF_SCREEN_W - (4 - valore.multiplication(4).division(10)));

        shadow->set_y(48 + 13);
        shadowRed->set_x(chr_x - HALF_SCREEN_W);
        shadowRed->set_y(48 + 13);

        shadow->set_horizontal_scale(bn::fixed(0.01) + valore.division(10));

        player->set_horizontal_flip(dir == DIR_LEFT);

        // update delle animazioni

        if (rolling)
        {
            angle += bn::fixed(chr_vx);
            player->set_rotation_angle_safe(angle);
        }
    }
};

class dog
{
public:
    bn::optional<bn::sprite_ptr> player;
    bn::optional<bn::sprite_animate_action<5>> actionStand;
    bn::optional<bn::sprite_animate_action<6>> actionWalk;
    bn::optional<bn::sprite_item> dog_item;

    bn::fixed chr_y = CHR_FLOOR;
    bn::fixed chr_x = 100;
    bn::fixed chr_vy = bn::fixed(0); // velocita' y del personaggio
    bn::fixed chr_vx = 0;

    bool walking = false, jumping = false;
    int dir = 1;
    bn::fixed max_vx = bn::fixed(1.0);

    ball* ball_ptr;
    human* human_ptr;
    bau* bau_ptr;
    bn::vector<enemy, MAX_ENEMIES>& enemies;
    int invincibile = 0;
    int palline_riportate = 0, schema = 1;

    dog(int n, ball* _ball, human* _human, bau* _bau, bn::vector<enemy, MAX_ENEMIES>& _enemies) : ball_ptr(_ball), human_ptr(_human), bau_ptr(_bau), enemies(_enemies)
    {
        switch (n)
        {
        case 0:
            dog_item = bn::sprite_items::dog;
            break;
        case 1:
            dog_item = bn::sprite_items::dog2;
        break;case 2:
            dog_item = bn::sprite_items::dog3;
        break;case 3:
            dog_item = bn::sprite_items::dog4;
        break;case 4:
            dog_item = bn::sprite_items::dog5;
            break;
        default:
            break;
        }


        player = dog_item->create_sprite(chr_x - HALF_SCREEN_W, chr_y - CHR_FLOOR + CHR_FLOOR_HEIGHT, 0);
        player->set_bg_priority(2);

        actionStand = bn::create_sprite_animate_action_forever(
            *player, 5, dog_item->tiles_item(), 0, 1, 2, 3, 4);
        actionWalk = bn::create_sprite_animate_action_forever(
            *player, 3, dog_item->tiles_item(), 6, 7, 8, 9, 10, 11);

    }

    void update();
};

void update_text(int status, dog* _dog, bn::sprite_text_generator* gen = nullptr)
{
    if (gen)
    {
        s_generator = gen;
        return;
    }
    s_sprites.clear();
    if (!s_generator || status < 0 || status >= NUM_TESTI)
        return;

    s_generator->set_left_alignment();
    bn::string<32> text;

    text.append(bn::format<24>("Schema {}. Premi START.", _dog->schema));

    s_generator->generate(-100, 10, text, s_sprites);

    s_timer = SHOW_FRAMES + FADE_FRAMES;
}
class human
{
public:
    bn::optional<bn::sprite_ptr> player;
    bn::optional<bn::sprite_animate_action<8>> actionStand;
    bn::optional<bn::sprite_animate_action<7>> actionLaunch;
    bn::optional<bn::sprite_ptr> barra_verde;
    bn::optional<bn::sprite_ptr> barra_rossa;

    bn::fixed chr_y = CHR_FLOOR;
    bn::fixed chr_x = 40;
    bn::fixed chr_vy = bn::fixed(0); // velocita' y del personaggio
    bn::fixed chr_vx = 0;

    bool walking = false, firing = false, sitting = false, jumping = false, launching = false;
    int dir = 1;
    bn::fixed max_vx = bn::fixed(1.0);

    int coolDown = 0;
    int max_cool_down = FRESH_COOLDOWN_COUNTER;
    bn::fixed life = MAX_LIFE;
    ball* _ball;
    bn::fixed max_ball_speed = bn::fixed(3.0);

    int ticks = 0;
    human(ball* ball) : _ball(ball)
    {
        player = bn::sprite_items::zombie.create_sprite(chr_x - HALF_SCREEN_W, chr_y - CHR_FLOOR + CHR_FLOOR_HEIGHT, 0);
        player->set_bg_priority(2);

        actionStand = bn::create_sprite_animate_action_forever(
            *player, 6, bn::sprite_items::zombie.tiles_item(), 0, 1, 2, 3, 4, 5, 6, 7);
        actionLaunch = bn::create_sprite_animate_action_once(
            *player, 3, bn::sprite_items::zombie.tiles_item(), 8, 9, 10, 11, 12, 13, 14);

        barra_rossa = bn::sprite_items::bars.create_sprite(80, 100, 1);
        barra_verde = bn::sprite_items::bars.create_sprite(80, 100, 0);
        barra_verde->set_bg_priority(2);
        barra_rossa->set_bg_priority(2);
        barra_rossa->set_z_order(1); // disegnata prima → sta sotto
        barra_verde->set_z_order(0); // disegnata dopo → sta sopra
    }

    void do_greet_and_launch()
    {
        update_text(1 + random1Instance.get_int(24));
        _ball->pickedup = false;
        launching = false;
        _ball->chr_x = chr_x;
        _ball->chr_y = 2000;
        ticks = 120;
    }

    void do_launch()
    {
        _ball->chr_x = chr_x;
        _ball->chr_y = chr_y;
        _ball->chr_vy = JUMP_VY - random1Instance.get_fixed(max_ball_speed);
        _ball->chr_vx = bn::fixed(2.5) + random1Instance.get_fixed(max_ball_speed);
        _ball->rolling = true;
        _ball->inHand = false;
        _ball->valore = 10;
        life -= _ball->valore;

        launching = true;
        update_text(25 + random1Instance.get_int(24));
    }

    void update()
    {
        if (ticks > 0)
        {
            ticks--;
            if (ticks == 0)
                do_launch();
        }
        // aggiorna posizione del personaggio
        player->set_x(chr_x - HALF_SCREEN_W);
        player->set_y(chr_y - CHR_FLOOR + CHR_FLOOR_HEIGHT);

        player->set_horizontal_flip(dir == DIR_LEFT);

        barra_verde->set_x(chr_x - HALF_SCREEN_W - (16 - life.multiplication(16).division(MAX_LIFE)));
        barra_verde->set_y(chr_y - CHR_FLOOR + CHR_FLOOR_HEIGHT - 16);
        barra_rossa->set_x(chr_x - HALF_SCREEN_W);
        barra_rossa->set_y(chr_y - CHR_FLOOR + CHR_FLOOR_HEIGHT - 16);

        barra_verde->set_horizontal_scale(bn::fixed(0.01) + life.division(MAX_LIFE));

        // update delle animazioni
        if (launching)
        {
            if (!actionLaunch->done())
                actionLaunch->update();
            else
            {
                launching = false;
                actionLaunch->reset();
            }
        }
        else
            actionStand->update();
    }
};

#define BONUS_STATUS_HIDE 0
#define BONUS_STATUS_SHOW 1
#define BONUS_STATUS_SHINE 2

class bonus
{
public:
    bn::optional<bn::sprite_ptr> sprite;
    bn::fixed pos_x;
    bn::fixed pos_y = CHR_FLOOR - 40;

    int value = -1;
    int ticks = 0;
    int status = BONUS_STATUS_HIDE;

    bonus()

    {
        value = random1Instance.get_int(13);
        sprite = bn::sprite_items::food.create_sprite(0, 0, value); // una sola create_sprite
        sprite->set_visible(false);
        new_position();

        // Sfasa i bonus nel tempo così non appaiono tutti insieme
        ticks = random1Instance.get_int(200);
        status = BONUS_STATUS_HIDE;
    }

    void new_position()
    {
        value = random1Instance.get_int(13);
        sprite->set_tiles(bn::sprite_items::food.tiles_item(), value);
        sprite->set_rotation_angle_safe(0);
        pos_x = 50 + random1Instance.get_int(1000);
    }

    void update();
};

class enemy
{
public:
    bn::optional<bn::sprite_ptr> player;
    bn::optional<bn::sprite_animate_action<4>> actionStand;
    bn::optional<bn::sprite_animate_action<8>> actionJump;
    bn::optional<bn::sprite_animate_action<8>> actionWalk;

    bn::fixed chr_y = CHR_FLOOR;
    bn::fixed chr_x = 40;
    bn::fixed chr_vy = bn::fixed(0); // velocita' y del personaggio
    bn::fixed chr_vx = 0;

    bool jumping = false, walking = false;
    int dir = 1;

    bn::fixed max_vx = bn::fixed(1.2);

    bn::fixed life = MAX_LIFE;
    ball* _ball;
    bau* _bau;

    bn::fixed target_x;
    int ticks2jump = 0, ticks2hit = 0;

    int status = 0;

    enemy(ball* ball, bau* bau) : _ball(ball), _bau(bau)
    {
        player = bn::sprite_items::cat.create_sprite(chr_x - HALF_SCREEN_W, chr_y - CHR_FLOOR + CHR_FLOOR_HEIGHT, 0);
        player->set_bg_priority(2);

        actionStand = bn::create_sprite_animate_action_forever(
            *player, 6, bn::sprite_items::cat.tiles_item(), 0, 1, 2, 3);
        actionJump = bn::create_sprite_animate_action_once(
            *player, 4, bn::sprite_items::cat.tiles_item(), 12, 13, 14, 15, 16, 17, 18, 19);
        actionWalk = bn::create_sprite_animate_action_forever(
            *player, 3, bn::sprite_items::cat.tiles_item(), 4, 5, 6, 7, 8, 9, 10, 11);
        chr_x = 260;
        do_spawn();
    }

    void do_spawn()
    {
        target_x = 60 + 16 + random1Instance.get_int(12) * 80;
        walking = true;
    }

    void update()
    {

        if (!jumping)
        {
            if (chr_x < target_x - bn::fixed(2.0))
            {
                dir = DIR_RIGHT;
                walking = true;
                chr_vx += bn::fixed(.1);
            }
            else if (chr_x > target_x + bn::fixed(2.0))
            {
                dir = DIR_LEFT;
                walking = true;
                chr_vx += bn::fixed(-.1);
            }
            else if (walking)
            {
                chr_x = target_x;
                walking = false;
                chr_vx = 0;
                if (ticks2jump == 0 && !jumping) // solo se non già in attesa
                    ticks2jump = 120 + random1Instance.get_int(120);
            }
            if (chr_vx > max_vx)
                chr_vx = max_vx;
            if (chr_vx < -max_vx)
                chr_vx = -max_vx;
        }
        chr_x += chr_vx;
        if (ticks2hit > 0)
            ticks2hit--;

        if (ticks2jump > 0)
        {
            ticks2jump--;
            if (ticks2jump == 0)
            {
                jumping = true;
                chr_vy = JUMP_VY;
            }
        }

        if (jumping)
        {
            chr_vy += GRAVITY;
            if (chr_vy > MAX_FALL)
                chr_vy = MAX_FALL;
            chr_y += chr_vy;
            if (chr_y >= CHR_FLOOR)
            {
                chr_y = CHR_FLOOR;
                chr_vy = 0;
                jumping = false;
                actionJump->reset();
                if (ticks2hit == 0) // se non è stato colpito durante il salto
                    do_spawn();
            }
        }

        // aggiorna posizione del personaggio
        player->set_x(chr_x - HALF_SCREEN_W);
        player->set_y(chr_y - CHR_FLOOR + CHR_FLOOR_HEIGHT + 8);

        player->set_horizontal_flip(dir == DIR_LEFT);

        // update delle animazioni
        if (jumping)
        {
            if (!actionJump->done())
                actionJump->update();
        }
        else if (walking)
        {
            actionWalk->update();
        }
        else
            actionStand->update();

        if (ticks2hit > 0)
            player->set_visible(ticks2hit % 2);
        else
            player->set_visible(true);
    }
};

bool check_collision_ball_dog(const bn::sprite_ptr& ball, const dog& _dog)
{
    bn::fixed dog_x = _dog.player->x();
    bn::fixed dog_y = _dog.player->y() + 6;
    bn::fixed dog_hw = 8;
    bn::fixed dog_hh = 13;

    bn::fixed ball_hw = 4;
    bn::fixed ball_hh = 4;

    bool hit = bn::abs(ball.x() - dog_x) < (dog_hw + ball_hw) &&
        bn::abs(ball.y() - dog_y) < (dog_hh + ball_hh);

    return hit;
}

bool check_collision_16(const bn::sprite_ptr& ball, const dog& _dog)
{
    bn::fixed dog_x = _dog.player->x();
    bn::fixed dog_y = _dog.player->y() + 6;
    bn::fixed dog_hw = 8;
    bn::fixed dog_hh = 13;

    bn::fixed ball_hw = 6;
    bn::fixed ball_hh = 6;

    bool hit = bn::abs(ball.x() - dog_x) < (dog_hw + ball_hw) &&
        bn::abs(ball.y() - dog_y) < (dog_hh + ball_hh);

    return hit;
}
bool check_collision_16(const bn::sprite_ptr& ball, const bn::sprite_ptr& _dog)
{
    bn::fixed dog_x = _dog.x();
    bn::fixed dog_y = _dog.y();
    bn::fixed dog_hw = 8;
    bn::fixed dog_hh = 8;

    bn::fixed ball_hw = 8;
    bn::fixed ball_hh = 8;

    bool hit = bn::abs(ball.x() - dog_x) < (dog_hw + ball_hw) &&
        bn::abs(ball.y() - dog_y) < (dog_hh + ball_hh);

    return hit;
}
class bau
{
public:
    bn::optional<bn::sprite_ptr> sprite;
    bn::fixed chr_x;
    bn::fixed chr_y = CHR_FLOOR - 40;

    int value = -1;
    int ticks = 0;
    bn::fixed dim = bn::fixed(.1);

    bau()
    {
        sprite = bn::sprite_items::bau.create_sprite(0, 0, 0); // una sola create_sprite
        sprite->set_visible(false);
    }

    void update()
    {

        if (ticks > 0)
            ticks--;
        if (dim < bn::fixed(1.0))
            dim += bn::fixed(0.1);
        else
            dim = bn::fixed(1.0);
        sprite->set_x(chr_x - HALF_SCREEN_W);
        sprite->set_y(chr_y - CHR_FLOOR + CHR_FLOOR_HEIGHT);
        sprite->set_scale(dim);
        sprite->set_visible(ticks > 60 || ticks % 2);
    }
};

void dog::update()
{

    if (bn::keypad::right_held())
    {
        walking = true;
        if (!jumping)
            dir = DIR_RIGHT;
    }
    else if (bn::keypad::left_held())
    {

        walking = true;
        if (!jumping)
            dir = DIR_LEFT;
    }
    else
    {
        walking = false;
    }

    if (bn::keypad::b_pressed() && !jumping)
    {
        jumping = true;
        chr_vy = bn::fixed(-4.0);
        chr_vx += bn::fixed(1.5);
        walking = false;
    }

    if (bn::keypad::a_pressed() && !jumping && bau_ptr->ticks == 0)
    {
        bau_ptr->chr_x = chr_x + bn::fixed(32).multiplication(dir);
        bau_ptr->chr_y = chr_y;
        bau_ptr->sprite->set_tiles(bn::sprite_items::bau.tiles_item(), dir == DIR_LEFT ? 1 : 0);
        bau_ptr->sprite->set_scale(ball_ptr->pickedup ? bn::fixed(0.75) : bn::fixed(1.0));
        bau_ptr->ticks = 120;
        bau_ptr->dim = bn::fixed(0.1);
    }

    if (invincibile > 0)
        invincibile--;

    if (bau_ptr->ticks > 0)
    {
        for (enemy& enemy : enemies)
        {
            if (enemy.ticks2hit > 0)
                continue;
            bool hit = check_collision_16(*enemy.player, *bau_ptr->sprite);
            if (hit)
            {
                enemy.jumping = true;
                enemy.chr_vy = bn::fixed(-1.0);
                enemy.dir = enemy.chr_x < bau_ptr->chr_x ? DIR_LEFT : DIR_RIGHT;
                enemy.chr_vx = bn::fixed(1.0).multiplication(enemy.dir);
                enemy.ticks2hit = 80;
                enemy.ticks2jump = 0; // interrompe eventuale attesa salto
                enemy.target_x = enemy.chr_x + bn::fixed(80).multiplication(enemy.dir);
            }
        }
    }

    if (!jumping && !invincibile)
        for (enemy& enemy : enemies)
        {
            if (enemy.ticks2hit > 0)
                continue;
            enemy.dir = enemy.chr_x > chr_x ? DIR_LEFT : DIR_RIGHT;
            bool hit = check_collision_16(*enemy.player, *this);
            if (hit)
            {
                chr_vx = bn::fixed(1.0);
                dir = -dir;
                chr_vy = bn::fixed(-1.0);
                jumping = true;
                walking = true;
                invincibile = 60;

                enemy.jumping = true;
                enemy.chr_vy = bn::fixed(-1.5);
                enemy.chr_vx = bn::fixed(1.5).multiplication(enemy.dir);

                chr_x += chr_vx;
                enemy.chr_x += enemy.chr_vx;

                enemy.target_x = enemy.chr_x + enemy.dir * bn::fixed(80);

                if (ball_ptr->pickedup)
                {
                    ball_ptr->pickedup = false;
                    ball_ptr->chr_vx = dir * bn::fixed(-2.0);
                    ball_ptr->chr_vy = JUMP_VY;
                    ball_ptr->rolling = true;
                    if (ball_ptr->valore > 0)
                        ball_ptr->valore -= 1;
                }
            }
        }

    if (walking && !jumping)
    {
        chr_vx += bn::fixed(0.1);

        if (chr_vx > max_vx)
            chr_vx = max_vx;
    }
    else if (!jumping)
    {
        chr_vx -= bn::fixed(0.05);
        if (chr_vx < bn::fixed(.0))
            chr_vx = bn::fixed(.0);
    }
    else if (jumping)
    {
        chr_vx -= bn::fixed(0.025);
        if (chr_vx < bn::fixed(.0))
            chr_vx = bn::fixed(.0);
    }

    chr_x = chr_x + bn::fixed(dir).multiplication(chr_vx);
    if (chr_x < 0)
        chr_x = 0;
    if (chr_x > 1791)
        chr_x = 1791;

    if (jumping)
    {
        chr_vy += GRAVITY;
        if (chr_vy > MAX_FALL)
            chr_vy = MAX_FALL;
        chr_y += chr_vy;

        if (chr_y > CHR_FLOOR)
        {
            chr_y = CHR_FLOOR;
            jumping = false;
            chr_vy = 0;
        }
    }

    // aggiorna posizione del personaggio
    player->set_x(chr_x - HALF_SCREEN_W);
    player->set_y(chr_y - CHR_FLOOR + CHR_FLOOR_HEIGHT);

    player->set_horizontal_flip(dir == DIR_LEFT);

    // update delle animazioni
    if (!walking)
        actionWalk->reset();

    if (jumping)
    {
        player->set_tiles(dog_item->tiles_item(), 9);
    }
    else if (walking)
    {
        actionWalk->update();
    }
    else if (!walking && !jumping)
    {
        actionStand->update();
    }

    if (!ball_ptr->pickedup && !ball_ptr->rolling && !ball_ptr->inHand)
    {
        bool hit = check_collision_ball_dog(*ball_ptr->player, *this);
        if (hit)
        {
            bn::sound_items::coin.play(bn::fixed(0.5));
            ball_ptr->pickedup = true;
        }
    }

    if (ball_ptr->pickedup && !ball_ptr->inHand)
    {
        ball_ptr->chr_x = chr_x + bn::fixed(14.0).multiplication(dir);
        ball_ptr->chr_y = chr_y - 8;

        bool hit = check_collision(*ball_ptr->player, *human_ptr->player);
        if (hit)
        {
            bn::sound_items::coin.play(bn::fixed(0.5));
            palline_riportate++;
            human_ptr->life += ball_ptr->valore;
            ball_ptr->pickedup = false;
            invincibile = 60;
            ball_ptr->inHand = true;
            if (palline_riportate < 3)
            {
                human_ptr->do_greet_and_launch();
            }
        }
    }

    if (invincibile)
        player->set_visible(invincibile % 2);
}

int main()
{

    bn::core::init();

    bn::regular_bg_ptr bg0 = bn::regular_bg_items::bg_country.create_bg(0);
    bn::regular_bg_ptr bg1 = bn::regular_bg_items::bg_country.create_bg(1);
    // bn::regular_bg_ptr bg2 = bn::regular_bg_items::mistbg.create_bg(2);

    bn::regular_bg_ptr bg3 = bn::regular_bg_items::dayplatf.create_bg(0);

    bn::sprite_text_generator text_generator(common::variable_8x16_sprite_font);
    update_text(0, &text_generator); // registrazione

    bn::music_items::qwak.play(0.25);

    // bg1.set_y(-32);
    // bg2.set_y(-32);
    bg3.set_y(-32);

    bg3.set_x(896 - HALF_SCREEN_W);

    bn::camera_ptr camera = bn::camera_ptr::create(0, 0);

    bg3.set_camera(camera);

    bau bau;
    bau.sprite->set_camera(camera);

    ball ball;
    ball.player->set_camera(camera);
    ball.shadow->set_camera(camera);
    ball.shadowRed->set_camera(camera);
    ball.player->set_bg_priority(2);

    human human(&ball);
    human.player->set_camera(camera);
    human.player->set_bg_priority(2);
    human.barra_rossa->set_camera(camera);
    human.barra_verde->set_camera(camera);

    bn::vector<enemy, MAX_ENEMIES> enemies;

    dog dog(1, &ball, &human, &bau, enemies);
    dog.player->set_camera(camera);
    dog.player->set_bg_priority(2);

    const bn::regular_bg_map_item& bg3_map_item = bn::regular_bg_items::country.map_item();

    while (true)
    {

        ball.pickedup = false;
        ball.rolling = false;
        ball.chr_y = 1000;

        // attendi START
        while (true)
        {
            int r = random1Instance.get_int();

            if (bn::keypad::start_released())
            {
                break;
            }
            update_text(0, &dog);

            if (dog.chr_x > 100)
                dog.chr_x = 100;

            dog.update();
            human.update();
            bn::core::update();
        }

        if (dog.schema % 2 == 0)
        {
            enemy w(&ball, &bau);
            w.player->set_camera(camera);
            w.do_spawn();
            enemies.push_back(bn::move(w));
        }
        else if (dog.schema % 2 == 1)
        {
            for (enemy& enemy : enemies)
            {
                enemy.max_vx += bn::fixed(0.2);
            }
        }

        update_text(-1);
        human.ticks = 120;
        while (true)
        {

            dog.update();
            human.update();
            ball.update();
            bau.update();

            for (enemy& enemy : enemies)
            {
                enemy.update();
            }
            int tx = (dog.chr_x).division(8).integer();

            bn::fixed chr_y_m = dog.chr_y - 32;

            int ty = (chr_y_m).division(8).integer();

            bn::regular_bg_map_cell cella = bg3_map_item.cell(bn::point(tx, ty));
            bn::regular_bg_map_cell_info info(cella);

            // update testo
            // update_text(status, text_generator, music_volume_sprites);

            if (dog.chr_x > HALF_SCREEN_W && dog.chr_x < 1792 - HALF_SCREEN_W)
            {

                bn::fixed cam_x = dog.chr_x - HALF_SCREEN_W; // soldier.player->x(); // o qualunque logica tu usi per muovere la camera
                camera.set_x(cam_x);

                // Ogni background si muove a una frazione della velocità della camera
                // bg0.set_x(cam_x * bn::fixed(-0.125)); // lontano, si muove lento
                bg0.set_x(cam_x * bn::fixed(-0.25)); // medio
                bg1.set_x(cam_x * bn::fixed(-0.5));  // più vicino, più veloce
            }
            update_text_tick(); // ogni frame
            bn::core::update();

            if (dog.palline_riportate == 3)
            {
                dog.palline_riportate = 0;
                dog.schema++;
                break;
            }
        }
    }
}