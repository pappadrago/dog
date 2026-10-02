// include/s1_piattaforme.h
#pragma once
#include "piattaforma.h"

constexpr piattaforma_def schema1_piattaforme[] = {
    // tile 16px (x1,y1,x2,y2) | hw solido[4] | hw vuoto[4] | stato iniziale | trigger
    { 27, 20, 31, 20, {40,41,56,57}, {10,11,26,27}, 0, 0 },
};
constexpr int schema1_piattaforme_count = sizeof(schema1_piattaforme) / sizeof(schema1_piattaforme[0]);