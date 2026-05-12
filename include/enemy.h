#pragma once

#include "bn_optional.h"
#include "bn_sprite_ptr.h"
#include "bn_sprite_animate_actions.h"
#include "bn_fixed.h"
#include "game_constants.h"

class ball;
class bau;

class enemy
{
public:
    bn::optional<bn::sprite_ptr>               player;
    bn::optional<bn::sprite_animate_action<4>> actionStand;
    bn::optional<bn::sprite_animate_action<8>> actionJump;
    bn::optional<bn::sprite_animate_action<8>> actionWalk;

    bn::fixed chr_y  = CHR_FLOOR;
    bn::fixed chr_x  = 40;
    bn::fixed chr_vy = bn::fixed(0);
    bn::fixed chr_vx = 0;

    bool jumping = false, walking = false;
    int  dir = 1;

    bn::fixed max_vx = bn::fixed(1.2);
    bn::fixed life   = MAX_LIFE;

    ball* _ball;
    bau*  _bau;

    bn::fixed target_x;
    int ticks2jump = 0, ticks2hit = 0;
    int status = 0;

    enemy(ball* ball, bau* bau);

    void do_spawn();
    void update();
};
