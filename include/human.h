#pragma once

#include "bn_optional.h"
#include "bn_sprite_ptr.h"
#include "bn_sprite_animate_actions.h"
#include "bn_fixed.h"
#include "game_constants.h"

class ball;

class human
{
public:
    bn::optional<bn::sprite_ptr>               player;
    bn::optional<bn::sprite_animate_action<8>> actionStand;
    bn::optional<bn::sprite_animate_action<7>> actionLaunch;
    bn::optional<bn::sprite_ptr>               barra_verde;
    bn::optional<bn::sprite_ptr>               barra_rossa;

    bn::fixed chr_y  = CHR_FLOOR;
    bn::fixed chr_x  = 40;
    bn::fixed chr_vy = bn::fixed(0);
    bn::fixed chr_vx = 0;

    bool walking = false, firing = false, sitting = false;
    bool jumping = false, launching = false;
    int  dir = 1;
    bn::fixed max_vx = bn::fixed(1.0);

    int coolDown = 0;
    int max_cool_down = FRESH_COOLDOWN_COUNTER;
    bn::fixed life = MAX_LIFE;

    bn::fixed max_ball_speed = bn::fixed(3.0);
    int ticks = 0;

    ball* _ball;

    explicit human(ball* ball);

    void do_greet_and_launch();
    void do_launch();
    void update();
};
