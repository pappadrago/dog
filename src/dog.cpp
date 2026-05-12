#include "dog.h"
#include "ball.h"
#include "human.h"
#include "bau.h"
#include "enemy.h"
#include "collision.h"
#include "bn_keypad.h"
#include "bn_math.h"
#include "bn_random.h"
#include "bn_sound_items.h"
#include "bn_sprite_items_dog.h"
#include "bn_sprite_items_dog2.h"
#include "bn_sprite_items_dog3.h"
#include "bn_sprite_items_dog4.h"
#include "bn_sprite_items_dog5.h"
#include "bn_sprite_items_bau.h"

extern bn::random random1Instance;

dog::dog(int n, ball* _ball, human* _human, bau* _bau,
         bn::vector<enemy, MAX_ENEMIES>& _enemies)
    : ball_ptr(_ball), human_ptr(_human), bau_ptr(_bau), enemies(_enemies)
{
    switch (n)
    {
        case 0: dog_item = bn::sprite_items::dog;  break;
        case 1: dog_item = bn::sprite_items::dog2; break;
        case 2: dog_item = bn::sprite_items::dog3; break;
        case 3: dog_item = bn::sprite_items::dog4; break;
        case 4: dog_item = bn::sprite_items::dog5; break;
        default: break;
    }
    player = dog_item->create_sprite(chr_x - HALF_SCREEN_W, chr_y - CHR_FLOOR + CHR_FLOOR_HEIGHT, 0);
    player->set_bg_priority(2);
    actionStand = bn::create_sprite_animate_action_forever(
        *player, 5, dog_item->tiles_item(), 0, 1, 2, 3, 4);
    actionWalk = bn::create_sprite_animate_action_forever(
        *player, 3, dog_item->tiles_item(), 6, 7, 8, 9, 10, 11);
}

void dog::update()
{
    // --- Input ---
    if (bn::keypad::right_held())      { walking = true; if (!jumping) dir = DIR_RIGHT; }
    else if (bn::keypad::left_held())  { walking = true; if (!jumping) dir = DIR_LEFT;  }
    else                               { walking = false; }

    if (bn::keypad::b_pressed() && !jumping)
    {
        jumping = true;
        chr_vy = bn::fixed(-4.0);
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

    if (invincibile > 0) invincibile--;

    // --- Bau vs nemici ---
    if (bau_ptr->ticks > 0)
    {
        for (enemy& e : enemies)
        {
            if (e.ticks2hit > 0) continue;
            bool hit = check_collision_16(*e.player, *bau_ptr->sprite);
            if (hit)
            {
                e.jumping = true;
                e.chr_vy  = bn::fixed(-1.0);
                e.dir     = e.chr_x < bau_ptr->chr_x ? DIR_LEFT : DIR_RIGHT;
                e.chr_vx  = bn::fixed(1.0).multiplication(e.dir);
                e.ticks2hit = 80;
                e.ticks2jump = 0;
                e.target_x = e.chr_x + bn::fixed(80).multiplication(e.dir);
            }
        }
    }

    // --- Dog vs nemici ---
    if (!jumping && !invincibile)
    {
        for (enemy& e : enemies)
        {
            if (e.ticks2hit > 0) continue;
            e.dir = e.chr_x > chr_x ? DIR_LEFT : DIR_RIGHT;
            bool hit = check_collision_16(*e.player, *this);
            if (hit)
            {
                chr_vx = bn::fixed(1.0);
                dir    = -dir;
                chr_vy = bn::fixed(-1.0);
                jumping = true; walking = true;
                invincibile = 60;
                e.jumping = true;
                e.chr_vy  = bn::fixed(-1.5);
                e.chr_vx  = bn::fixed(1.5).multiplication(e.dir);
                chr_x += chr_vx;
                e.chr_x += e.chr_vx;
                e.target_x = e.chr_x + e.dir * bn::fixed(80);
                if (ball_ptr->pickedup)
                {
                    ball_ptr->pickedup = false;
                    ball_ptr->chr_vx   = dir * bn::fixed(-2.0);
                    ball_ptr->chr_vy   = JUMP_VY;
                    ball_ptr->rolling  = true;
                    if (ball_ptr->valore > 0) ball_ptr->valore -= 1;
                }
            }
        }
    }

    // --- Fisica orizzontale ---
    if (jumping)
    {
        static constexpr bn::fixed AIR_ACCEL    = bn::fixed(0.06);
        static constexpr bn::fixed AIR_FRICTION = bn::fixed(0.02);
        static constexpr bn::fixed AIR_MAX_VX   = bn::fixed(2.0);
        if (bn::keypad::right_held())      { dir = DIR_RIGHT; chr_vx += AIR_ACCEL; if (chr_vx > AIR_MAX_VX) chr_vx = AIR_MAX_VX; }
        else if (bn::keypad::left_held())  { dir = DIR_LEFT;  chr_vx += AIR_ACCEL; if (chr_vx > AIR_MAX_VX) chr_vx = AIR_MAX_VX; }
        else { chr_vx -= AIR_FRICTION; if (chr_vx < bn::fixed(0)) chr_vx = bn::fixed(0); }
    }
    else
    {
        static constexpr bn::fixed GROUND_ACCEL    = bn::fixed(0.15);
        static constexpr bn::fixed GROUND_FRICTION = bn::fixed(0.08);
        if (bn::keypad::right_held())      { dir = DIR_RIGHT; walking = true; chr_vx += GROUND_ACCEL; if (chr_vx > max_vx) chr_vx = max_vx; }
        else if (bn::keypad::left_held())  { dir = DIR_LEFT;  walking = true; chr_vx += GROUND_ACCEL; if (chr_vx > max_vx) chr_vx = max_vx; }
        else { walking = false; chr_vx -= GROUND_FRICTION; if (chr_vx < bn::fixed(0)) chr_vx = bn::fixed(0); }
    }

    chr_x += bn::fixed(dir).multiplication(chr_vx);
    if (chr_x < 0)    chr_x = 0;
    if (chr_x > 1791) chr_x = 1791;

    // --- Fisica verticale ---
    if (jumping)
    {
        chr_vy += GRAVITY;
        if (chr_vy > MAX_FALL) chr_vy = MAX_FALL;
        chr_y += chr_vy;
        if (chr_y > CHR_FLOOR) { chr_y = CHR_FLOOR; jumping = false; chr_vy = 0; }
    }

    // --- Sprite ---
    player->set_x(chr_x - HALF_SCREEN_W);
    player->set_y(chr_y - CHR_FLOOR + CHR_FLOOR_HEIGHT);
    player->set_horizontal_flip(dir == DIR_LEFT);

    if (!walking) actionWalk->reset();
    if (jumping)                   player->set_tiles(dog_item->tiles_item(), 9);
    else if (walking)              actionWalk->update();
    else                           actionStand->update();

    // --- Raccoglie palla ---
    if (!ball_ptr->pickedup && !ball_ptr->rolling && !ball_ptr->inHand)
    {
        if (check_collision_ball_dog(*ball_ptr->player, *this))
        {
            bn::sound_items::coin.play(bn::fixed(0.5));
            ball_ptr->pickedup = true;
        }
    }

    // --- Consegna palla a human ---
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
                human_ptr->do_greet_and_launch();
        }
    }

    if (invincibile)
        player->set_visible(invincibile % 2);
}
