#pragma once

#include "bn_optional.h"
#include "bn_sprite_ptr.h"
#include "bn_sprite_animate_actions.h"
#include "bn_sprite_item.h"
#include "bn_fixed.h"
#include "bn_vector.h"
#include "game_constants.h"

class ball;
class human;
class bau;
class enemy;

class dog
{
public:
    bn::optional<bn::sprite_ptr>               player;
    bn::optional<bn::sprite_animate_action<5>> actionStand;
    bn::optional<bn::sprite_animate_action<6>> actionWalk;
    bn::optional<bn::sprite_item>              dog_item;

    bn::fixed chr_y  = CHR_FLOOR;
    bn::fixed chr_x  = 100;
    bn::fixed chr_vy = bn::fixed(0);
    bn::fixed chr_vx = 0;

    bool walking = false, jumping = false;
    int  dir = 1;
    bn::fixed max_vx = bn::fixed(1.0);

    ball*  ball_ptr;
    human* human_ptr;
    bau*   bau_ptr;
    bn::vector<enemy, MAX_ENEMIES>& enemies;

    int invincibile      = 0;
    int palline_riportate = 0;
    int schema            = 1;

    dog(int skin_index, ball* _ball, human* _human, bau* _bau,
        bn::vector<enemy, MAX_ENEMIES>& _enemies);

    void update();
};
