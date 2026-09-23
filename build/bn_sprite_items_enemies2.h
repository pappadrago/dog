#ifndef BN_SPRITE_ITEMS_ENEMIES2_H
#define BN_SPRITE_ITEMS_ENEMIES2_H

#include "bn_sprite_item.h"

//{{BLOCK(enemies2_bn_gfx)

//======================================================================
//
//	enemies2_bn_gfx, 512x16@4, 
//	+ palette 16 entries, not compressed
//	+ 128 tiles Metatiled by 2x2 not compressed
//	Total size: 32 + 4096 = 4128
//
//	Time-stamp: 2026-09-22, 22:02:30
//	Exported by Cearn's GBA Image Transmogrifier, v0.9.2
//	( http://www.coranac.com/projects/#grit )
//
//======================================================================

#ifndef GRIT_ENEMIES2_BN_GFX_H
#define GRIT_ENEMIES2_BN_GFX_H

#define enemies2_bn_gfxTilesLen 4096
extern const bn::tile enemies2_bn_gfxTiles[128];

#define enemies2_bn_gfxPalLen 32
extern const bn::color enemies2_bn_gfxPal[16];

#endif // GRIT_ENEMIES2_BN_GFX_H

//}}BLOCK(enemies2_bn_gfx)

namespace bn::sprite_items
{
    constexpr inline sprite_item enemies2(sprite_shape_size(sprite_shape::SQUARE, sprite_size::NORMAL), 
            sprite_tiles_item(span<const tile>(enemies2_bn_gfxTiles, 128), bpp_mode::BPP_4, compression_type::NONE, 32), 
            sprite_palette_item(span<const color>(enemies2_bn_gfxPal, 16), bpp_mode::BPP_4, compression_type::NONE));
}

#endif

