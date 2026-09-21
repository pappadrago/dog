#ifndef BN_SPRITE_ITEMS_FOX_H
#define BN_SPRITE_ITEMS_FOX_H

#include "bn_sprite_item.h"

//{{BLOCK(fox_bn_gfx)

//======================================================================
//
//	fox_bn_gfx, 192x64@4, 
//	+ palette 16 entries, not compressed
//	+ 192 tiles Metatiled by 4x4 not compressed
//	Total size: 32 + 6144 = 6176
//
//	Time-stamp: 2026-09-18, 12:51:48
//	Exported by Cearn's GBA Image Transmogrifier, v0.9.2
//	( http://www.coranac.com/projects/#grit )
//
//======================================================================

#ifndef GRIT_FOX_BN_GFX_H
#define GRIT_FOX_BN_GFX_H

#define fox_bn_gfxTilesLen 6144
extern const bn::tile fox_bn_gfxTiles[192];

#define fox_bn_gfxPalLen 32
extern const bn::color fox_bn_gfxPal[16];

#endif // GRIT_FOX_BN_GFX_H

//}}BLOCK(fox_bn_gfx)

namespace bn::sprite_items
{
    constexpr inline sprite_item fox(sprite_shape_size(sprite_shape::SQUARE, sprite_size::BIG), 
            sprite_tiles_item(span<const tile>(fox_bn_gfxTiles, 192), bpp_mode::BPP_4, compression_type::NONE, 12), 
            sprite_palette_item(span<const color>(fox_bn_gfxPal, 16), bpp_mode::BPP_4, compression_type::NONE));
}

#endif

