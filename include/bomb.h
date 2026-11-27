#pragma once

#include "bn_optional.h"
#include "bn_sprite_ptr.h"
#include "bn_fixed.h"
#include "game_constants.h"
#include "obj.h"

#define BOMB_MAX_BOUNCES      2    // rimbalzi sulla mappa prima dell'esplosione
#define BOMB_DAGGER_LIFETIME  30
#define BOMB_PROJECTILE_LIFETIME 120
#define BOMB_MINA_LIFETIME    360
#define BOMB_MINA_FUSE_TICKS  30
#define BOMB_EXPLOSION_FRAME_TICKS  3   // durata dell'esplosione (frame)
#define BOMB_EXPLOSION_RADIUS 20   // raggio del danno (px)
#define BOMB_EXPLOSION_FIRST_TILE 9
#define BOMB_TILE_INDEX       11    // frame di bn::sprite_items::weapons usato per la bomba (da adattare)
#define BOMB_ORB_WAVE_STEP 8              // gradi di fase per frame (periodo = 360/8 = 45 frame)
#define BOMB_ORB_WAVE_K    bn::fixed(0.6) // ampiezza: più alto = onda più larga

enum bomb_tipo : uint8_t
{
    BOMB_DAGGER = 0,
    BOMB_BOMB,
    BOMB_FIREBALL,
    BOMB_BULLET,
    BOMB_ARROW,
    BOMB_MINA,
    BOMB_FULMINE,
    BOMB_ORB
};


struct bomb_def
{
    u_int8_t tipoProjec;
    bn::fixed x, y, vx, vy;
    bool ignoreGravity, ignoreWalls;
    int max_bounces;
    uint8_t tileIndex;
    bn::fixed angle;
};


class bomb : public live_obj
{
public:
    bool finished = false;
    u_int8_t tipoProjec;
    explicit bomb(const bomb_def& def);

    bn::optional<bn::sprite_animate_action<9>> actionExplode;
    bn::optional<bn::sprite_animate_action<16>> actionThunder;
    bn::optional<bn::sprite_animate_action<16>> actionFulmine;
    bn::optional<bn::sprite_animate_action<4>> actionBullet;
    bn::optional<bn::sprite_animate_action<8>> actionStartBullet;
    void update();

private:
    bool ignoreGravity = false, ignoreWalls = false;
    int  max_bounces;
    int  bounces = 0;
    bool exploded = false;
    void explode();
    void update_explosion();

    void hide_and_finish();
};

bomb* spawn_bomb(const bomb_def& def, int direction = 0);