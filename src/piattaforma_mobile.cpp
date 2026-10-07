#include "piattaforma_mobile.h"
#include "globals.h"
#include "dog.h"
#include "enemy.h"
#include "bn_math.h"
#include "bn_sprite_items_fox1632.h"

static constexpr bn::fixed META_LARGHEZZA = 16;
static constexpr bn::fixed META_ALTEZZA   = 4;
static constexpr bn::fixed TOLLERANZA     = 6;

static bool attraversa(bn::fixed vecchio, bn::fixed nuovo, bn::fixed target, bool oraria)
{
    bn::fixed rel_target = oraria ? (target - vecchio) : (vecchio - target);
    bn::fixed rel_nuovo  = oraria ? (nuovo  - vecchio) : (vecchio - nuovo);

    while (rel_target < 0) rel_target += 360;
    while (rel_nuovo  < 0) rel_nuovo  += 360;

    return rel_target <= rel_nuovo;
}

piattaforma_mobile::piattaforma_mobile(const piattaforma_mobile_def& d) : def(d)
{
    _angolo = _angolo_iniziale = 0;
    calcola_posizione();
    _px = _x; _py = _y;

    sprite = bn::sprite_items::fox1632.create_sprite(_x - HALF_SCREEN_W, _y - HALF_SCREEN_H, 60);
    sprite->set_camera(g_camera);
    sprite->set_bg_priority(1);

    attiva = _attiva_iniziale = d.parte_attiva;
    if (attiva)
        _ticks_vita = d.durata_ticks;
}

void piattaforma_mobile::attiva_piattaforma()
{
    if (attiva || distrutta) return;
    attiva = true;
    _ticks_vita = def.durata_ticks;
    _px = _x; _py = _y;
}

void piattaforma_mobile::calcola_posizione()
{
    bn::fixed cos_a = bn::degrees_lut_cos_safe(_angolo);
    bn::fixed sin_a = bn::degrees_lut_sin_safe(_angolo);

    bn::fixed lx = bn::fixed(def.semiasse_x).multiplication(cos_a);
    bn::fixed ly = bn::fixed(def.semiasse_y).multiplication(sin_a);

    bn::fixed cos_r = bn::degrees_lut_cos_safe(bn::fixed(def.rotazione));
    bn::fixed sin_r = bn::degrees_lut_sin_safe(bn::fixed(def.rotazione));

    _x = bn::fixed(def.centro_x) + lx.multiplication(cos_r) - ly.multiplication(sin_r);
    _y = bn::fixed(def.centro_y) + lx.multiplication(sin_r) + ly.multiplication(cos_r);
}

void piattaforma_mobile::rinasci()
{
    distrutta = false;
    dog_agganciato = false;
    nemico_agganciato = nullptr;
    _ticks_dopo_salita_rimanenti = -1;

    _angolo = _angolo_iniziale;
    attiva = _attiva_iniziale;
    _ticks_vita = attiva ? def.durata_ticks : -1;

    calcola_posizione();
    _px = _x; _py = _y;

    sprite = bn::sprite_items::fox1632.create_sprite(_x - HALF_SCREEN_W, _y - HALF_SCREEN_H, 60);
    sprite->set_camera(g_camera);
    sprite->set_bg_priority(1);
}

void piattaforma_mobile::muovi()
{
    if (distrutta)
    {
        if (_ticks_respawn_rimanenti > 0)
        {
            _ticks_respawn_rimanenti--;
            if (_ticks_respawn_rimanenti == 0)
                rinasci();
        }
        return;
    }

    _px = _x; _py = _y;

    if (attiva)
    {
        if (_ticks_pausa_rimanenti > 0)
        {
            _ticks_pausa_rimanenti--;
        }
        else
        {
            bn::fixed passo = bn::fixed(def.velocita_gradi_x10).division(10);
            bn::fixed vecchio = _angolo;
            bn::fixed nuovo = _angolo + (def.oraria ? passo : -passo);
            while (nuovo >= 360) nuovo -= 360;
            while (nuovo < 0) nuovo += 360;

            if (def.pausa_angolo_0 > 0 && attraversa(vecchio, nuovo, 0, def.oraria))
            {
                _angolo = 0;
                _ticks_pausa_rimanenti = def.pausa_angolo_0;
            }
            else if (def.pausa_angolo_180 > 0 && attraversa(vecchio, nuovo, 180, def.oraria))
            {
                _angolo = 180;
                _ticks_pausa_rimanenti = def.pausa_angolo_180;
            }
            else
            {
                _angolo = nuovo;
            }
            calcola_posizione();
        }
    }

    sprite->set_x(_x - HALF_SCREEN_W);
    sprite->set_y(_y - HALF_SCREEN_H);

    if (attiva && def.durata_ticks >= 0)
    {
        if (_ticks_vita > 0) _ticks_vita--;

        static constexpr int TICKS_LAMPEGGIO = 60;
        if (_ticks_vita <= TICKS_LAMPEGGIO)
            sprite->set_visible((_ticks_vita / 4) % 2);

        if (_ticks_vita == 0)
        {
            distrutta = true;
            dog_agganciato = false;
            nemico_agganciato = nullptr;
            sprite.reset();
            return;
        }
    }

    if (_ticks_dopo_salita_rimanenti >= 0)
    {
        if (_ticks_dopo_salita_rimanenti > 0)
            _ticks_dopo_salita_rimanenti--;
        else
        {
            distrutta = true;
            dog_agganciato = false;
            nemico_agganciato = nullptr;
            sprite.reset();
            if (def.ticks_prima_di_tornare >= 0)
                _ticks_respawn_rimanenti = def.ticks_prima_di_tornare;
        }
    }
}

void piattaforma_mobile::trasporta_se_agganciato()
{
    if (distrutta) return;

    if (dog_agganciato)
    {
        g_dog->chr_x += delta_x();
        g_dog->chr_y += delta_y();
        g_dog->chr_vy = ZERO;
        g_dog->onGround = true;
    }
    if (nemico_agganciato && !nemico_agganciato->distrutto)
    {
        nemico_agganciato->chr_x += delta_x();
        nemico_agganciato->chr_y += delta_y();
        nemico_agganciato->chr_vy = ZERO;
        nemico_agganciato->onGround = true;
    }
}

bool piattaforma_mobile::continua_aggancio(live_obj& o)
{
    bool salta_via = o.chr_vy < 0;
    bool fuori_x = bn::abs(o.chr_x - _x) >= META_LARGHEZZA;
    if (salta_via || fuori_x) return false;

    o.chr_y = (_y - META_ALTEZZA) - o.box_dim;
    o.chr_vy = ZERO;
    o.onGround = true;
    return true;
}

bool piattaforma_mobile::prova_nuovo_aggancio(live_obj& o)
{
    bn::fixed piedi = o.chr_y + o.box_dim;
    bool sopra_in_x   = bn::abs(o.chr_x - _x) < META_LARGHEZZA;
    bool vicino_sopra = bn::abs(piedi - (_y - META_ALTEZZA)) < TOLLERANZA;
    bool non_salta_su = o.chr_vy >= 0;

    if (sopra_in_x && vicino_sopra && non_salta_su)
    {
        o.chr_y = (_y - META_ALTEZZA) - o.box_dim;
        o.chr_vy = ZERO;
        o.onGround = true;
        return true;
    }
    return false;
}

void piattaforma_mobile::verifica_nuovo_aggancio()
{
    if (distrutta) return;

    // --- cane ---
    if (def.tipo_collisione == PIATTAFORMA_MOBILE_SOLIDO)
    {
        bn::fixed dx = g_dog->chr_x - _x;
        bn::fixed dy = g_dog->chr_y - _y;
        bn::fixed overlap_x = (META_LARGHEZZA + g_dog->box_halfdim) - bn::abs(dx);
        bn::fixed overlap_y = (META_ALTEZZA + g_dog->box_dim) - bn::abs(dy);

        if (overlap_x <= 0 || overlap_y <= 0)
        {
            dog_agganciato = false;
        }
        else if (overlap_x < overlap_y)
        {
            g_dog->chr_x += (dx > 0) ? overlap_x : -overlap_x;
            g_dog->chr_vx = ZERO;
            dog_agganciato = false;
        }
        else if (dy < 0)
        {
            g_dog->chr_y = (_y - META_ALTEZZA) - g_dog->box_dim;
            g_dog->chr_vy = ZERO;
            g_dog->onGround = true;
            if (!dog_agganciato && def.ticks_dopo_salita >= 0 && _ticks_dopo_salita_rimanenti < 0)
                _ticks_dopo_salita_rimanenti = def.ticks_dopo_salita;
            dog_agganciato = true;
        }
        else
        {
            g_dog->chr_y = (_y + META_ALTEZZA) + g_dog->box_dim;
            if (g_dog->chr_vy < 0) g_dog->chr_vy = ZERO;
            dog_agganciato = false;
        }
    }
    else
    {
        bool era_agganciato = dog_agganciato;
        bool agganciato_ora = era_agganciato ? continua_aggancio(*g_dog) : prova_nuovo_aggancio(*g_dog);
        dog_agganciato = agganciato_ora;

        if (!era_agganciato && agganciato_ora && def.ticks_dopo_salita >= 0 && _ticks_dopo_salita_rimanenti < 0)
            _ticks_dopo_salita_rimanenti = def.ticks_dopo_salita;
    }

    // --- un nemico alla volta, solo appoggio dall'alto (indipendentemente da tipo_collisione) ---
    if (nemico_agganciato)
    {
        if (nemico_agganciato->distrutto || !continua_aggancio(*nemico_agganciato))
            nemico_agganciato = nullptr;
    }
    else
    {
        for (enemy* e : *g_enemies)
        {
            if (e->distrutto) continue;
            if (prova_nuovo_aggancio(*e))
            {
                nemico_agganciato = e;
                break;
            }
        }
    }
}