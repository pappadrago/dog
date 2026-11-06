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

    imposta_fase();
    aggiorna_sprite();
}

void spike::imposta_fase()
{
    const int H = def.altezza;
    const int durate[] = {
        durata(def.ticks_attesa),
        durata(def.ticks_avviso),
        durata(def.ticks_scatto),
        durata(def.ticks_estesa),
        durata(def.ticks_ritiro)
    };
    const stato stati[] = { ATTESA, AVVISO, SCATTO, ESTESA, RITIRO };
    uint32_t ciclo = 0;
    for (int d : durate)
        ciclo += d;

    uint32_t fase = uint32_t((uint64_t(g_ostacoli_ticks) + def.sfasamento) % ciclo);
    uint32_t inizio = 0;

    for (int i = 0; i < 5; ++i)
    {
        if (fase < inizio + uint32_t(durate[i]))
        {
            _stato = stati[i];
            _ticks = int(fase - inizio);
            if (_ticks == 0)
            {
                _ext = i == 0 ? 0 : (i == 1 ? 0 : (i == 2 ? SPIKE_EXT_AVVISO : H));
            }
            else
            {
                switch (i)
                {
                case 0:
                    _ext = 0;
                    break;
                case 1:
                    _ext = SPIKE_EXT_AVVISO;
                    break;
                case 2:
                    _ext = SPIKE_EXT_AVVISO +
                        ((H - SPIKE_EXT_AVVISO) * _ticks) / durate[i];
                    break;
                case 3:
                    _ext = H;
                    break;
                case 4:
                    _ext = H - (H * _ticks) / durate[i];
                    break;
                default:
                    _ext = 0;
                    break;
                }
            }
            return;
        }
        inizio += uint32_t(durate[i]);
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
    imposta_fase();
    aggiorna_sprite();
    controlla_cane();
}