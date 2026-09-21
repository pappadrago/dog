#pragma once

#include "bn_optional.h"
#include "bn_sprite_ptr.h"
#include "bn_sprite_animate_actions.h"
#include "bn_sprite_item.h"
#include "bn_fixed.h"
#include "bn_vector.h"
#include "game_constants.h"


static constexpr bn::fixed AIR_ACCEL = bn::fixed(0.06);
static constexpr bn::fixed AIR_FRICTION = bn::fixed(0.02);
static constexpr bn::fixed GROUND_ACCEL = bn::fixed(0.15);
static constexpr bn::fixed GROUND_FRICTION = bn::fixed(0.08);

class obj
{
public:
    bn::optional<bn::sprite_ptr>               sprite;

    bn::optional<bn::sprite_animate_action<8>> actionStand;
    bn::optional<bn::sprite_animate_action<8>> actionWalk;
    bn::optional<bn::sprite_item>              spriteItems;

    bn::fixed chr_y  = 512-200;
    bn::fixed chr_x  = 100;
    bn::fixed chr_vy = bn::fixed(0);
    bn::fixed chr_accx = bn::fixed(0);
    bn::fixed chr_vx = bn::fixed(0);

    bn::fixed box_dim = bn::fixed(16);
    bn::fixed box_halfdim = bn::fixed(8);
};

class live_obj: public obj
{
public:

    bool onGround = false;
    int  dir = 1;
    int ticks2action = 0, invulnerability = 0;
    bn::fixed life = MAX_LIFE;
    bn::fixed max_vx = bn::fixed(2.0);
    void apply_gravity();
    void apply_friction();
    void apply_map();
};


// true se il punto (x, y) in coordinate mondo cade in un tile solido
// (fuori mappa conta come solido)
bool is_solid_at(bn::fixed x, bn::fixed y);