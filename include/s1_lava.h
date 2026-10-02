#pragma once
#include "lava.h"

constexpr lava_zona_def schema1_lava[] = {
    { 30, 25, 33, 25,
      { {60,61,76,77}, {62,63,78,79}, {64,65,80,81} },
      3, 18, 15 },
};
constexpr int schema1_lava_count = sizeof(schema1_lava) / sizeof(schema1_lava[0]);