#include "oggetto_percorso_lineare.h"
#include "globals.h"
#include "dog.h"
#include "bn_sprite_items_items.h"
#include "bn_sprite_items_zombie.h"

namespace
{
    constexpr uint32_t PAUSA_ESTREMO = 30;
}

oggetto_percorso_lineare::oggetto_percorso_lineare(
    const oggetto_percorso_lineare_def& def, uint32_t sfasamento_ticks) :
    _def(def), _tick_iniziale(g_ostacoli_ticks), _sfasamento_ticks(sfasamento_ticks)
{
    if (_def.escursione < 1)
        _def.escursione = 1;
    if (_def.direzione != PERCORSO_LINEARE_VERTICALE)
        _def.direzione = PERCORSO_LINEARE_ORIZZONTALE;
    if (_def.velocita_x10 < 1)
        _def.velocita_x10 = 1;
    if (_def.sprite_size != PERCORSO_SPRITE_32)
        _def.sprite_size = PERCORSO_SPRITE_16;

    const uint32_t durata_numeratore = uint32_t(_def.escursione) * 30;
    _durata_tratto = (durata_numeratore + _def.velocita_x10 - 1) / _def.velocita_x10;
    if (_durata_tratto < 1)
        _durata_tratto = 1;

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

void oggetto_percorso_lineare::punto()
{
    const uint32_t durata_ciclo = (_durata_tratto + PAUSA_ESTREMO) * 2;
    const uint32_t tempo = ((g_ostacoli_ticks - _tick_iniziale) % durata_ciclo +
                            _sfasamento_ticks % durata_ciclo) % durata_ciclo;
    const uint32_t durata_fase = _durata_tratto + PAUSA_ESTREMO;
    const bool ritorno = tempo >= durata_fase;
    const uint32_t tempo_tratto = tempo % durata_fase;
    bn::fixed t = bn::fixed::from_data(int(
        (uint64_t(tempo_tratto) * bn::fixed::scale()) / _durata_tratto));
    if (t > 1)
        t = 1;

    // Smoothstep parte e arriva a velocita' zero, con la massima velocita' al centro.
    const bn::fixed progressione = t * t * (bn::fixed(3) - bn::fixed(2) * t);
    const bn::fixed posizione = ritorno ? bn::fixed(1) - progressione : progressione;
    const bn::fixed offset = (posizione * bn::fixed(2) - bn::fixed(1)) * bn::fixed(_def.escursione);

    chr_x = bn::fixed(_def.centro_x) + _centro_offset_x;
    chr_y = bn::fixed(_def.centro_y) + _centro_offset_y;
    if (_def.direzione == PERCORSO_LINEARE_VERTICALE)
        chr_y += offset;
    else
        chr_x += offset;
}

void oggetto_percorso_lineare::sposta_centro(bn::fixed offset_x, bn::fixed offset_y)
{
    _centro_offset_x = offset_x;
    _centro_offset_y = offset_y;
    punto();
    sprite->set_x(chr_x - HALF_SCREEN_W);
    sprite->set_y(chr_y - HALF_SCREEN_H);
}

void oggetto_percorso_lineare::update()
{
    punto();
    if (bn::abs(g_dog->chr_x - chr_x) > SCREEN_W ||
        bn::abs(g_dog->chr_y - chr_y) > SCREEN_H)
        return;

    sprite->set_x(chr_x - HALF_SCREEN_W);
    sprite->set_y(chr_y - HALF_SCREEN_H);
    controlla_cane();
}

void oggetto_percorso_lineare::controlla_cane()
{
    if (g_dog->invulnerability > 0)
        return;

    if (bn::abs(g_dog->chr_x - chr_x) < g_dog->box_halfdim + box_halfdim &&
        bn::abs(g_dog->chr_y - chr_y) < g_dog->box_dim + box_dim)
    {
        g_dog->dog_damage_and_jump(_def.danno, g_dog->chr_x < chr_x);
    }
}
