#ifndef BN_SPRITE_ITEMS_DOG_H
#define BN_SPRITE_ITEMS_DOG_H

#include "bn_sprite_item.h"

//{{BLOCK(dog_bn_gfx)

//======================================================================
//
//	dog_bn_gfx, 480x32@4, 
//	+ palette 16 entries, not compressed
//	+ 240 tiles Metatiled by 4x4 not compressed
//	Total size: 32 + 7680 = 7712
//
//	Time-stamp: 2026-09-25, 20:43:14
//	Exported by Cearn's GBA Image Transmogrifier, v0.9.2
//	( http://www.coranac.com/projects/#grit )
//
//======================================================================

#ifndef GRIT_DOG_BN_GFX_H
#define GRIT_DOG_BN_GFX_H

#define dog_bn_gfxTilesLen 7680
extern const bn::tile dog_bn_gfxTiles[240];

#define dog_bn_gfxPalLen 32
extern const bn::color dog_bn_gfxPal[16];

#endif // GRIT_DOG_BN_GFX_H

//}}BLOCK(dog_bn_gfx)

namespace bn::sprite_items
{
    constexpr inline sprite_item dog(sprite_shape_size(sprite_shape::SQUARE, sprite_size::BIG), 
            sprite_tiles_item(span<const tile>(dog_bn_gfxTiles, 240), bpp_mode::BPP_4, compression_type::NONE, 15), 
            sprite_palette_item(span<const color>(dog_bn_gfxPal, 16), bpp_mode::BPP_4, compression_type::NONE));
}

#endif

