#pragma once
#include "item.h"

constexpr item_def schema1_items[] = {
    { ITEM_TIPO_CHIAVE,  0,             50, 400 },
    { ITEM_TIPO_CHIAVE,  0,             100, 350 },
    { ITEM_TIPO_CHIAVE,  0,            150, 400 },
    { ITEM_TIPO_POWERUP, POWERUP_SALTO, 500, 300 },
    { ITEM_TIPO_POWERUP, POWERUP_CORSA, 900, 250 },
    { ITEM_TIPO_ENERGIA, 0,             200, 300 },
    { ITEM_TIPO_ENERGIA, 0,            1300, 300 },
};
constexpr int schema1_items_count = sizeof(schema1_items) / sizeof(schema1_items[0]);