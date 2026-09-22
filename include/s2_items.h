#pragma once
#include "item.h"

constexpr item_def schema2_items[] = {
    { ITEM_TIPO_CHIAVE,  0,             300, 400 },
    { ITEM_TIPO_CHIAVE,  0,             700, 350 },
    { ITEM_TIPO_CHIAVE,  0,            1100, 400 },
    { ITEM_TIPO_POWERUP, POWERUP_SALTO, 500, 300 },
    { ITEM_TIPO_POWERUP, POWERUP_CORSA, 900, 250 },
    { ITEM_TIPO_ENERGIA, 0,             200, 300 },
    { ITEM_TIPO_ENERGIA, 0,            1300, 300 },
};
constexpr int schema2_items_count = sizeof(schema2_items) / sizeof(schema2_items[0]);