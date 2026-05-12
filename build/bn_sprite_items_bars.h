#ifndef BN_SPRITE_ITEMS_BARS_H
#define BN_SPRITE_ITEMS_BARS_H

#include "bn_sprite_item.h"

//{{BLOCK(bars_bn_gfx)

//======================================================================
//
//	bars_bn_gfx, 64x8@4, 
//	+ palette 16 entries, not compressed
//	+ 8 tiles Metatiled by 4x1 not compressed
//	Total size: 32 + 256 = 288
//
//	Time-stamp: 2026-04-30, 21:15:11
//	Exported by Cearn's GBA Image Transmogrifier, v0.9.2
//	( http://www.coranac.com/projects/#grit )
//
//======================================================================

#ifndef GRIT_BARS_BN_GFX_H
#define GRIT_BARS_BN_GFX_H

#define bars_bn_gfxTilesLen 256
extern const bn::tile bars_bn_gfxTiles[8];

#define bars_bn_gfxPalLen 32
extern const bn::color bars_bn_gfxPal[16];

#endif // GRIT_BARS_BN_GFX_H

//}}BLOCK(bars_bn_gfx)

namespace bn::sprite_items
{
    constexpr inline sprite_item bars(sprite_shape_size(sprite_shape::WIDE, sprite_size::NORMAL), 
            sprite_tiles_item(span<const tile>(bars_bn_gfxTiles, 8), bpp_mode::BPP_4, compression_type::NONE, 2), 
            sprite_palette_item(span<const color>(bars_bn_gfxPal, 16), bpp_mode::BPP_4, compression_type::NONE));
}

#endif

