#pragma once

#include "bn_optional.h"
#include "bn_sprite_ptr.h"

class enemy;
class live_obj;

#define HEALTHBAR_FRAMES   9      // deve coincidere con il BMP
#define HEALTHBAR_DURATA   60    // 3 secondi
#define HEALTHBAR_FLICK    30     // lampeggia negli ultimi frame

class healthbar
{
public:
    bn::optional<bn::sprite_ptr> sprite;
    const live_obj* obj_target = nullptr;
    int ticks = 0;

    healthbar();

    void show(const live_obj* b);
    void update();               // una volta per frame
};