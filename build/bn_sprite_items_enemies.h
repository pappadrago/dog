#ifndef BN_SPRITE_ITEMS_ENEMIES_H
#define BN_SPRITE_ITEMS_ENEMIES_H

#include "bn_sprite_item.h"

//{{BLOCK(enemies_bn_gfx)

//======================================================================
//
//	enemies_bn_gfx, 480x16@4, 
//	+ palette 16 entries, not compressed
//	+ 120 tiles Metatiled by 2x2 not compressed
//	Total size: 32 + 3840 = 3872
//
//	Time-stamp: 2026-09-18, 12:51:48
//	Exported by Cearn's GBA Image Transmogrifier, v0.9.2
//	( http://www.coranac.com/projects/#grit )
//
//======================================================================

#ifndef GRIT_ENEMIES_BN_GFX_H
#define GRIT_ENEMIES_BN_GFX_H

#define enemies_bn_gfxTilesLen 3840
extern const bn::tile enemies_bn_gfxTiles[120];

#define enemies_bn_gfxPalLen 32
extern const bn::color enemies_bn_gfxPal[16];

#endif // GRIT_ENEMIES_BN_GFX_H

//}}BLOCK(enemies_bn_gfx)

namespace bn::sprite_items
{
    constexpr inline sprite_item enemies(sprite_shape_size(sprite_shape::SQUARE, sprite_size::NORMAL), 
            sprite_tiles_item(span<const tile>(enemies_bn_gfxTiles, 120), bpp_mode::BPP_4, compression_type::NONE, 30), 
            sprite_palette_item(span<const color>(enemies_bn_gfxPal, 16), bpp_mode::BPP_4, compression_type::NONE));
}

#endif

