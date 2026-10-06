#pragma once
#include "bn_optional.h"
#include "bn_sprite_ptr.h"
#include "bn_fixed.h"
#include "obj.h"
#include <cstdint>

#define PIATTAFORMA_MOBILE_CLOUD  0
#define PIATTAFORMA_MOBILE_SOLIDO 1

struct piattaforma_mobile_def
{
    int16_t  centro_x, centro_y;
    int16_t  semiasse_x, semiasse_y;
    int16_t  rotazione;
    uint8_t  velocita_gradi_x10;
    bool     oraria;
    bool     parte_attiva;
    uint8_t  attiva_con;
    int16_t  durata_ticks;          // -1 = nessuna autodistruzione legata all'attivazione
    uint8_t  tipo_collisione;
    uint16_t pausa_angolo_0;        // tick di sosta a 0°; 0 = nessuna pausa
    uint16_t pausa_angolo_180;      // tick di sosta a 180°; 0 = nessuna pausa
    int16_t  ticks_dopo_salita;     // -1 = nessuna autodistruzione da salita di dog
    int16_t  ticks_prima_di_tornare;// -1 = non ricompare mai, dopo l'autodistruzione da salita
};

class piattaforma_mobile
{
public:
    piattaforma_mobile_def def;
    bool attiva = false;
    bool distrutta = false;
    bool dog_agganciato = false;
    class enemy* nemico_agganciato = nullptr;

    explicit piattaforma_mobile(const piattaforma_mobile_def& d);

    void muovi();
    void trasporta_se_agganciato();
    void verifica_nuovo_aggancio();
    void attiva_piattaforma();

private:
    bn::optional<bn::sprite_ptr> sprite;

    bn::fixed _angolo = 0;
    bn::fixed _angolo_iniziale = 0;
    bn::fixed _x, _y;
    bn::fixed _px, _py;

    int _ticks_vita = -1;
    int _ticks_pausa_rimanenti = 0;
    int _ticks_dopo_salita_rimanenti = -1;
    int _ticks_respawn_rimanenti = -1;
    bool _attiva_iniziale = false;

    void calcola_posizione();
    void rinasci();
    bn::fixed delta_x() const { return _x - _px; }
    bn::fixed delta_y() const { return _y - _py; }

    bool continua_aggancio(live_obj& o);
    bool prova_nuovo_aggancio(live_obj& o);
};