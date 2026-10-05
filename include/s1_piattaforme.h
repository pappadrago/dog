// include/s1_piattaforme.h
#pragma once
#include "piattaforma.h"
#include "s1_items.h"

constexpr piattaforma_def schema1_piattaforme[] = {
    // tile 16px (x1,y1,x2,y2) | hw solido[4] | hw vuoto[4] | stato iniziale | trigger
    { 48, 27, 57, 27, {6,7,16,17}, {4, 5, 14, 15}, 0, ID_PIATTAFORMA_S1_1 },
};
constexpr int schema1_piattaforme_count = sizeof(schema1_piattaforme) / sizeof(schema1_piattaforme[0]);