#ifndef BN_SPRITE_ITEMS_ITEMS_H
#define BN_SPRITE_ITEMS_ITEMS_H

#include "bn_sprite_item.h"

//{{BLOCK(items_bn_gfx)

//======================================================================
//
//	items_bn_gfx, 512x16@4, 
//	+ palette 16 entries, not compressed
//	+ 128 tiles Metatiled by 2x2 not compressed
//	Total size: 32 + 4096 = 4128
//
//	Time-stamp: 2026-09-18, 12:51:48
//	Exported by Cearn's GBA Image Transmogrifier, v0.9.2
//	( http://www.coranac.com/projects/#grit )
//
//======================================================================

#ifndef GRIT_ITEMS_BN_GFX_H
#define GRIT_ITEMS_BN_GFX_H

#define items_bn_gfxTilesLen 4096
extern const bn::tile items_bn_gfxTiles[128];

#define items_bn_gfxPalLen 32
extern const bn::color items_bn_gfxPal[16];

#endif // GRIT_ITEMS_BN_GFX_H

//}}BLOCK(items_bn_gfx)

namespace bn::sprite_items
{
    constexpr inline sprite_item items(sprite_shape_size(sprite_shape::SQUARE, sprite_size::NORMAL), 
            sprite_tiles_item(span<const tile>(items_bn_gfxTiles, 128), bpp_mode::BPP_4, compression_type::NONE, 32), 
            sprite_palette_item(span<const color>(items_bn_gfxPal, 16), bpp_mode::BPP_4, compression_type::NONE));
}

#endif

