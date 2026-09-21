#pragma once

#include "bn_optional.h"
#include "bn_sprite_ptr.h"
#include "bn_sprite_animate_actions.h"
#include "bn_fixed.h"
#include "game_constants.h"
#include "obj.h"
#include "bomb.h"

class ball;
class bau;
class dog;

#define WEAPON_FLICK_TIME 25

#define TIPO_NEMICO_GENERICO 0
#define TIPO_NEMICO_DRUIDO_DINAMICO 1
#define TIPO_NEMICO_DRUIDO_STATICO 2
#define TIPO_NEMICO_ARCIERE_STATICO 3
#define TIPO_NEMICO_BLOB_PATTUGLIATORE 4
#define TIPO_NEMICO_SPADACCINO_PATTUGLIATORE 5
#define TIPO_NEMICO_BOMBAROLO 6


#define ATTRIBUTO_NO 0
#define ATTRIBUTO_AIM 1

#define ACTION_STAND 0
#define ACTION_MOVE 1
#define ACTION_ATTACK 2
#define ACTION_STUN 3

class enemy : public live_obj
{
public:
    bn::optional<bomb> bomba;   // creata al lancio, distrutta a esplosione finita
    void throw_bomb();

    bn::optional<bn::sprite_ptr>               weaponSprite;

    bn::fixed wx_base = 0, wy_base = 0;
    bn::fixed wpn_vx = 0, wpn_vy = 0;
    bn::fixed weapon_time = bn::fixed(120.0);
    int weaponDir = DIR_RIGHT;
    uint8_t tipo = 0;
    uint8_t attributo = 0;
    uint8_t currentAction = ACTION_STAND;
    uint16_t weaponTicks = 0;

    enemy(u_int8_t tipo, u_int8_t attributo);

    void init();
    virtual void update();
    virtual ~enemy() = default;  // distruttore virtual obbligatorio

    void beHitByBark(int _dir);
};

