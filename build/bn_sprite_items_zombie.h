#ifndef BN_SPRITE_ITEMS_ZOMBIE_H
#define BN_SPRITE_ITEMS_ZOMBIE_H

#include "bn_sprite_item.h"

//{{BLOCK(zombie_bn_gfx)

//======================================================================
//
//	zombie_bn_gfx, 256x64@4, 
//	+ palette 16 entries, not compressed
//	+ 256 tiles Metatiled by 4x4 not compressed
//	Total size: 32 + 8192 = 8224
//
//	Time-stamp: 2026-09-17, 16:24:48
//	Exported by Cearn's GBA Image Transmogrifier, v0.9.2
//	( http://www.coranac.com/projects/#grit )
//
//======================================================================

#ifndef GRIT_ZOMBIE_BN_GFX_H
#define GRIT_ZOMBIE_BN_GFX_H

#define zombie_bn_gfxTilesLen 8192
extern const bn::tile zombie_bn_gfxTiles[256];

#define zombie_bn_gfxPalLen 32
extern const bn::color zombie_bn_gfxPal[16];

#endif // GRIT_ZOMBIE_BN_GFX_H

//}}BLOCK(zombie_bn_gfx)

namespace bn::sprite_items
{
    constexpr inline sprite_item zombie(sprite_shape_size(sprite_shape::SQUARE, sprite_size::BIG), 
            sprite_tiles_item(span<const tile>(zombie_bn_gfxTiles, 256), bpp_mode::BPP_4, compression_type::NONE, 16), 
            sprite_palette_item(span<const color>(zombie_bn_gfxPal, 16), bpp_mode::BPP_4, compression_type::NONE));
}

#endif

