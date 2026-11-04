#include "spike.h"
#include "globals.h"
#include "dog.h"
#include "constanti_fisica.h"
#include "bn_math.h"
#include "bn_sprite_items_weapons.h"   // nuovo asset: 16x16, spuntone rivolto verso l'alto

static int durata(int v) { return v > 0 ? v : 1; }   // evita divisioni per zero

spike::spike(const spike_def& d) : def(d)
{
    sprite = bn::sprite_items::weapons.create_sprite(0, 0, 10);
    sprite->set_camera(g_camera);
    sprite->set_bg_priority(3);   // dietro al foreground (2): il pavimento lo nasconde

    // fast-forward: applica lo sfasamento senza grafica né danni
    for (int i = 0; i < def.sfasamento; ++i)
        avanza();
    aggiorna_sprite();
}

void spike::avanza()
{
    _ticks++;
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
        _ext = SPIKE_EXT_AVVISO + ((SPIKE_MAX_EXT - SPIKE_EXT_AVVISO) * _ticks) / t;
        if (_ticks >= t) { _ext = SPIKE_MAX_EXT; cambia(ESTESA); }
        break;
    }

    case ESTESA:
        _ext = SPIKE_MAX_EXT;
        if (_ticks >= def.ticks_estesa) cambia(RITIRO);
        break;

    case RITIRO:
    {
        int t = durata(def.ticks_ritiro);
        _ext = SPIKE_MAX_EXT - (SPIKE_MAX_EXT * _ticks) / t;
        if (_ticks >= t) { _ext = 0; cambia(ATTESA); }
        break;
    }
    }
}

void spike::aggiorna_sprite()
{
    // Estensione 0 = sprite interamente dentro la tile del pavimento
    int cx = def.tile_x * 16 + 8;
    int cy = def.tile_y * 16 + 8 - _ext;

    // Vibrazione solo nella fase di avviso: alterna ±1px ogni 2 frame
    if (_stato == AVVISO)
        cx += ((_ticks >> 1) & 1) ? 1 : -1;

    sprite->set_x(cx - HALF_SCREEN_W);
    sprite->set_y(cy - HALF_SCREEN_H);
    sprite->set_visible(_ext > 0);
}

void spike::controlla_cane()
{
    if (_ext < SPIKE_EXT_PERICOLO || g_dog->invulnerability > 0) return;

    int cx = def.tile_x * 16 + 8;
    int top = def.tile_y * 16 - _ext;       // punta
    int base = def.tile_y * 16;             // bordo superiore del pavimento

    // hitbox cane: x ±box_halfdim, y ±box_dim (centrata su chr_y)
    bn::fixed dog_top = g_dog->chr_y - g_dog->box_dim;
    bn::fixed dog_bot = g_dog->chr_y + g_dog->box_dim;

    bool hit = bn::abs(g_dog->chr_x - cx) < (5 + g_dog->box_halfdim) &&   // spuntone largo ~10px
               dog_bot > top && dog_top < base;

    if (hit)
    {
        g_dog->chr_vy = MEDIUM_JUMP_VY;
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