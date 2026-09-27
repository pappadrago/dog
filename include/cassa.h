// include/cassa.h
#pragma once
#include "bn_optional.h"
#include "bn_sprite_ptr.h"
#include "obj.h"

#define CASSA_SCATOLA  0
#define CASSA_INCUDINE 1
#define CASSA_MATTONI  2

class cassa : public live_obj
{
public:
    uint8_t tipo;
    bool a_terra    = false;
    bool distrutta  = false;
    int  ticks_a_terra = 0;

    cassa(uint8_t tipo, bn::fixed x, bn::fixed y);
    void update();
};