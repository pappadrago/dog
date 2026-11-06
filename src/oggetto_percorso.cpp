#include "oggetto_percorso.h"
#include "globals.h"
#include "dog.h"
#include "bn_math.h"
#include "bn_sprite_animate_actions.h"
#include "bn_sprite_items_weapons.h"
#include "bn_sprite_items_zombie.h"
#include "bn_math.h"

namespace
{
    bn::fixed calcola_perimetro(const oggetto_percorso_def& def)
    {
        bn::fixed a(def.semiasse_x);
        bn::fixed b(def.semiasse_y);

        switch (def.forma)
        {
        case PERCORSO_RETTANGOLO:
        case PERCORSO_OTTO:
            return (a + b) * 4;
        default:
            return (a + b).multiplication(bn::fixed(3.14159));
        }
    }
}

oggetto_percorso::oggetto_percorso(const oggetto_percorso_def& def) : _def(def)
{
    if (_def.semiasse_x < 1)
        _def.semiasse_x = 1;
    if (_def.semiasse_y < 1)
        _def.semiasse_y = 1;
    if (_def.forma > PERCORSO_OTTO)
        _def.forma = PERCORSO_ELLISSE;
    if (_def.sprite_size != PERCORSO_SPRITE_32)
        _def.sprite_size = PERCORSO_SPRITE_16;
    _perimetro = calcola_perimetro(_def);
    _segno = _def.oraria ? 1 : -1;
    _delta = bn::fixed(_def.velocita_x10 * 36).division(_perimetro);

    punto(fase_corrente());
    if (_def.sprite_size == PERCORSO_SPRITE_32)
    {
        sprite = bn::sprite_items::zombie.create_sprite(
            chr_x - HALF_SCREEN_W, chr_y - HALF_SCREEN_H, 0);
        _animazione = bn::create_sprite_animate_action_forever(
            *sprite, 10, bn::sprite_items::zombie.tiles_item(), 0, 1, 2, 3);
        box_dim = bn::fixed(16);
        box_halfdim = bn::fixed(16);
    }
    else
    {
        sprite = bn::sprite_items::weapons.create_sprite(
            chr_x - HALF_SCREEN_W, chr_y - HALF_SCREEN_H, 0);
        _animazione = bn::create_sprite_animate_action_forever(
            *sprite, 6, bn::sprite_items::weapons.tiles_item(), 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47);
        box_dim = bn::fixed(8);
        box_halfdim = bn::fixed(8);
    }
    sprite->set_camera(g_camera);
    sprite->set_bg_priority(1);
}

bn::fixed oggetto_percorso::wrap360(bn::fixed fase) const
{
    while (fase >= 360)
        fase -= 360;
    while (fase < 0)
        fase += 360;
    return fase;
}

bn::fixed oggetto_percorso::fase_corrente() const
{
    const int64_t giro_data = int64_t(360) * bn::fixed::scale();
    int64_t fase_data = (int64_t(_delta.data()) * g_ostacoli_ticks) % giro_data;
    fase_data *= _segno;
    return wrap360(bn::fixed(_def.fase_iniziale) + bn::fixed::from_data(int(fase_data)));
}

void oggetto_percorso::punto(bn::fixed fase)
{
    bn::fixed a(_def.semiasse_x);
    bn::fixed b(_def.semiasse_y);
    bn::fixed cx(_def.centro_x);
    bn::fixed cy(_def.centro_y);

    switch (_def.forma)
    {
    case PERCORSO_RETTANGOLO:
    {
        bn::fixed s = fase.multiplication(_perimetro).division(360);
        bn::fixed w = a * 2;
        bn::fixed h = b * 2;
        if (s < w) { chr_x = cx - a + s;     chr_y = cy - b; }
        else {
            s -= w;
            if (s < h) { chr_x = cx + a;         chr_y = cy - b + s; }
            else {
                s -= h;
                if (s < w) { chr_x = cx + a - s;     chr_y = cy + b; }
                else { chr_x = cx - a;         chr_y = cy + b - (s - w); }
            }
        }
        break;
    }
    case PERCORSO_OTTO:
        chr_x = cx + a.multiplication(bn::degrees_lut_sin_safe(fase));
        chr_y = cy + b.multiplication(bn::degrees_lut_sin_safe(wrap360(fase * 2)));
        break;
    default:
        chr_x = cx + a.multiplication(bn::degrees_lut_cos_safe(fase));
        chr_y = cy + b.multiplication(bn::degrees_lut_sin_safe(fase));
        break;
    }
}

void oggetto_percorso::update()
{

    if ( bn::abs(g_dog->chr_x - chr_x) > SCREEN_W)
        return; // cane troppo lontano, non aggiorno 

    punto(fase_corrente());
    sprite->set_x(chr_x - HALF_SCREEN_W);
    sprite->set_y(chr_y - HALF_SCREEN_H);
    _animazione->update();
    controlla_cane();
}

void oggetto_percorso::controlla_cane()
{
    if (g_dog->invulnerability > 0)
        return;

    if (bn::abs(g_dog->chr_x - chr_x) < g_dog->box_halfdim + box_halfdim &&
        bn::abs(g_dog->chr_y - chr_y) < g_dog->box_dim + box_dim)
    {
        g_dog->dog_damage_and_jump(_def.danno, g_dog->chr_x < chr_x);
    }
}
