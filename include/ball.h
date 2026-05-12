#pragma once

#include "bn_optional.h"
#include "bn_sprite_ptr.h"
#include "bn_fixed.h"
#include "game_constants.h"

class ball
{
public:
    bn::optional<bn::sprite_ptr> player;
    bn::optional<bn::sprite_ptr> shadowRed;
    bn::optional<bn::sprite_ptr> shadow;

    bn::fixed chr_y  = CHR_FLOOR;
    bn::fixed chr_x  = 40;
    bn::fixed chr_vy = bn::fixed(0);
    bn::fixed chr_vx = 0;

    bool rolling = false, pickedup = false, inHand = false;
    int  dir = 1;
    bn::fixed max_vx = bn::fixed(2.0);
    bn::fixed angle  = bn::fixed(0);
    bn::fixed valore;

    ball();
    void update();
};
