#pragma once

#include "bn_fixed.h"
#include "obj.h"

// live_obj (gravità + mappa) che dog può spingere lateralmente.
// Il contatto dall'alto e il resto del comportamento restano alle classi derivate.
class spingibile : public live_obj
{
public:
    bn::fixed meta_larghezza  = bn::fixed(8);      // mezza larghezza dello sprite, per il contatto con dog
    bn::fixed velocita_spinta = bn::fixed(0.35);   // tetto a chr_vx di dog mentre spinge: più basso = più "pesante"

protected:
    // Se dog tocca l'oggetto di lato: sposta l'oggetto via da dog (rispettando la mappa) e rallenta dog;
    // se l'oggetto è contro un muro, è dog a venire fermato. Ritorna true se c'era contatto laterale.
    // Da chiamare prima di apply_gravity().
    bool gestisci_spinta_dog();
};