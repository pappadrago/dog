#ifndef BN_SPRITE_ITEMS_HUMAN_H
#define BN_SPRITE_ITEMS_HUMAN_H

#include "bn_sprite_item.h"

//{{BLOCK(human_bn_gfx)

//======================================================================
//
//	human_bn_gfx, 640x64@4, 
//	+ palette 16 entries, not compressed
//	+ 640 tiles Metatiled by 4x4 not compressed
//	Total size: 32 + 20480 = 20512
//
//	Time-stamp: 2026-04-25, 21:51:02
//	Exported by Cearn's GBA Image Transmogrifier, v0.9.2
//	( http://www.coranac.com/projects/#grit )
//
//======================================================================

#ifndef GRIT_HUMAN_BN_GFX_H
#define GRIT_HUMAN_BN_GFX_H

#define human_bn_gfxTilesLen 20480
extern const bn::tile human_bn_gfxTiles[640];

#define human_bn_gfxPalLen 32
extern const bn::color human_bn_gfxPal[16];

#endif // GRIT_HUMAN_BN_GFX_H

//}}BLOCK(human_bn_gfx)

namespace bn::sprite_items
{
    constexpr inline sprite_item human(sprite_shape_size(sprite_shape::SQUARE, sprite_size::BIG), 
            sprite_tiles_item(span<const tile>(human_bn_gfxTiles, 640), bpp_mode::BPP_4, compression_type::NONE, 40), 
            sprite_palette_item(span<const color>(human_bn_gfxPal, 16), bpp_mode::BPP_4, compression_type::NONE));
}

#endif

