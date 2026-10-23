#pragma once

#include "bn_fixed.h"
#include "obj.h"

// live_obj (gravità + mappa) che dog può spingere e il bau può lanciare.
class spingibile : public live_obj
{
public:
    virtual ~spingibile() = default;

    // bn::fixed meta_larghezza  = bn::fixed(8);      // mezza larghezza dello sprite
    bn::fixed velocita_spinta = bn::fixed(0.35);   // tetto a chr_vx di dog mentre spinge: più basso = più "pesante"
    bn::fixed fattore_bau     = bn::fixed(1);      // scala l'impulso del bau: 1 = come un nemico, < 1 = più pesante
    int ticks_immune_bau      = 0;                 // dopo un colpo, lo stesso bau non lo colpisce di nuovo

    // Impulso orizzontale + verticale; verso = DIR_LEFT / DIR_RIGHT, power = 0..1 (carica del bau)
    void colpito_da_bau(int verso, bn::fixed power);

protected:
    // Contatto laterale con dog: sposta l'oggetto o ferma dog. Prima di aggiorna_fisica().
    bool gestisci_spinta_dog();

    // Movimento orizzontale con attrito e muri, poi gravità. Una volta per frame.
    void aggiorna_fisica();

    // Hook per le classi derivate
    virtual bool puo_essere_spinto() const { return true; }        // false: muro per dog, immune al bau
    virtual bool ostacolato_da_oggetti() const { return false; }   // true: nella posizione attuale si sovrappone ad altro
};