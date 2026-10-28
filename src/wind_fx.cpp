#include "wind_fx.h"
#include "globals.h"
#include "bn_math.h"
#include "bn_sprite_items_fox1632.h"

namespace
{
    constexpr bn::fixed SCREEN_SPEED = 8;
    constexpr bn::fixed MIN_WIND     = bn::fixed(0.05);
    constexpr bn::fixed MEDIUM_WIND  = bn::fixed(0.25);
    constexpr bn::fixed STRONG_WIND  = bn::fixed(0.45);
    constexpr int       WRAP_X       = 136;
    constexpr int       BAND_H       = 17;   // 8 fasce * 17 = 136 px
}

bn::fixed wind_fx::band_y(int band)
{
    return bn::fixed(-68 + band * BAND_H + g_rng.get_int(BAND_H - 5));
}

wind_fx::wind_fx()
{
    for(int i = 0; i < NUM_PARTICLES; ++i)
    {
        particle& p = _particles[i];
        p.x         = bn::fixed(g_rng.get_int(240) - 120);
        p.band      = i;
        p.y         = band_y(i);
        p.speed_mul = bn::fixed(1) + bn::fixed(g_rng.get_int(100)) / 100;
        p.phase     = g_rng.get_int(360);
    }

    for(int i = 0; i < NUM_SPRITES; ++i)
    {
        bn::sprite_ptr spr = bn::sprite_items::fox1632.create_sprite(0, 0, 57);
        spr.set_bg_priority(1);
        spr.set_z_order(-5);
        spr.set_visible(false);
        _sprites.push_back(spr);
    }
}

void wind_fx::update()
{
    ++_ticks;

    bn::fixed w  = g_wind_x;
    bn::fixed aw = bn::abs(w);

    if(aw <= MIN_WIND)
    {
        for(bn::sprite_ptr& s : _sprites)
            s.set_visible(false);
        return;
    }

    int frame = aw < MEDIUM_WIND ? 57 : (aw < STRONG_WIND ? 58 : 59);
    bool change_frame = (frame != _frame);
    _frame = frame;

    // 1) logica: tutte e 8 le particelle avanzano ogni frame
    for(particle& p : _particles)
    {
        p.x += w.multiplication(p.speed_mul).multiplication(SCREEN_SPEED);

        if(p.x > WRAP_X || p.x < -WRAP_X)
        {
            p.x = (w > 0) ? bn::fixed(-WRAP_X) : bn::fixed(WRAP_X);
            p.y = band_y(p.band);
        }
    }

    // 2) rendering: lo slot k mostra la particella k (frame pari) o k+4 (dispari)
    int offset = (_ticks & 1) ? NUM_SPRITES : 0;

    for(int k = 0; k < NUM_SPRITES; ++k)
    {
        const particle& p = _particles[k + offset];
        bn::sprite_ptr& s = _sprites[k];

        bn::fixed wobble = bn::degrees_lut_sin_safe(
            bn::fixed((p.phase + _ticks * 4) % 360)).multiplication(2);

        s.set_position(p.x, p.y + wobble);
        s.set_horizontal_flip(w < 0);
        if(change_frame)
            s.set_tiles(bn::sprite_items::fox1632.tiles_item(), frame);
        s.set_visible(true);
    }
}