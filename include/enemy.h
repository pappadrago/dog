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

#define TIPO_NEMICO_GENERICO 0
#define TIPO_NEMICO_DRUIDO_DINAMICO 1
#define TIPO_NEMICO_DRUIDO_STATICO 2
#define TIPO_NEMICO_ARCIERE_STATICO 3
#define TIPO_NEMICO_BLOB_PATTUGLIATORE 4
#define TIPO_NEMICO_SPADACCINO_PATTUGLIATORE 5

#define ATTRIBUTO_NO 0
#define ATTRIBUTO_AIM 1

#define ACTION_STAND 0
#define ACTION_MOVE 1
#define ACTION_ATTACK 2
#define ACTION_STUN 3

class enemy : public live_obj
{
public:
    
    bn::optional<bn::sprite_ptr>               weaponSprite;

    bn::fixed wx_base, wy_base;
    bn::fixed wpn_vx, wpn_vy;
    bn::fixed weapon_time = bn::fixed(120.0);
    int weaponDir ;
    uint8_t tipo;
    uint8_t attributo;
    uint8_t currentAction;
    uint16_t weaponTicks;

    enemy(u_int8_t tipo, u_int8_t attributo);

    void init();
    virtual void update();
    virtual ~enemy() = default;  // distruttore virtual obbligatorio

    void beHitByBark(int _dir);
};

