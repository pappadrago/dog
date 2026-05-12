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
    bn::fixed chr_y = CHR_FLOOR - 40;

    int value = -1;
    int ticks = 0;
    bn::fixed dim = bn::fixed(.1);

    bau();
    void update();
};
