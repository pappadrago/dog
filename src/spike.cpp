#include "spike.h"
#include "globals.h"
#include "dog.h"
#include "constanti_fisica.h"
#include "bn_math.h"
#include "bn_sprite_items_weapons.h"
#include "bn_sprite_items_doors.h"

static int durata(int v) { return v > 0 ? v : 1; }

spike::spike(const spike_def& d) : def(d)
{
    if (def.altezza != SPIKE_ALTA_32)
        def.altezza = SPIKE_ALTA_16;   // valori non validi -> 16

    _segno = def.verso_basso ? 1 : -1;
    _base = def.verso_basso ? (def.tile_y + 1) * 16 : def.tile_y * 16;

    sprite = (def.altezza == SPIKE_ALTA_32)
        ? bn::sprite_items::doors.create_sprite(0, 0, 6)
        : bn::sprite_items::weapons.create_sprite(0, 0, 10);
    sprite->set_camera(g_camera);
    sprite->set_bg_priority(3);                    // dietro al foreground: il solido lo nasconde
    sprite->set_vertical_flip(def.verso_basso);    // stessa grafica, capovolta

    for (int i = 0; i < def.sfasamento; ++i)
        avanza();
    aggiorna_sprite();
}

void spike::avanza()
{
    _ticks++;
    const int H = def.altezza;

    switch (_stato)
    {
    case ATTESA:
        _ext = 0;
        if (_ticks >= def.ticks_attesa) cambia(AVVISO);
        break;

    case AVVISO:
        _ext = SPIKE_EXT_AVVISO;
        if (_ticks >= def.ticks_avviso) cambia(SCATTO);
        break;

    case SCATTO:
    {
        int t = durata(def.ticks_scatto);
        _ext = SPIKE_EXT_AVVISO + ((H - SPIKE_EXT_AVVISO) * _ticks) / t;
        if (_ticks >= t) { _ext = H; cambia(ESTESA); }
        break;
    }

    case ESTESA:
        _ext = H;
        if (_ticks >= def.ticks_estesa) cambia(RITIRO);
        break;

    case RITIRO:
    {
        int t = durata(def.ticks_ritiro);
        _ext = H - (H * _ticks) / t;
        if (_ticks >= t) { _ext = 0; cambia(ATTESA); }
        break;
    }
    }
}

void spike::aggiorna_sprite()
{
    int cx = def.tile_x * 16 + 8;
    int cy = _base + _segno * (_ext - def.altezza / 2);

    if (_stato == AVVISO)
        cx += ((_ticks >> 1) & 1) ? 1 : -1;   // vibrazione orizzontale

    sprite->set_x(cx - HALF_SCREEN_W);
    sprite->set_y(cy - HALF_SCREEN_H);
    sprite->set_visible(_ext > 0);
}

void spike::controlla_cane()
{
    if (_ext < SPIKE_EXT_PERICOLO || g_dog->invulnerability > 0) return;

    int cx = def.tile_x * 16 + 8;

    // zona occupata dallo spuntone sopra il solido: [y1, y2]
    int punta = _base + _segno * _ext;
    int y1 = punta < _base ? punta : _base;
    int y2 = punta < _base ? _base : punta;

    bn::fixed dog_top = g_dog->chr_y - g_dog->box_dim;
    bn::fixed dog_bot = g_dog->chr_y + g_dog->box_dim;

    bool hit = bn::abs(g_dog->chr_x - cx) < (5 + g_dog->box_halfdim) &&
               dog_bot > y1 && dog_top < y2;

    if (hit)
    {
        // verso l'alto: rimbalzo in su; verso il basso: spinto via dal soffitto
        g_dog->chr_vy = def.verso_basso ? bn::fixed(1.5) : MEDIUM_JUMP_VY;
        g_dog->chr_vx = (g_dog->chr_x < cx) ? SMALL_VX_L : SMALL_VX_R;
        g_dog->invulnerability = 60;
        g_dog->dog_damage(def.danno);
    }
}

void spike::update()
{
    avanza();
    aggiorna_sprite();
    controlla_cane();
}