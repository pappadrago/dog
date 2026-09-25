#ifndef BN_SPRITE_ITEMS_CAT_H
#define BN_SPRITE_ITEMS_CAT_H

#include "bn_sprite_item.h"

//{{BLOCK(cat_bn_gfx)

//======================================================================
//
//	cat_bn_gfx, 192x32@4, 
//	+ palette 16 entries, not compressed
//	+ 96 tiles Metatiled by 2x2 not compressed
//	Total size: 32 + 3072 = 3104
//
//	Time-stamp: 2026-09-25, 20:43:14
//	Exported by Cearn's GBA Image Transmogrifier, v0.9.2
//	( http://www.coranac.com/projects/#grit )
//
//======================================================================

#ifndef GRIT_CAT_BN_GFX_H
#define GRIT_CAT_BN_GFX_H

#define cat_bn_gfxTilesLen 3072
extern const bn::tile cat_bn_gfxTiles[96];

#define cat_bn_gfxPalLen 32
extern const bn::color cat_bn_gfxPal[16];

#endif // GRIT_CAT_BN_GFX_H

//}}BLOCK(cat_bn_gfx)

namespace bn::sprite_items
{
    constexpr inline sprite_item cat(sprite_shape_size(sprite_shape::SQUARE, sprite_size::NORMAL), 
            sprite_tiles_item(span<const tile>(cat_bn_gfxTiles, 96), bpp_mode::BPP_4, compression_type::NONE, 24), 
            sprite_palette_item(span<const color>(cat_bn_gfxPal, 16), bpp_mode::BPP_4, compression_type::NONE));
}

#endif

