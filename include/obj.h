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

static constexpr bn::fixed RUN_EXTRA_SPEED = bn::fixed(0.5);

static constexpr bn::fixed ZERO = bn::fixed(0.0);

class obj
{
public:
    bn::optional<bn::sprite_ptr>               sprite;

    bn::optional<bn::sprite_animate_action<8>> actionStand;
    bn::optional<bn::sprite_animate_action<8>> actionWalk;
    bn::optional<bn::sprite_item>              spriteItems;

    bn::fixed chr_y = 512 - 200;
    bn::fixed chr_x = 100;
    bn::fixed chr_vy = ZERO;
    bn::fixed chr_accx = ZERO;
    bn::fixed chr_vx = ZERO;

    bn::fixed box_dim = bn::fixed(16);
    bn::fixed box_halfdim = bn::fixed(7);
};

class scala;

class live_obj : public obj
{
public:

    bool onGround = false;
    int  dir = 1;
    int ticks2action = 0, invulnerability = 0;
    int life = START_LIFE;
    bn::fixed max_vx = bn::fixed(1.0);

    bool atterrato_ora = false;           // true solo nel frame esatto in cui tocca terra
    bn::fixed velocita_atterraggio = ZERO;   // chr_vy un istante prima di essere azzerata
    bool testata_ora = false;
    bool salto_in_corso = false;
    int muro_lato = 0;   // DIR_LEFT/DIR_RIGHT se in questo istante c'è un muro di fianco, 0 se nessuno
    void apply_gravity();
    void apply_friction();
    void apply_map();

    // --- scale ---
    bool puo_scalare = false;        // dog: true; enemy: true solo con ATTR_CLIMB
    scala* scala_corrente = nullptr;
    scala* scala_bloccata = nullptr;  // scala da cui sono appena uscito automaticamente
    int    scala_blocco_dir = 0;      // direzione (-1 su / +1 giù) che non deve riagganciarmi
    int    intent_scala = 0;
    bn::fixed scala_vy = ZERO;        // velocità verticale durante la scala, con inerzia
    bool su_scala() const { return scala_corrente != nullptr; }
    void stacca_da_scala(bn::fixed vx = ZERO, bn::fixed vy = ZERO); // salto/rilascio manuale

    bn::fixed wind_factor = 1;   // 0 = immune, <1 = pesante
    void apply_wind();

};


// true se il punto (x, y) in coordinate mondo cade in un tile solido
// (fuori mappa conta come solido)
bool is_solid_at(bn::fixed x, bn::fixed y, int schema);
bool is_standable_at(bn::fixed x, bn::fixed y, int schema);
