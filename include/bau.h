#pragma once

#include "bn_optional.h"
#include "bn_sprite_ptr.h"
#include "bn_fixed.h"
#include "obj.h"
#include "game_constants.h"

#define BAU_SPEED_R bn::fixed(2.0)
#define BAU_SPEED_L bn::fixed(-2.0)
#define BAU_TICKS_BETWEEN_BAU 30
#define BAU_RANGE bn::fixed(140)
#define BAU_DURATA 60
#define BAU_FLICK_TIME 15
#define BAU_DAMAGE 10

#define BAU_CHARGE_MAX 60   // frame di pressione per la carica massima (1 s)
#define BAU_CHARGE_MIN_RILASCIO 20   // sotto questa carica il rilascio non spara: un tap lancia solo il primo bau

class bau: public obj
{
public:

    bn::fixed power = 0;      // 0..1, calcolata allo sparo
    bn::fixed hit_half = 8;   // mezza hitbox, cresce con la carica

    int ticks = 0;
        

    bau();

    void do_spawn(int charge = 0);
    void update();
};