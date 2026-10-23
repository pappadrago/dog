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
static constexpr u_int8_t DANNO_PER_TIPO[3] = { 15, 40, 25 };
static constexpr bn::fixed SPINTA_PER_TIPO[3] = { bn::fixed(0.6), bn::fixed(0.2), bn::fixed(0.35) };   // più pesante = più lenta da spingere
static constexpr bn::fixed FATTORE_BAU_PER_TIPO[3] = { bn::fixed(1.0), bn::fixed(0.35), bn::fixed(0.6) };
static constexpr u_int8_t TILE_PER_TIPO[3] = { 18, 18, 18 };

static bool sovrapposte_in_x(const cassa& a, const cassa& b)
{
    return bn::abs(a.chr_x - b.chr_x) < a.box_halfdim + b.box_halfdim;
}

cassa::cassa(const cassa_def& d)
{
    tipo = (d.tipo < 3) ? d.tipo : CASSA_SCATOLA;   // tipo fuori range: scatola
    sparisce = d.sparisce;
    chr_x = bn::fixed(d.x);
    chr_y = bn::fixed(d.y);
    box_dim = bn::fixed(8);
    box_halfdim = bn::fixed(7);

    // meta_larghezza = box_halfdim;
    velocita_spinta = SPINTA_PER_TIPO[tipo];
    fattore_bau = FATTORE_BAU_PER_TIPO[tipo];

    sprite = bn::sprite_items::items.create_sprite(
        chr_x - HALF_SCREEN_W, chr_y - HALF_SCREEN_H, TILE_PER_TIPO[tipo]);
    sprite->set_camera(g_camera);
    sprite->set_bg_priority(1);
}

// Con una cassa appoggiata sopra, la cassa è troppo pesante per essere spinta
bool cassa::puo_essere_spinto() const
{
    if (!g_casse.has_value())
        return true;

    bn::fixed mio_top = chr_y - box_dim;
    for (const cassa* c : *g_casse)
    {
        if (c == this || c->distrutta) continue;
        bn::fixed fondo_c = c->chr_y + c->box_dim;
        if (sovrapposte_in_x(*this, *c) && bn::abs(fondo_c - mio_top) < 2)
            return false;
    }
    return true;
}

// Chiamata dopo lo spostamento laterale tentato: true se ora si sovrappone a un'altra cassa
bool cassa::ostacolato_da_oggetti() const
{
    if (!g_casse.has_value())
        return false;

    for (const cassa* c : *g_casse)
    {
        if (c == this || c->distrutta) continue;
        // margine di 2px in verticale: una cassa appoggiata sopra/sotto non conta come ostacolo laterale
        if (sovrapposte_in_x(*this, *c) && bn::abs(c->chr_y - chr_y) < box_dim + c->box_dim - 2)
            return true;
    }
    return false;
}

// Come l'atterraggio sulla mappa: dog resta sul bordo e riceve gli stessi effetti di un atterraggio normale
bool cassa::appoggia_dog()
{
    dog& d = *g_dog;

    bn::fixed top         = chr_y - box_dim;
    bn::fixed piedi       = d.chr_y + d.box_dim;
    bn::fixed piedi_prima = piedi - d.chr_vy;   // dog::update() ha già mosso chr_y in questo frame

    bool sopra_in_x = bn::abs(d.chr_x - chr_x) < box_halfdim + d.box_halfdim;
    if (!(d.chr_vy > 0 && sopra_in_x && piedi_prima <= top + 1 && piedi >= top))
        return false;

    // dog::update() leggerà questi flag nel prossimo frame, come per un atterraggio sulla mappa
    d.velocita_atterraggio = d.chr_vy;
    d.atterrato_ora = true;
    d.salto_in_corso = false;

    d.chr_y = top - d.box_dim;
    d.chr_vy = ZERO;
    d.onGround = true;
    d.sprite->set_y(d.chr_y - HALF_SCREEN_H);
    return true;
}

// Dopo apply_gravity(): se il fondo ha attraversato il bordo superiore di un'altra cassa, si appoggia
void cassa::appoggia_su_casse()
{
    if (chr_vy <= 0 || !g_casse.has_value())
        return;

    bn::fixed fondo       = chr_y + box_dim;
    bn::fixed fondo_prima = fondo - chr_vy;   // chr_y è già stato aggiornato da apply_gravity()

    for (cassa* c : *g_casse)
    {
        if (c == this || c->distrutta) continue;

        bn::fixed top_c = c->chr_y - c->box_dim;
        if (sovrapposte_in_x(*this, *c) && fondo_prima <= top_c + 1 && fondo >= top_c)
        {
            velocita_atterraggio = chr_vy;
            atterrato_ora = true;
            chr_y = top_c - box_dim;
            chr_vy = ZERO;
            onGround = true;
            return;
        }
    }
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

    // dog che atterra sopra ha la precedenza sulla spinta laterale
    if (!appoggia_dog())
        gestisci_spinta_dog();

    aggiorna_fisica();
    appoggia_su_casse();

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
    if (sparisce && !a_terra && atterrato_ora && velocita_atterraggio > SOGLIA_CADUTA)
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
    bn::fixed fondo_prima = fondo - chr_vy;

    for (enemy* e : *g_enemies)
    {
        if (e->distrutto || e->is_dead() || e->invulnerability > 0) continue;

        bn::fixed testa = e->chr_y - e->box_dim;
        bool sopra_in_x = bn::abs(e->chr_x - chr_x) < box_halfdim + e->box_halfdim;

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