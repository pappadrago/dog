#pragma once
#include "bn_optional.h"
#include "bn_sprite_ptr.h"
#include "bn_fixed.h"
#include <cstdint>

#define PIATTAFORMA_MOBILE_CLOUD  0   // solo appoggio dall'alto, trasparente dalle altre direzioni (comportamento attuale)
#define PIATTAFORMA_MOBILE_SOLIDO 1   // blocco pieno su tutti e 4 i lati

struct piattaforma_mobile_def
{
    int16_t  centro_x, centro_y;
    int16_t  semiasse_x, semiasse_y;
    int16_t  rotazione;
    uint8_t  velocita_gradi_x10;
    bool     oraria;
    bool     parte_attiva;
    uint8_t  attiva_con;
    int16_t  durata_ticks;
    uint8_t  tipo_collisione;   // PIATTAFORMA_MOBILE_CLOUD o _SOLIDO
};

class piattaforma_mobile
{
public:
    piattaforma_mobile_def def;
    bool attiva = false;
    bool distrutta = false;

    explicit piattaforma_mobile(const piattaforma_mobile_def& d);
    void update();
    void attiva_piattaforma();

private:
    bn::optional<bn::sprite_ptr> sprite;
    bn::fixed _angolo = 0;
    bn::fixed _x, _y;     // posizione mondo attuale (centro dello sprite)
    bn::fixed _px, _py;   // posizione del frame precedente, per calcolare lo spostamento
    int _ticks_vita = -1;

private:
    void calcola_posizione();
    void collisione_cloud();     // il comportamento che avevi già: solo appoggio dall'alto
    void collisione_solida();    // nuovo: blocco sui 4 lati
};