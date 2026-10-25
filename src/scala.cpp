#include "scala.h"
#include "bn_math.h"
#include "constanti_fisica.h"

static constexpr bn::fixed TOLLERANZA_CIMA = 6;

bool scala::in_colonna(const live_obj& o) const
{
    return bn::abs(o.chr_x - x_centro()) < SCALA_MEZZA_LARGHEZZA;
}

void scala::rilascia(live_obj& o, int dir_uscita)
{
    o.scala_corrente = nullptr;
    o.scala_bloccata = this;
    o.scala_blocco_dir = dir_uscita;   // finché tieni premuta questa direzione, non si riaggancia
}

bool scala::prova_aggancio(live_obj& o)
{
    if (o.scala_bloccata == this)
    {
        if (o.intent_scala == o.scala_blocco_dir && in_colonna(o))
            return false;            // ancora premuto verso l'uscita: niente riaggancio
        o.scala_bloccata = nullptr;  // tasto lasciato o spostato fuori colonna: sblocca
    }

    if (!o.puo_scalare || o.intent_scala == 0 || !in_colonna(o)) return false;

    bn::fixed piedi = o.chr_y + o.box_dim;

    // Salire: i piedi devono stare dentro la scala (o appena sotto il fondo)
    bool sale_ok  = o.intent_scala < 0 && piedi > y_cima() && piedi <= y_fondo() + TOLLERANZA_CIMA;
    // Scendere: solo dalla cima di una scala con piano, o da dentro la scala
    bool scende_ok = o.intent_scala > 0 &&
        ((def.tipo == SCALA_CON_PIANO && bn::abs(piedi - y_cima()) < TOLLERANZA_CIMA) ||
         (piedi > y_cima() && piedi < y_fondo()));

    if (!sale_ok && !scende_ok) return false;

    o.scala_corrente = this;
    o.chr_x = x_centro();          // si centra sulla scala
    o.chr_vx = o.chr_vy = ZERO;
    o.onGround = false;
    if (scende_ok && bn::abs(piedi - y_cima()) < TOLLERANZA_CIMA)
        o.chr_y = y_cima() - o.box_dim + 2;   // entra nella scala dalla cima
    return true;
}

void scala::muovi(live_obj& o)
{
    o.chr_vx = ZERO;
    o.chr_vy = ZERO;
    o.chr_y += SCALA_VELOCITA.multiplication(bn::fixed(o.intent_scala));

    bn::fixed piedi = o.chr_y + o.box_dim;

    if (piedi <= y_cima())                       // cima raggiunta: sgancio automatico
    {
        if (def.tipo == SCALA_CON_PIANO)
        {
            o.chr_y = y_cima() - o.box_dim;      // in piedi sul piano
            o.onGround = true;
        }
        else
        {
            o.chr_y = y_cima() - o.box_dim - 1;  // scala senza piano: piccolo scatto fuori
            o.chr_vy = MEDIUM_JUMP_VY;
        }
        rilascia(o, -1);
    }
    else if (piedi >= y_fondo())                 // fondo raggiunto: sgancio automatico
    {
        o.chr_y = y_fondo() - o.box_dim;
        o.onGround = true;
        rilascia(o, +1);
    }
}

bool scala::appoggia_in_cima(live_obj& o)
{
    if (def.tipo != SCALA_CON_PIANO || o.su_scala()) return false;
    if (o.chr_vy < 0 || o.intent_scala > 0) return false;   // salta su o vuole scendere: non regge

    // il piano copre la colonna, un po' più larga della sola scala
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