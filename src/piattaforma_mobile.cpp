#include "piattaforma_mobile.h"
#include "globals.h"
#include "dog.h"
#include "bn_math.h"
#include "bn_sprite_items_fox1632.h"

piattaforma_mobile::piattaforma_mobile(const piattaforma_mobile_def& d) : def(d)
{
    calcola_posizione();
    _px = _x; _py = _y;

    sprite = bn::sprite_items::fox1632.create_sprite(_x - HALF_SCREEN_W, _y - HALF_SCREEN_H, 60);
    sprite->set_camera(g_camera);
    sprite->set_bg_priority(1);

    attiva = d.parte_attiva;
    if (attiva)
        _ticks_vita = d.durata_ticks;
}

void piattaforma_mobile::attiva_piattaforma()
{
    if (attiva || distrutta) return;
    attiva = true;
    _ticks_vita = def.durata_ticks;
    _px = _x; _py = _y;   // evita un salto fittizio di posizione al primo frame di movimento
}

void piattaforma_mobile::calcola_posizione()
{
    bn::fixed cos_a = bn::degrees_lut_cos_safe(_angolo);
    bn::fixed sin_a = bn::degrees_lut_sin_safe(_angolo);

    bn::fixed lx = bn::fixed(def.semiasse_x) * cos_a;
    bn::fixed ly = bn::fixed(def.semiasse_y) * sin_a;

    bn::fixed cos_r = bn::degrees_lut_cos_safe(bn::fixed(def.rotazione));
    bn::fixed sin_r = bn::degrees_lut_sin_safe(bn::fixed(def.rotazione));

    _x = bn::fixed(def.centro_x) + lx.multiplication(cos_r) - ly.multiplication(sin_r);
    _y = bn::fixed(def.centro_y) + lx.multiplication(sin_r) + ly.multiplication(cos_r);
}


void piattaforma_mobile::update()
{
    if (distrutta) return;

    if (attiva)
    {
        _px = _x; _py = _y;

        bn::fixed passo = bn::fixed(def.velocita_gradi_x10).division(bn::fixed(10));
        _angolo += def.oraria ? passo : -passo;
        if (_angolo >= 360) _angolo -= 360;
        if (_angolo < 0) _angolo += 360;

        calcola_posizione();
    }

    sprite->set_x(_x - HALF_SCREEN_W);
    sprite->set_y(_y - HALF_SCREEN_H);

    if (def.tipo_collisione == PIATTAFORMA_MOBILE_SOLIDO)
        collisione_solida();
    else
        collisione_cloud();


    if (attiva && def.durata_ticks >= 0)
    {
        if (_ticks_vita > 0) _ticks_vita--;

        static constexpr int TICKS_LAMPEGGIO = 60;   // ultimo secondo: lampeggia
        if (_ticks_vita <= TICKS_LAMPEGGIO)
            sprite->set_visible((_ticks_vita / 4) % 2);

        if (_ticks_vita == 0)
        {
            distrutta = true;
            sprite.reset();
        }
    }
}

void piattaforma_mobile::collisione_cloud()
{
    static constexpr bn::fixed META_LARGHEZZA = 16;
    static constexpr bn::fixed META_ALTEZZA = 8;
    static constexpr bn::fixed TOLLERANZA = 4;

    bn::fixed dog_feet = g_dog->chr_y + g_dog->box_dim;
    bn::fixed superficie_precedente = _py - META_ALTEZZA;

    bool sopra_in_x = bn::abs(g_dog->chr_x - _x) < META_LARGHEZZA;
    bool vicino_sopra = (dog_feet > superficie_precedente - TOLLERANZA) &&
        (dog_feet < superficie_precedente + TOLLERANZA);
    bool non_salta_su = g_dog->chr_vy >= 0;

    if (sopra_in_x && vicino_sopra && non_salta_su)
    {
        g_dog->chr_x += (_x - _px);
        g_dog->chr_y = (_y - META_ALTEZZA) - g_dog->box_dim;
        g_dog->chr_vy = 0;
        g_dog->onGround = true;
    }
}

void piattaforma_mobile::collisione_solida()
{
    static constexpr bn::fixed META_LARGHEZZA = 16;
    static constexpr bn::fixed META_ALTEZZA = 4;

    bn::fixed dx = g_dog->chr_x - _x;
    bn::fixed dy = g_dog->chr_y - _y;

    bn::fixed overlap_x = (META_LARGHEZZA + g_dog->box_halfdim) - bn::abs(dx);
    bn::fixed overlap_y = (META_ALTEZZA + g_dog->box_dim) - bn::abs(dy);

    if (overlap_x <= 0 || overlap_y <= 0)
        return;   // nessuna sovrapposizione reale

    if (overlap_x < overlap_y)
    {
        // risolvi lungo X: blocco laterale, come un muro
        g_dog->chr_x += (dx > 0) ? overlap_x : -overlap_x;
        g_dog->chr_vx = 0;
    }
    else
    {
        if (dy < 0)
        {
            // il cane è sopra: appoggio, come la nuvola, ma qui è garantito (nessuna finestra di tolleranza)
            g_dog->chr_y = (_y - META_ALTEZZA) - g_dog->box_dim;
            g_dog->chr_vy = 0;
            g_dog->onGround = true;
            g_dog->chr_x += (_x - _px);
        }
        else
        {
            // il cane è sotto: bloccato dal basso, come un soffitto
            g_dog->chr_y = (_y + META_ALTEZZA) + g_dog->box_dim;
            if (g_dog->chr_vy < 0) g_dog->chr_vy = 0;
        }
    }
}