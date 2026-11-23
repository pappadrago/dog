#include "oggetto_percorso_lineare.h"
#include "percorso_sprite.h"
#include "globals.h"
#include "dog.h"

namespace
{
    constexpr int TREMOLIO_TICKS = 24;      // durata del tremolio, alla fine della pausa
    constexpr int TREMOLIO_AMPIEZZA = 1;    // px
}

oggetto_percorso_lineare::oggetto_percorso_lineare(
    const oggetto_percorso_lineare_def& def, uint32_t sfasamento_ticks) :
    _def(def), _tick_iniziale(g_ostacoli_ticks), _sfasamento_ticks(sfasamento_ticks)
{
    if (_def.escursione < 1)
        _def.escursione = 1;
    if (_def.direzione != PERCORSO_LINEARE_VERTICALE)
        _def.direzione = PERCORSO_LINEARE_ORIZZONTALE;
    if (_def.sprite_size != PERCORSO_SPRITE_32)
        _def.sprite_size = PERCORSO_SPRITE_16;

    punto();
    if (_def.sprite_size == PERCORSO_SPRITE_32)
    {
        sprite = bn::sprite_items::zombie.create_sprite(chr_x - HALF_SCREEN_W, chr_y - HALF_SCREEN_H, 18);
        box_dim = bn::fixed(12);
        box_halfdim = bn::fixed(12);
    }
    else
    {
        sprite = bn::sprite_items::items.create_sprite(chr_x - HALF_SCREEN_W, chr_y - HALF_SCREEN_H, 19);
        box_dim = bn::fixed(8);
        box_halfdim = bn::fixed(8);
    }
    sprite->set_camera(g_camera);
    sprite->set_bg_priority(1);
}

void oggetto_percorso_lineare::punto()
{
    const int durata_ciclo = ((_def.durata_tratto + _def.pausa_tratto) * 2);
    const int durata_fase = (_def.durata_tratto + _def.pausa_tratto);
    const int tempo = int(((g_ostacoli_ticks - _tick_iniziale) % durata_ciclo +
        _sfasamento_ticks % durata_ciclo) % durata_ciclo);

    const bool ritorno = tempo >= durata_fase;
    const int tt_raw = tempo % durata_fase;
    const int durata = int(_def.durata_tratto);
    const int tt = tt_raw > durata ? durata : tt_raw;   // pausa all'estremo

    const int pos = ritorno ? durata - tt : tt;   // 0..durata

    const bool in_pausa = tt_raw >= _def.durata_tratto;

    // Il costruttore chiama punto() prima di creare lo sprite.
    aggiorna_sprite_percorso(sprite, _def.sprite_size, in_pausa);
    // p in [0, scale], poi smoothstep e = p^2 * (3 - 2p), tutto in interi.
    // Passi intermedi: massimo ~5e7, ben dentro int32.
    constexpr int S = bn::fixed::scale();
    const int p = int((int64_t(pos) * S) / durata);
    const int p2 = (p * p) / S;                 // p^2
    const int e = (p2 * (3 * S - 2 * p)) / S;   // 0..S

    // offset in [-escursione, +escursione], già in unità raw di fixed
    const bn::fixed offset = bn::fixed::from_data((2 * e - S) * int(_def.escursione));

    chr_x = bn::fixed(_def.centro_x) + _centro_offset_x;
    chr_y = bn::fixed(_def.centro_y) + _centro_offset_y;

    if (_def.direzione == PERCORSO_LINEARE_VERTICALE)
        chr_y += offset;
    else
        chr_x += offset;

    chr_x = bn::fixed(chr_x.round_integer());
    chr_y = bn::fixed(chr_y.round_integer());

    // Tremolio: solo negli ultimi tick della pausa, prima di ripartire.
    // Se la pausa è più corta di TREMOLIO_TICKS, il tremolio occupa tutta la pausa.
    const int pausa = int(_def.pausa_tratto);
    const int durata_tremolio = pausa < TREMOLIO_TICKS ? pausa : TREMOLIO_TICKS;
    const int tick_rimasti = durata_fase - tt_raw;   // alla ripartenza vale 0

    if (durata_tremolio > 0 && tt_raw >= durata && tick_rimasti <= durata_tremolio)
    {
        // alterna ogni 2 tick: ogni tick a 60 fps sembrerebbe solo uno sfocato
        const int s = ((g_ostacoli_ticks >> 1) & 1) ? TREMOLIO_AMPIEZZA : -TREMOLIO_AMPIEZZA;

        // perpendicolare alla direzione di marcia
        if (_def.direzione == PERCORSO_LINEARE_VERTICALE)
            chr_x += s;
        else
            chr_y += s;
    }
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
    if (bn::abs(g_dog->chr_x - chr_x) > SCREEN_W ||
        bn::abs(g_dog->chr_y - chr_y) > SCREEN_H)
        return;
    punto();

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
