#pragma once

#include "bn_optional.h"
#include "bn_sprite_ptr.h"
#include "spingibile.h"

#define CASSA_SCATOLA  0
#define CASSA_INCUDINE 1
#define CASSA_MATTONI  2

// Definizione statica di una cassa posizionata in uno schema (come item_def, enemy_def...)
struct cassa_def
{
    uint8_t tipo;   // CASSA_SCATOLA / CASSA_INCUDINE / CASSA_MATTONI
    int16_t x;      // posizione iniziale del centro (px, mondo): poi cade fino a terra
    int16_t y;
};

class cassa : public spingibile
{
public:
    uint8_t tipo;
    bool a_terra       = false;   // è caduta e sta per sparire
    bool distrutta     = false;
    int  ticks_a_terra = 0;
    const cassa_def* def_origine = nullptr;
    explicit cassa(const cassa_def& d);
    void update();

private:
    bool colpisci_nemici();
    bool colpisci_boss();
};