#ifndef BN_SPRITE_ITEMS_KING_H
#define BN_SPRITE_ITEMS_KING_H

#include "bn_sprite_item.h"

//{{BLOCK(king_bn_gfx)

//======================================================================
//
//	king_bn_gfx, 640x16@4, 
//	+ palette 16 entries, not compressed
//	+ 160 tiles Metatiled by 4x2 not compressed
//	Total size: 32 + 5120 = 5152
//
//	Time-stamp: 2026-09-25, 20:43:14
//	Exported by Cearn's GBA Image Transmogrifier, v0.9.2
//	( http://www.coranac.com/projects/#grit )
//
//======================================================================

#ifndef GRIT_KING_BN_GFX_H
#define GRIT_KING_BN_GFX_H

#define king_bn_gfxTilesLen 5120
extern const bn::tile king_bn_gfxTiles[160];

#define king_bn_gfxPalLen 32
extern const bn::color king_bn_gfxPal[16];

#endif // GRIT_KING_BN_GFX_H

//}}BLOCK(king_bn_gfx)

namespace bn::sprite_items
{
    constexpr inline sprite_item king(sprite_shape_size(sprite_shape::WIDE, sprite_size::BIG), 
            sprite_tiles_item(span<const tile>(king_bn_gfxTiles, 160), bpp_mode::BPP_4, compression_type::NONE, 20), 
            sprite_palette_item(span<const color>(king_bn_gfxPal, 16), bpp_mode::BPP_4, compression_type::NONE));
}

#endif

