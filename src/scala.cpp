#include "scala.h"
#include "globals.h"
#include "bn_math.h"
#include "constanti_fisica.h"

static constexpr bn::fixed TOLLERANZA_CIMA = 6;
static constexpr bn::fixed SCALA_SALTO_INIZIO = bn::fixed(0.3);

bool scala::in_colonna(const live_obj& o) const
{
    return bn::abs(o.chr_x - x_centro()) < SCALA_MEZZA_LARGHEZZA;
}

void scala::rilascia(live_obj& o, int dir_uscita)
{
    o.scala_corrente = nullptr;
    o.scala_bloccata = this;
    o.scala_blocco_dir = dir_uscita;
    o.scala_vy = ZERO;
}

bool scala::prova_aggancio(live_obj& o)
{
    if (o.scala_bloccata == this)
    {
        if (o.intent_scala == o.scala_blocco_dir && in_colonna(o))
            return false;
        o.scala_bloccata = nullptr;
    }

    if (!o.puo_scalare || o.intent_scala == 0 || !in_colonna(o)) return false;

    bn::fixed piedi = o.chr_y + o.box_dim;

    bool sale_ok = o.intent_scala < 0 && piedi > y_cima() && piedi <= y_fondo() + TOLLERANZA_CIMA;
    bool scende_ok = o.intent_scala > 0 &&
        ((def.tipo == SCALA_CON_PIANO && bn::abs(piedi - y_cima()) < TOLLERANZA_CIMA) ||
         (piedi > y_cima() && piedi < y_fondo()));

    if (!sale_ok && !scende_ok) return false;

    o.scala_corrente = this;
    o.chr_x = x_centro();
    o.chr_vx = o.chr_vy = ZERO;
    o.scala_vy = ZERO;
    o.onGround = false;
    if (scende_ok && bn::abs(piedi - y_cima()) < TOLLERANZA_CIMA)
        o.chr_y = y_cima() - o.box_dim + 2;
    return true;
}

void scala::muovi(live_obj& o)
{
    o.chr_vx = ZERO;

    bn::fixed movimento = bn::fixed(o.intent_scala);
    if (o.intent_scala != 0)
    {
        if (o.scala_vy == ZERO)
            o.scala_vy = movimento * SCALA_SALTO_INIZIO;
        else if (o.scala_vy * movimento < ZERO)
            o.scala_vy = o.scala_vy * bn::fixed(0.75) + movimento * SCALA_SALTO_INIZIO;
        else
            o.scala_vy += movimento * SCALA_ACCELERAZIONE;

        if (o.scala_vy > SCALA_VELOCITA)
            o.scala_vy = SCALA_VELOCITA;
        else if (o.scala_vy < -SCALA_VELOCITA)
            o.scala_vy = -SCALA_VELOCITA;
    }
    else
    {
        if (o.scala_vy > ZERO)
            o.scala_vy -= SCALA_FRENATA;
        else if (o.scala_vy < ZERO)
            o.scala_vy += SCALA_FRENATA;

        if (bn::abs(o.scala_vy) < SCALA_FRENATA)
            o.scala_vy = ZERO;
    }

    o.chr_y += o.scala_vy;
    o.chr_vy = ZERO;

    bn::fixed piedi = o.chr_y + o.box_dim;

    if (piedi <= y_cima())
    {
        if (def.tipo == SCALA_CON_PIANO)
        {
            o.chr_y = y_cima() - o.box_dim;
            o.onGround = true;
        }
        else
        {
            o.chr_y = y_cima() - o.box_dim - 1;
            o.chr_vy = MEDIUM_JUMP_VY;
        }
        rilascia(o, -1);
    }
    else if (piedi >= y_fondo())
    {
        o.chr_y = y_fondo() - o.box_dim;
        o.onGround = true;
        rilascia(o, +1);
    }
}

bool scala::appoggia_in_cima(live_obj& o)
{
    if (def.tipo != SCALA_CON_PIANO || o.su_scala()) return false;
    if (o.chr_vy < 0 || o.intent_scala > 0) return false;

    if (bn::abs(o.chr_x - x_centro()) >= 16) return false;

    bn::fixed piedi = o.chr_y + o.box_dim;
    if (bn::abs(piedi - y_cima()) >= TOLLERANZA_CIMA) return false;

    o.chr_y = y_cima() - o.box_dim;
    o.chr_vy = ZERO;
    o.onGround = true;
    return true;
}

void scala::aggiorna(live_obj& o)
{
    if (o.scala_corrente == this)
        muovi(o);
    else if (!o.scala_corrente && !prova_aggancio(o))
        appoggia_in_cima(o);
}
