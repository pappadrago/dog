#pragma once

#include "bn_sprite_ptr.h"
#include "bn_vector.h"
#include "bn_fixed.h"

// Scie di vento in coordinate schermo (senza camera): non seguono la mappa,
// ma scorrono in base a g_wind_x.
class wind_fx
{
public:
    static constexpr int NUM_PARTICLES = 8;

    wind_fx();
    void update();   // una volta per frame, dopo wind_update()

private:
    struct particle
    {
        bn::sprite_ptr sprite;
        bn::fixed speed_mul;    // 1.0..2.0: velocità diverse danno profondità
        bn::fixed base_y;
        int phase;              // sfasamento dell'ondeggiamento verticale
    };

    bn::vector<particle, NUM_PARTICLES> _particles;
    int _frame = -1;            // frame corrente (-1 = da impostare)
    int _ticks = 0;
};