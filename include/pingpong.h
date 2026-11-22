#pragma once

#include "oggetto_percorso.h"

struct pingpong_def
{
    int16_t  centro_x;        // x a metà tra partenza e arrivo
    int16_t  base_y;          // y di partenza e di arrivo (terra)
    int16_t  vx_x100;         // velocità orizzontale in px/frame x100 (200 = 2.0)
    int16_t  vy_x100;         // velocità iniziale verso l'alto, px/frame x100
    int16_t  gravita_x100;    // accelerazione di gravità, px/frame^2 x100 (15 = 0.15, come GRAVITY)
    uint16_t pausa;           // tick di sosta a ogni estremo
    uint8_t  sprite_size;     // PERCORSO_SPRITE_16 o PERCORSO_SPRITE_32
    int16_t  danno;
};

class pingpong : public obj
{
public:
    explicit pingpong(const pingpong_def& def,
                                       uint32_t sfasamento_ticks = 0);
    void update();

private:
    pingpong_def _def;
    uint32_t _tick_iniziale;
    uint32_t _sfasamento_ticks;
    int _durata_volo;       // tick per un volo completo
    int _altezza_raw;       // altezza massima, in unità raw di bn::fixed
    int _semigittata_raw;   // metà dello spostamento orizzontale, in unità raw

    void punto();
    void controlla_cane();
};