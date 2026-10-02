#pragma once
#include "bn_regular_bg_item.h"
#include "bn_regular_bg_map_item.h"

struct platform_map
{
    static constexpr int MAX_CELLE = 192 * 64;   // dimensione hardware reale verificata per s1/s2

    alignas(int) bn::regular_bg_map_cell cells[MAX_CELLE];
    bn::regular_bg_map_item map_item;

    explicit platform_map(const bn::regular_bg_item& originale);
};