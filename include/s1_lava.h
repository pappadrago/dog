#pragma once
#include "lava.h"

constexpr lava_zona_def schema1_lava[] = {
    { 48,28, 57, 28,
      { { 1,2,LAVA_TILE,LAVA_TILE },{ 2,3,LAVA_TILE,LAVA_TILE }, },
      2, 12, 15 },
};
constexpr int schema1_lava_count = sizeof(schema1_lava) / sizeof(schema1_lava[0]);