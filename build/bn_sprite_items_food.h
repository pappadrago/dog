#ifndef BN_SPRITE_ITEMS_FOOD_H
#define BN_SPRITE_ITEMS_FOOD_H

#include "bn_sprite_item.h"

//{{BLOCK(food_bn_gfx)

//======================================================================
//
//	food_bn_gfx, 128x128@4, 
//	+ palette 16 entries, not compressed
//	+ 256 tiles Metatiled by 2x2 not compressed
//	Total size: 32 + 8192 = 8224
//
//	Time-stamp: 2026-09-17, 16:24:48
//	Exported by Cearn's GBA Image Transmogrifier, v0.9.2
//	( http://www.coranac.com/projects/#grit )
//
//======================================================================

#ifndef GRIT_FOOD_BN_GFX_H
#define GRIT_FOOD_BN_GFX_H

#define food_bn_gfxTilesLen 8192
extern const bn::tile food_bn_gfxTiles[256];

#define food_bn_gfxPalLen 32
extern const bn::color food_bn_gfxPal[16];

#endif // GRIT_FOOD_BN_GFX_H

//}}BLOCK(food_bn_gfx)

namespace bn::sprite_items
{
    constexpr inline sprite_item food(sprite_shape_size(sprite_shape::SQUARE, sprite_size::NORMAL), 
            sprite_tiles_item(span<const tile>(food_bn_gfxTiles, 256), bpp_mode::BPP_4, compression_type::NONE, 64), 
            sprite_palette_item(span<const color>(food_bn_gfxPal, 16), bpp_mode::BPP_4, compression_type::NONE));
}

#endif

