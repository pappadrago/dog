#pragma once
#include "enemy.h"

constexpr enemy_def schema2_enemies[] = {
    { TIPO_NEMICO_SPADACCINO_PATTUGLIATORE, ATTRIBUTO_NO,  400, DIR_RIGHT, 80  },
    { TIPO_NEMICO_GENERICO,                 ATTRIBUTO_AIM, 200, DIR_LEFT,  150, 2 },
    { TIPO_NEMICO_GENERICO,                 ATTRIBUTO_AIM, 600, DIR_RIGHT, 210, 5 },
    { TIPO_NEMICO_DRUIDO_DINAMICO,          ATTRIBUTO_AIM, 900, DIR_LEFT,  180 },
    { TIPO_NEMICO_ARCIERE_STATICO,          ATTRIBUTO_AIM, 1200, DIR_RIGHT, 240 },
    { TIPO_NEMICO_BLOB_PATTUGLIATORE,       ATTRIBUTO_NO,  1400, DIR_LEFT,  120 },
};
constexpr int schema2_enemies_count = sizeof(schema2_enemies) / sizeof(schema2_enemies[0]);