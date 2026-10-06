#pragma once
#include "piattaforma_mobile.h"

constexpr piattaforma_mobile_def schema2_piattaforme_mobili[] = {
    // centro(x,y) | semiassi(x,y) | rotazione | velocità°/tick x10 | oraria | parte_attiva | attiva_con | durata | tipo_collisione
    { 500, 300, 0, 60, 0, 8, true, true, 0, -1, PIATTAFORMA_MOBILE_CLOUD },
    { 1100, 300, 80, 0, 0, 10, false, true, 0, -1, PIATTAFORMA_MOBILE_SOLIDO }
};
constexpr int schema2_piattaforme_mobili_count =
    sizeof(schema2_piattaforme_mobili) / sizeof(schema2_piattaforme_mobili[0]);
