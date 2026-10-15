#pragma once

#include "bn_optional.h"
#include "bn_sprite_ptr.h"
#include "bn_fixed.h"
#include "game_constants.h"
#include "obj.h"

#define BOMB_MAX_BOUNCES      2    // rimbalzi sulla mappa prima dell'esplosione
#define BOMB_EXPLOSION_TICKS  27   // durata dell'esplosione (frame)
#define BOMB_EXPLOSION_FRAME_TICKS  3   // durata dell'esplosione (frame)
#define BOMB_EXPLOSION_RADIUS 20   // raggio del danno (px)
#define BOMB_TILE_INDEX       11    // frame di bn::sprite_items::weapons usato per la bomba (da adattare)

enum bomb_tipo : uint8_t
{
    BOMB_DAGGER = 0,
    BOMB_BOMB,
    BOMB_FIREBALL,
    BOMB_ARROW,
};


struct bomb_def
{
    u_int8_t tipoProjec;
    bn::fixed x, y, vx, vy;
    bool ignoreGravity, ignoreWalls;
    int max_bounces, ttl;
    uint8_t tileIndex;
    bn::fixed angle;
};


class bomb : public live_obj
{
public:
    bool finished = false;
    u_int8_t tipoProjec;
    bomb(bn::fixed x, bn::fixed y, bn::fixed vx, bn::fixed vy, int max_bounces = BOMB_MAX_BOUNCES);

    explicit bomb(const bomb_def& def);

    bn::optional<bn::sprite_animate_action<9>> actionExplode;
    void update();

private:
    bool ignoreGravity = false, ignoreWalls = false;
    int  max_bounces;
    int  bounces = 0;
    int  age = 0;
    int  ttl = 0;
    int  explosion_ticks = 0;
    bool exploded = false;
    void explode();
    void update_explosion();

    void update_physics();
    void update_graphics();
    void check_dog_hit();
    void hide_and_finish();
};