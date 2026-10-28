#include "wind_fx.h"
#include "globals.h"
#include "bn_math.h"
#include "bn_sprite_items_fox1632.h"

namespace
{
    
    constexpr bn::fixed SCREEN_SPEED   = 6;      // px schermo per ogni px/frame di vento
    constexpr bn::fixed MIN_WIND       = bn::fixed(0.05);
    constexpr bn::fixed MEDIUM_WIND    = bn::fixed(0.25);
    constexpr bn::fixed STRONG_WIND    = bn::fixed(0.45);
    constexpr int       WRAP_X         = 136;    // oltre il bordo (±120) + metà sprite
}

wind_fx::wind_fx()
{
    for(int i = 0; i < NUM_PARTICLES; ++i)
    {
        bn::fixed x = g_rng.get_int(240) - 120;
        bn::fixed y = g_rng.get_int(140) - 70;   // evita la fascia dell'HUD

        bn::sprite_ptr spr = bn::sprite_items::fox1632.create_sprite(x, y, 59);
        spr.set_bg_priority(1);
        spr.set_z_order(-5);
        spr.set_visible(false);

        _particles.push_back(particle{
            spr,
            bn::fixed(1) + bn::fixed(g_rng.get_int(100)) / 100,
            y,
            g_rng.get_int(360) });
    }
}

void wind_fx::update()
{
    bn::fixed w  = g_wind_x;
    bn::fixed aw = bn::abs(w);
    bool active  = aw > MIN_WIND;

    int frame = aw < MEDIUM_WIND ? 57 : (aw < STRONG_WIND ? 58 : 59);
    bool change_frame = (frame != _frame);
    _frame = frame;
    ++_ticks;

    for(particle& p : _particles)
    {
        p.sprite.set_visible(active);
        if(!active)
            continue;

        bn::fixed x = p.sprite.x() + w.multiplication(p.speed_mul).multiplication(SCREEN_SPEED);

        // uscita da un lato: rientra dall'altro a una nuova altezza
        if(x > WRAP_X || x < -WRAP_X)
        {
            x = (w > 0) ? bn::fixed(-WRAP_X) : bn::fixed(WRAP_X);
            p.base_y = g_rng.get_int(140) - 70;
        }

        // leggero ondeggiamento verticale
        bn::fixed wobble = bn::degrees_lut_sin_safe(
            bn::fixed((p.phase + _ticks * 4) % 360)).multiplication(2);

        p.sprite.set_x(x);
        p.sprite.set_y(p.base_y + wobble);
        p.sprite.set_horizontal_flip(w < 0);

        if(change_frame)
            p.sprite.set_tiles(bn::sprite_items::fox1632.tiles_item(), frame);
    }
}