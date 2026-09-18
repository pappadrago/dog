#ifndef BN_SPRITE_ITEMS_DOG2_H
#define BN_SPRITE_ITEMS_DOG2_H

#include "bn_sprite_item.h"

//{{BLOCK(dog2_bn_gfx)

//======================================================================
//
//	dog2_bn_gfx, 192x64@4, 
//	+ palette 16 entries, not compressed
//	+ 192 tiles Metatiled by 4x4 not compressed
//	Total size: 32 + 6144 = 6176
//
//	Time-stamp: 2026-09-17, 16:24:48
//	Exported by Cearn's GBA Image Transmogrifier, v0.9.2
//	( http://www.coranac.com/projects/#grit )
//
//======================================================================

#ifndef GRIT_DOG2_BN_GFX_H
#define GRIT_DOG2_BN_GFX_H

#define dog2_bn_gfxTilesLen 6144
extern const bn::tile dog2_bn_gfxTiles[192];

#define dog2_bn_gfxPalLen 32
extern const bn::color dog2_bn_gfxPal[16];

#endif // GRIT_DOG2_BN_GFX_H

//}}BLOCK(dog2_bn_gfx)

namespace bn::sprite_items
{
    constexpr inline sprite_item dog2(sprite_shape_size(sprite_shape::SQUARE, sprite_size::BIG), 
            sprite_tiles_item(span<const tile>(dog2_bn_gfxTiles, 192), bpp_mode::BPP_4, compression_type::NONE, 12), 
            sprite_palette_item(span<const color>(dog2_bn_gfxPal, 16), bpp_mode::BPP_4, compression_type::NONE));
}

#endif

