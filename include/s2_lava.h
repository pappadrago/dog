#pragma once
#include "lava.h"

constexpr lava_zona_def schema2_lava[] = {
    { 49,3, 55, 3,
      { { 1,2,LAVA_TILE,LAVA_TILE },{ 2,3,LAVA_TILE,LAVA_TILE }, },
      2, 12, 15 },
};
constexpr int schema2_lava_count = sizeof(schema2_lava) / sizeof(schema2_lava[0]);