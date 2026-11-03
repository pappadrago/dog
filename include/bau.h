#pragma once

#include "bn_sprite_ptr.h"
#include "bn_fixed.h"
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

class bau
{
public:

    bn::sprite_ptr sprite;
    bn::fixed chr_x = 0, chr_y = 0, chr_vx = 0;
    uint8_t charge_ticks = 0;   // 0..BAU_CHARGE_MAX
    int8_t ticks = 0;     // positivo = bau in volo; negativo = animazione d'impatto


    bau();

    void do_spawn(int charge = 0);
    void update();
};