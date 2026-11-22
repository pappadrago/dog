#include "pingpong.h"
#include "globals.h"
#include "dog.h"
#include "bn_sprite_items_items.h"
#include "bn_sprite_items_zombie.h"

pingpong::pingpong(
    const pingpong_def& def, uint32_t sfasamento_ticks) :
    _def(def), _tick_iniziale(g_ostacoli_ticks), _sfasamento_ticks(sfasamento_ticks)
{
    constexpr int S = bn::fixed::scale();

    if (_def.vy_x100 < 1)
        _def.vy_x100 = 1;
    if (_def.gravita_x100 < 1)
        _def.gravita_x100 = 1;
    if (_def.vx_x100 < 0)
        _def.vx_x100 = 0;
    if (_def.sprite_size != PERCORSO_SPRITE_32)
        _def.sprite_size = PERCORSO_SPRITE_16;

    const int vy = _def.vy_x100;
    const int g = _def.gravita_x100;

    // T = 2*vy/g, arrotondato; almeno 2 tick
    _durata_volo = (2 * vy + g / 2) / g;
    if (_durata_volo < 2)
        _durata_volo = 2;

    // H = vy^2 / (2g)  (i due x100 si semplificano, resta un /100)
    _altezza_raw = int((int64_t(vy) * vy * S) / (int64_t(2) * g * 100));

    // R = vx * T / 2
    _semigittata_raw = int((int64_t(_def.vx_x100) * _durata_volo * S) / 200);

    punto();
    if (_def.sprite_size == PERCORSO_SPRITE_32)
    {
        sprite = bn::sprite_items::zombie.create_sprite(
            chr_x - HALF_SCREEN_W, chr_y - HALF_SCREEN_H, 18);
        box_dim = bn::fixed(16);
        box_halfdim = bn::fixed(16);
    }
    else
    {
        sprite = bn::sprite_items::items.create_sprite(
            chr_x - HALF_SCREEN_W, chr_y - HALF_SCREEN_H, 19);
        box_dim = bn::fixed(8);
        box_halfdim = bn::fixed(8);
    }
    sprite->set_camera(g_camera);
    sprite->set_bg_priority(1);
}

void pingpong::punto()
{
    constexpr int S = bn::fixed::scale();

    const int durata_fase = _durata_volo + int(_def.pausa);
    const int durata_ciclo = durata_fase * 2;
    const int tempo = int(((g_ostacoli_ticks - _tick_iniziale) % durata_ciclo +
        _sfasamento_ticks % durata_ciclo) % durata_ciclo);

    const bool ritorno = tempo >= durata_fase;
    int tt = tempo % durata_fase;
    if (tt > _durata_volo)
        tt = _durata_volo;                 // fermo all'estremo durante la pausa

    // p in [0, S]; q = p*(1-p) in [0, S/4]
    const int p = int((int64_t(tt) * S) / _durata_volo);
    const int q = int((int64_t(p) * (S - p)) / S);

    // quota sopra la base: 4*H*p*(1-p)
    const int64_t salita = (int64_t(_altezza_raw) * 4 * q) / S;

    // avanzamento orizzontale in [0, 2R]
    const int64_t avanzamento = (int64_t(2) * _semigittata_raw * p) / S;
    const int64_t dx = ritorno ? _semigittata_raw - avanzamento
                               : avanzamento - _semigittata_raw;

    chr_x = bn::fixed(_def.centro_x) + bn::fixed::from_data(int(dx));
    chr_y = bn::fixed(_def.base_y) - bn::fixed::from_data(int(salita));

    chr_x = bn::fixed(chr_x.round_integer());
    chr_y = bn::fixed(chr_y.round_integer());
}

void pingpong::update()
{
    if (bn::abs(g_dog->chr_x - chr_x) > SCREEN_W ||
        bn::abs(g_dog->chr_y - chr_y) > SCREEN_H)
        return;
   punto();
 
    sprite->set_x(chr_x - HALF_SCREEN_W);
    sprite->set_y(chr_y - HALF_SCREEN_H);
    controlla_cane();
}

void pingpong::controlla_cane()
{
    if (g_dog->invulnerability > 0)
        return;

    if (bn::abs(g_dog->chr_x - chr_x) < g_dog->box_halfdim + box_halfdim &&
        bn::abs(g_dog->chr_y - chr_y) < g_dog->box_dim + box_dim)
    {
        g_dog->dog_damage_and_jump(_def.danno, g_dog->chr_x < chr_x);
    }
}