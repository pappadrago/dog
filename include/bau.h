#pragma once

#include "bn_optional.h"
#include "bn_sprite_ptr.h"
#include "bn_fixed.h"
#include "game_constants.h"

class bau
{
public:
    bn::optional<bn::sprite_ptr> sprite;
    bn::fixed chr_x;
    bn::fixed chr_y;

    int value = -1;
    int ticks = 0;

    bau();

    void do_spawn();
    void update();
};
