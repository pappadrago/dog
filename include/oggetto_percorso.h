#pragma once

#include "obj.h"
#include <cstdint>

enum percorso_forma : uint8_t
{
    PERCORSO_ELLISSE = 0,
    PERCORSO_RETTANGOLO,
    PERCORSO_OTTO
};

enum percorso_sprite_size : uint8_t
{
    PERCORSO_SPRITE_16 = 16,
    PERCORSO_SPRITE_32 = 32
};

struct oggetto_percorso_def
{
    int16_t centro_x, centro_y;
    int16_t semiasse_x, semiasse_y;
    uint8_t forma;
    uint8_t velocita_x10;   // px/frame x10
    bool oraria;
    int16_t fase_iniziale;  // gradi
    uint8_t sprite_size;    // PERCORSO_SPRITE_16 o PERCORSO_SPRITE_32
    int16_t danno;
};

class oggetto_percorso : public obj
{
public:
    explicit oggetto_percorso(const oggetto_percorso_def& def);
    void update();

private:
    oggetto_percorso_def _def;
    bn::optional<bn::sprite_animate_action<16>> _animazione;
    bn::fixed _perimetro;
    bn::fixed _delta;
    int _segno = 1;

    bn::fixed wrap360(bn::fixed fase) const;
    bn::fixed fase_corrente() const;
    void punto(bn::fixed fase);
    void controlla_cane();
};
