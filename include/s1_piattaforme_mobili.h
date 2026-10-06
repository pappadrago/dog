// include/s1_piattaforme_mobili.h
#pragma once
#include "piattaforma_mobile.h"

constexpr piattaforma_mobile_def schema1_piattaforme_mobili[] = {
    // int16_t  centro_x, centro_y;
    // int16_t  semiasse_x, semiasse_y;
    // int16_t  rotazione;
    // uint8_t  velocita_gradi_x10;
    // bool     oraria;
    // bool     parte_attiva;
    // uint8_t  attiva_con;
    // int16_t  durata_ticks;          // -1 = nessuna autodistruzione legata all'attivazione
    // uint8_t  tipo_collisione;
    // uint16_t pausa_angolo_0;        // tick di sosta a 0°; 0 = nessuna pausa
    // uint16_t pausa_angolo_180;      // tick di sosta a 180°; 0 = nessuna pausa
    // int16_t  ticks_dopo_salita;     // -1 = nessuna autodistruzione da salita di dog
    // int16_t  ticks_prima_di_tornare;// -1 = non ricompare mai, dopo l'autodistruzione da salita

{ 75,390, 0, 70, 20, 4, true, true, 0, -1, PIATTAFORMA_MOBILE_SOLIDO, 0, 0, -1, -1 },



};
constexpr int schema1_piattaforme_mobili_count =
sizeof(schema1_piattaforme_mobili) / sizeof(schema1_piattaforme_mobili[0]);