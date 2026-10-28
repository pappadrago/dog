#pragma once

#include "bn_sprite_ptr.h"
#include "bn_vector.h"
#include "bn_fixed.h"

// 8 particelle logiche, 4 sprite: ogni frame si disegna una metà delle particelle
class wind_fx
{
public:
    static constexpr int NUM_PARTICLES = 8;
    static constexpr int NUM_SPRITES   = NUM_PARTICLES / 2;

    wind_fx();
    void update();   // una volta per frame, dopo wind_update()

private:
    struct particle
    {
        bn::fixed x;
        bn::fixed y;            // y corrente (fascia + ondeggiamento)
        bn::fixed speed_mul;    // 1.0..2.0
        int band;               // fascia verticale 0..7
        int phase;
    };

    particle _particles[NUM_PARTICLES];
    bn::vector<bn::sprite_ptr, NUM_SPRITES> _sprites;
    int _frame = -1;
    int _ticks = 0;

    static bn::fixed band_y(int band);
};