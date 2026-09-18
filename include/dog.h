#pragma once

#include "bn_optional.h"
#include "bn_sprite_ptr.h"
#include "bn_sprite_animate_actions.h"
#include "bn_sprite_item.h"
#include "bn_fixed.h"
#include "bn_vector.h"
#include "game_constants.h"
#include "obj.h"

class ball;
class human;
class bau;
class enemy;

class dog: public live_obj
{
public:

    int palline_riportate = 0;
    int schema            = 1;

    dog(int skin_index);

    void update();
};
