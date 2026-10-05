// include/s1_piattaforme_mobili.h
#pragma once
#include "piattaforma_mobile.h"

constexpr piattaforma_mobile_def schema1_piattaforme_mobili[] = {
    // centro(x,y) | semiassi(x,y) | rotazione | velocità°/tick x10 | oraria | parte_attiva | attiva_con | durata | tipo_collisione
    { 20, 400, 1, 80,   0, 2, true,  true,  0, -1, PIATTAFORMA_MOBILE_SOLIDO },   // orizzontale, attiva da subito, infinita
    { 900, 250, 0,  60,  0, 10, false, false, 2, 600, PIATTAFORMA_MOBILE_CLOUD }  // verticale, da attivare, si autodistrugge dopo 10s
};
constexpr int schema1_piattaforme_mobili_count =
    sizeof(schema1_piattaforme_mobili) / sizeof(schema1_piattaforme_mobili[0]);