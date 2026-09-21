#pragma once

#include "bn_optional.h"
#include "bn_sprite_ptr.h"
#include "bn_fixed.h"
#include "game_constants.h"
#include "obj.h"

#define BOMB_MAX_BOUNCES      2    // rimbalzi sulla mappa prima dell'esplosione
#define BOMB_EXPLOSION_TICKS  24   // durata dell'esplosione (frame)
#define BOMB_EXPLOSION_RADIUS 20   // raggio del danno (px)
#define BOMB_TILE_INDEX       11    // frame di bn::sprite_items::weapons usato per la bomba (da adattare)

class bomb : public live_obj
{
public:
    bool finished = false;   // true a esplosione conclusa: l'owner puo' distruggerla

    bomb(bn::fixed x, bn::fixed y, bn::fixed vx, bn::fixed vy);

    void update();

private:
    int  bounces         = 0;
    int  age             = 0;
    int  explosion_ticks = 0;
    bool exploded        = false;

    void explode();
    void update_explosion();
};
