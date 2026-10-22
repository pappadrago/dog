#pragma once

#include "bn_optional.h"
#include "bn_sprite_ptr.h"
#include "spingibile.h"

#define CASSA_SCATOLA  0
#define CASSA_INCUDINE 1
#define CASSA_MATTONI  2

// Definizione statica di una cassa posizionata in uno schema
struct cassa_def
{
    uint8_t tipo;       // CASSA_SCATOLA / CASSA_INCUDINE / CASSA_MATTONI
    int16_t x;          // posizione iniziale del centro (px, mondo): poi cade fino a terra
    int16_t y;
    bool    sparisce;   // true: dopo una vera caduta sparisce in ~3 secondi; false: resta (per impilarla)
};

class cassa : public spingibile
{
public:
    uint8_t tipo;
    bool sparisce = false;
    bool a_terra = false;   // è caduta e sta per sparire (solo se sparisce)
    bool distrutta = false;
    int  ticks_a_terra = 0;

    explicit cassa(const cassa_def& d);
    void update();
    const cassa_def* def_origine = nullptr;
protected:
    bool puo_essere_spinto() const override;
    bool ostacolato_da_oggetti() const override;

private:
    bool appoggia_dog();         // dog atterra sul bordo superiore
    void appoggia_su_casse();    // questa cassa atterra su un'altra
    bool colpisci_nemici();
    bool colpisci_boss();
};