#include "enemy.h"
#include "ball.h"
#include "bau.h"
#include "bn_random.h"
#include "bn_sprite_items_cat.h"
#include "bn_math.h"

extern bn::random random1Instance;

enemy::enemy(ball* _ball_ptr, bau* _bau_ptr, dog* _dog_ptr) : _ball(_ball_ptr), _bau(_bau_ptr), _dog(_dog_ptr)
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

void enemy::do_spawn()
{
    target_x = 60 + 16 + random1Instance.get_int(12) * 80;
    walking = true;
}

void enemy::update()
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
            if (ticks2jump == 0 && !jumping)
                ticks2jump = 120 + random1Instance.get_int(120);
        }

        if (chr_vx > max_vx) chr_vx = max_vx;
        if (chr_vx < -max_vx) chr_vx = -max_vx;
    }

    chr_x += chr_vx;

    if (ticks2hit > 0) ticks2hit--;

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
        if (chr_vy > MAX_FALL) chr_vy = MAX_FALL;
        chr_y += chr_vy;

        if (chr_y >= CHR_FLOOR)
        {
            chr_y = CHR_FLOOR;
            chr_vy = 0;
            jumping = false;
            actionJump->reset();
            if (ticks2hit == 0)
                do_spawn();
        }
    }

    player->set_x(chr_x - HALF_SCREEN_W);
    player->set_y(chr_y - CHR_FLOOR + CHR_FLOOR_HEIGHT + 8);
    player->set_horizontal_flip(dir == DIR_LEFT);

    if (jumping)
    {
        if (!actionJump->done())
            actionJump->update();
    }
    else if (walking)
        actionWalk->update();
    else
        actionStand->update();

    if (ticks2hit > 0)
        player->set_visible(ticks2hit % 2);
    else
        player->set_visible(true);
}
