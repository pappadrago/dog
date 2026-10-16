#pragma once
#include "bn_regular_bg_item.h"
#include "bn_regular_bg_map_item.h"

struct platform_map
{
    static constexpr int MAX_CELLE = 192 * 32 * 4;    // dimensione hardware reale, ogni tile 16x16 è composta da 4 tile hardware

    alignas(int) bn::regular_bg_map_cell cells[MAX_CELLE];
    bn::regular_bg_map_item map_item;

    explicit platform_map(const bn::regular_bg_item& originale);
};