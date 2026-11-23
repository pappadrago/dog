#pragma once

#include "oggetto_percorso.h"
#include "bn_sprite_items_items.h"
#include "bn_sprite_items_zombie.h"

inline void aggiorna_sprite_percorso(
    bn::optional<bn::sprite_ptr>& sprite, uint8_t sprite_size, bool in_pausa)
{
    if (!sprite)
        return;

    switch (sprite_size)
    {
    case PERCORSO_SPRITE_16:
        sprite->set_tiles(bn::sprite_items::items.tiles_item(), in_pausa ? 20 : 19);
        break;
    case PERCORSO_SPRITE_32:
    default:
        sprite->set_tiles(bn::sprite_items::zombie.tiles_item(), in_pausa ? 19 : 18);
        break;
    }
}
