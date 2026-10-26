#include "globals.h"
#include "spingibile.h"
#include "dog.h"
#include <bn_log.h>
#include "constanti_fisica.h"
#include "bn_math.h"
#include <bn_sound_items.h>

void spingibile::colpito_da_bau(int verso, bn::fixed power)
{
    if (!puo_essere_spinto())
        return;   // con un carico sopra non si muove
    // stessa formula di enemy::beHitByBark(), scalata dal peso
    chr_vy = ENEMY_SMALL_JUMP_VY - power.multiplication(fattore_bau);
    chr_vx = (0.5+power).multiplication(verso);
    onGround = false;
}

void spingibile::aggiorna_fisica()
{
    if (ticks_immune_bau > 0)
        ticks_immune_bau--;

    if (chr_vx != ZERO)
    {
        bn::fixed x_prima = chr_x;

        chr_x += chr_vx;
        apply_map();   // muro: annulla lo spostamento e azzera chr_vx

        if (chr_vx != ZERO && ostacolato_da_oggetti())
        {
            chr_x = x_prima;   // sbatte contro un'altra cassa
            chr_vx = ZERO;
        }
        apply_friction();
    }

    apply_gravity();
}

bool spingibile::gestisci_spinta_dog()
{
    bn::fixed top   = chr_y - box_dim;
    bn::fixed piedi = g_dog->chr_y + g_dog->box_dim;
    bn::fixed testa = g_dog->chr_y - g_dog->box_dim;
    bn::fixed dx    = g_dog->chr_x - chr_x;

    bn::fixed contatto_x = box_halfdim + g_dog->box_halfdim;

    // contatto laterale: sovrapposti in x, con dog sotto il bordo superiore e sopra il fondo
    if (bn::abs(dx) >= contatto_x || piedi <= top + 1 || testa >= chr_y + box_dim)
        return false;

    bn::fixed penetrazione = contatto_x - bn::abs(dx);
    int verso = (dx > 0) ? DIR_LEFT : DIR_RIGHT;   // l'oggetto si sposta via da dog

    bool bloccato = !puo_essere_spinto();          // troppo "carico": per dog è un muro

    if (!bloccato)
    {
        bn::fixed x_prima    = chr_x;
        bn::fixed vx_salvata = chr_vx;   // la velocità data dal bau non va persa

        // apply_map() guarda il lato in base al segno di chr_vx e, se trova un muro,
        // annulla esattamente questo spostamento e azzera chr_vx
        chr_vx = penetrazione * verso;
        chr_x += chr_vx;
        apply_map();
        bloccato = (chr_vx == ZERO);

        if (!bloccato && ostacolato_da_oggetti())
        {
            chr_x = x_prima;   // sbatterebbe contro un altro oggetto
            bloccato = true;
        }
        chr_vx = vx_salvata;
    }

    if (bloccato)
    {
        // l'oggetto non si muove: è dog a non poter avanzare
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