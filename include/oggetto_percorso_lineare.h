#pragma once

#include "oggetto_percorso.h"

enum percorso_lineare_direzione : uint8_t
{
    PERCORSO_LINEARE_ORIZZONTALE = 0,
    PERCORSO_LINEARE_VERTICALE
};

struct oggetto_percorso_lineare_def
{
    int16_t centro_x, centro_y;
    int16_t escursione;
    uint8_t direzione;
    uint8_t durata_tratto, pausa_tratto;
    uint8_t sprite_size;    // PERCORSO_SPRITE_16 o PERCORSO_SPRITE_32
    int16_t danno;
};

class oggetto_percorso_lineare : public obj
{
public:
    explicit oggetto_percorso_lineare(const oggetto_percorso_lineare_def& def,
                                      uint32_t sfasamento_ticks = 0);
    void sposta_centro(bn::fixed offset_x, bn::fixed offset_y);
    void update();

private:
    oggetto_percorso_lineare_def _def;
    bn::optional<bn::sprite_animate_action<16>> _animazione;
    uint32_t _tick_iniziale;
    uint32_t _sfasamento_ticks;
    bn::fixed _centro_offset_x = ZERO;
    bn::fixed _centro_offset_y = ZERO;

    void punto();
};
