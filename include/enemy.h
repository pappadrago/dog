#pragma once

#include "bn_optional.h"
#include "bn_sprite_ptr.h"
#include "bn_sprite_animate_actions.h"
#include "bn_fixed.h"
#include "game_constants.h"
#include "obj.h"

class ball;
class bau;
class dog;

class enemy : public live_obj
{
public:
    
    bn::optional<bn::sprite_ptr>               weaponSprite;

    bn::fixed wx_base, wy_base;
    bn::fixed weapon_time = bn::fixed(120.0);
    int weaponDir ;
    int weaponTicks;

    enemy();

    void do_spawn();
    virtual void update();
    virtual ~enemy() = default;  // distruttore virtual obbligatorio
    void do_powerup();
    void bounce(int _dir);
};

