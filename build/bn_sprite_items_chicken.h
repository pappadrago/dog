#ifndef BN_SPRITE_ITEMS_CHICKEN_H
#define BN_SPRITE_ITEMS_CHICKEN_H

#include "bn_sprite_item.h"

//{{BLOCK(chicken_bn_gfx)

//======================================================================
//
//	chicken_bn_gfx, 64x16@4, 
//	+ palette 16 entries, not compressed
//	+ 16 tiles Metatiled by 2x2 not compressed
//	Total size: 32 + 512 = 544
//
//	Time-stamp: 2026-09-17, 16:24:48
//	Exported by Cearn's GBA Image Transmogrifier, v0.9.2
//	( http://www.coranac.com/projects/#grit )
//
//======================================================================

#ifndef GRIT_CHICKEN_BN_GFX_H
#define GRIT_CHICKEN_BN_GFX_H

#define chicken_bn_gfxTilesLen 512
extern const bn::tile chicken_bn_gfxTiles[16];

#define chicken_bn_gfxPalLen 32
extern const bn::color chicken_bn_gfxPal[16];

#endif // GRIT_CHICKEN_BN_GFX_H

//}}BLOCK(chicken_bn_gfx)

namespace bn::sprite_items
{
    constexpr inline sprite_item chicken(sprite_shape_size(sprite_shape::SQUARE, sprite_size::NORMAL), 
            sprite_tiles_item(span<const tile>(chicken_bn_gfxTiles, 16), bpp_mode::BPP_4, compression_type::NONE, 4), 
            sprite_palette_item(span<const color>(chicken_bn_gfxPal, 16), bpp_mode::BPP_4, compression_type::NONE));
}

#endif

