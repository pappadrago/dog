#pragma once

#include "bn_optional.h"
#include "bn_sprite_ptr.h"
#include "bn_sprite_animate_actions.h"
#include "bn_fixed.h"
#include "game_constants.h"
#include "obj.h"

class bau;
class dog;

#define ACTION_MOVING 1
#define ACTION_ATTACK 2

class item : public live_obj
{
public:
    
    int action = 0;

    item();

    void do_spawn();
    virtual void update();
    virtual ~item() = default;  // distruttore virtual obbligatorio

    void bounce(int _dir);
};

