#include "globals.h"
#include "dog.h"
#include "boss.h"
#include "enemy.h"
#include "collision.h"
#include "bn_math.h"
#include "bn_sprite_items_items.h"
#include "cassa.h"

static constexpr int TICKS_PRIMA_DI_SPARIRE = 180;   // ~3 secondi a 60fps
static constexpr bn::fixed SOGLIA_CADUTA = bn::fixed(1.0);   // sotto questa velocità non è una "caduta": né colpo né conto alla rovescia

// per tipo: SCATOLA, INCUDINE, MATTONI
static constexpr int DANNO_PER_TIPO[3] = { 15, 40, 25 };
static constexpr bn::fixed SPINTA_PER_TIPO[3] = { bn::fixed(0.6), bn::fixed(0.2), bn::fixed(0.35) };   // più pesante = più lenta da spingere

cassa::cassa(const cassa_def& d)
{
    tipo = (d.tipo < 3) ? d.tipo : CASSA_SCATOLA;   // tipo fuori range: scatola
    chr_x = bn::fixed(d.x);
    chr_y = bn::fixed(d.y);
    box_dim = bn::fixed(12);
    box_halfdim = bn::fixed(6);

    meta_larghezza = box_halfdim;
    velocita_spinta = SPINTA_PER_TIPO[tipo];

    sprite = bn::sprite_items::items.create_sprite(
        chr_x - HALF_SCREEN_W, chr_y - HALF_SCREEN_H, tipo);
    sprite->set_camera(g_camera);
    sprite->set_bg_priority(1);
}

void cassa::update()
{
    if (distrutta) return;

    if (a_terra)
    {
        if (--ticks_a_terra <= 0)
        {
            distrutta = true;
            sprite->set_visible(false);
            return;
        }
        sprite->set_visible(ticks_a_terra >= 60 || ticks_a_terra % 2);   // lampeggia poco prima di sparire
    }

    // la cassa si può spingere anche da ferma e anche dopo essere caduta
    gestisci_spinta_dog();
    chr_vx = ZERO;
    apply_gravity();

    // colpisce solo mentre sta cadendo attivamente, non se ferma/appoggiata
    if (!onGround && chr_vy > SOGLIA_CADUTA)
    {
        if (colpisci_nemici() || colpisci_boss())
        {
            distrutta = true;       // ogni cassa = un colpo
            sprite->set_visible(false);
            return;
        }
    }

    // il conto alla rovescia parte solo dopo una vera caduta, non appena la cassa si appoggia
    if (!a_terra && atterrato_ora && velocita_atterraggio > SOGLIA_CADUTA)
    {
        a_terra = true;
        ticks_a_terra = TICKS_PRIMA_DI_SPARIRE;
    }

    sprite->set_x(chr_x - HALF_SCREEN_W);
    sprite->set_y(chr_y - HALF_SCREEN_H);
}

// Il fondo della cassa attraversa la testa del nemico in questo frame
bool cassa::colpisci_nemici()
{
    bn::fixed fondo       = chr_y + box_dim;
    bn::fixed fondo_prima = fondo - chr_vy;   // chr_y è già stato aggiornato da apply_gravity()

    for (enemy* e : *g_enemies)
    {
        if (e->distrutto || e->is_dead() || e->invulnerability > 0) continue;

        bn::fixed testa = e->chr_y - e->box_dim;
        bool sopra_in_x = bn::abs(e->chr_x - chr_x) < meta_larghezza + e->box_halfdim;

        if (sopra_in_x && fondo_prima <= testa + 1 && fondo >= testa)
        {
            e->beHitByCassa(DANNO_PER_TIPO[tipo]);
            return true;
        }
    }
    return false;
}

bool cassa::colpisci_boss()
{
    if (g_boss.has_value() && check_collision_boss(*this))
    {
        g_boss->subisci_colpo();
        return true;
    }
    return false;
}