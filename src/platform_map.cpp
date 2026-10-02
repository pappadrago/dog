#include "platform_map.h"
#include "bn_assert.h"

platform_map::platform_map(const bn::regular_bg_item& originale) :
    map_item(cells[0],
             originale.map_item().dimensions(),
             originale.map_item().compression(),
             originale.map_item().maps_count(),
             originale.map_item().big())
{
    bn::regular_bg_map_item item_originale = originale.map_item();
    const bn::regular_bg_map_cell& primo = item_originale.cells_ref();
    int n = item_originale.cells_count();

    BN_ASSERT(n <= MAX_CELLE, "platform_map: buffer troppo piccolo");

    for (int i = 0; i < n; i++)
        cells[i] = (&primo)[i];
}