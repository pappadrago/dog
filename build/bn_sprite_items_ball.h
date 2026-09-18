#ifndef BN_SPRITE_ITEMS_BALL_H
#define BN_SPRITE_ITEMS_BALL_H

#include "bn_sprite_item.h"

//{{BLOCK(ball_bn_gfx)

//======================================================================
//
//	ball_bn_gfx, 24x8@4, 
//	+ palette 16 entries, not compressed
//	+ 3 tiles not compressed
//	Total size: 32 + 96 = 128
//
//	Time-stamp: 2026-09-17, 16:24:48
//	Exported by Cearn's GBA Image Transmogrifier, v0.9.2
//	( http://www.coranac.com/projects/#grit )
//
//======================================================================

#ifndef GRIT_BALL_BN_GFX_H
#define GRIT_BALL_BN_GFX_H

#define ball_bn_gfxTilesLen 96
extern const bn::tile ball_bn_gfxTiles[3];

#define ball_bn_gfxPalLen 32
extern const bn::color ball_bn_gfxPal[16];

#endif // GRIT_BALL_BN_GFX_H

//}}BLOCK(ball_bn_gfx)

namespace bn::sprite_items
{
    constexpr inline sprite_item ball(sprite_shape_size(sprite_shape::SQUARE, sprite_size::SMALL), 
            sprite_tiles_item(span<const tile>(ball_bn_gfxTiles, 3), bpp_mode::BPP_4, compression_type::NONE, 3), 
            sprite_palette_item(span<const color>(ball_bn_gfxPal, 16), bpp_mode::BPP_4, compression_type::NONE));
}

#endif

