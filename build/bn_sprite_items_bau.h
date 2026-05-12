#ifndef BN_SPRITE_ITEMS_BAU_H
#define BN_SPRITE_ITEMS_BAU_H

#include "bn_sprite_item.h"

//{{BLOCK(bau_bn_gfx)

//======================================================================
//
//	bau_bn_gfx, 64x32@4, 
//	+ palette 16 entries, not compressed
//	+ 32 tiles Metatiled by 4x4 not compressed
//	Total size: 32 + 1024 = 1056
//
//	Time-stamp: 2026-05-05, 21:54:04
//	Exported by Cearn's GBA Image Transmogrifier, v0.9.2
//	( http://www.coranac.com/projects/#grit )
//
//======================================================================

#ifndef GRIT_BAU_BN_GFX_H
#define GRIT_BAU_BN_GFX_H

#define bau_bn_gfxTilesLen 1024
extern const bn::tile bau_bn_gfxTiles[32];

#define bau_bn_gfxPalLen 32
extern const bn::color bau_bn_gfxPal[16];

#endif // GRIT_BAU_BN_GFX_H

//}}BLOCK(bau_bn_gfx)

namespace bn::sprite_items
{
    constexpr inline sprite_item bau(sprite_shape_size(sprite_shape::SQUARE, sprite_size::BIG), 
            sprite_tiles_item(span<const tile>(bau_bn_gfxTiles, 32), bpp_mode::BPP_4, compression_type::NONE, 2), 
            sprite_palette_item(span<const color>(bau_bn_gfxPal, 16), bpp_mode::BPP_4, compression_type::NONE));
}

#endif

