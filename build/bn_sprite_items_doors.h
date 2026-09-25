#ifndef BN_SPRITE_ITEMS_DOORS_H
#define BN_SPRITE_ITEMS_DOORS_H

#include "bn_sprite_item.h"

//{{BLOCK(doors_bn_gfx)

//======================================================================
//
//	doors_bn_gfx, 96x32@4, 
//	+ palette 16 entries, not compressed
//	+ 48 tiles Metatiled by 2x4 not compressed
//	Total size: 32 + 1536 = 1568
//
//	Time-stamp: 2026-09-25, 20:43:14
//	Exported by Cearn's GBA Image Transmogrifier, v0.9.2
//	( http://www.coranac.com/projects/#grit )
//
//======================================================================

#ifndef GRIT_DOORS_BN_GFX_H
#define GRIT_DOORS_BN_GFX_H

#define doors_bn_gfxTilesLen 1536
extern const bn::tile doors_bn_gfxTiles[48];

#define doors_bn_gfxPalLen 32
extern const bn::color doors_bn_gfxPal[16];

#endif // GRIT_DOORS_BN_GFX_H

//}}BLOCK(doors_bn_gfx)

namespace bn::sprite_items
{
    constexpr inline sprite_item doors(sprite_shape_size(sprite_shape::TALL, sprite_size::BIG), 
            sprite_tiles_item(span<const tile>(doors_bn_gfxTiles, 48), bpp_mode::BPP_4, compression_type::NONE, 6), 
            sprite_palette_item(span<const color>(doors_bn_gfxPal, 16), bpp_mode::BPP_4, compression_type::NONE));
}

#endif

