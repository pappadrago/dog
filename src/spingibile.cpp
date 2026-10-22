#include "globals.h"
#include "spingibile.h"
#include "dog.h"
#include "bn_math.h"

bool spingibile::gestisci_spinta_dog()
{
    bn::fixed top   = chr_y - box_dim;
    bn::fixed piedi = g_dog->chr_y + g_dog->box_dim;
    bn::fixed testa = g_dog->chr_y - g_dog->box_dim;
    bn::fixed dx    = g_dog->chr_x - chr_x;

    bn::fixed contatto_x = meta_larghezza + g_dog->box_halfdim;

    // contatto laterale: sovrapposti in x, con dog sotto il bordo superiore e sopra il fondo
    if (bn::abs(dx) >= contatto_x || piedi <= top + 1 || testa >= chr_y + box_dim)
        return false;

    bn::fixed penetrazione = contatto_x - bn::abs(dx);
    int verso = (dx > 0) ? DIR_LEFT : DIR_RIGHT;   // l'oggetto si sposta via da dog

    // apply_map() guarda il lato in base al segno di chr_vx e, se trova un muro,
    // annulla esattamente questo spostamento e azzera chr_vx
    chr_vx = penetrazione * verso;
    chr_x += chr_vx;
    apply_map();
    bool bloccato = (chr_vx == ZERO);
    chr_vx = ZERO;

    if (bloccato)
    {
        // contro un muro: è dog a non poter avanzare
        g_dog->chr_x = chr_x - contatto_x * verso;
        g_dog->chr_vx = ZERO;
    }
    else
    {
        // dal prossimo frame dog::update() limita chr_vx prima di muoversi
        g_dog->tetto_vx_spinta = velocita_spinta;
        g_dog->spinta_ticks = 1;
    }

    return true;
}