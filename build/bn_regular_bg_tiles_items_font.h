#ifndef BN_REGULAR_BG_TILES_ITEMS_FONT_H
#define BN_REGULAR_BG_TILES_ITEMS_FONT_H

#include "bn_regular_bg_tiles_item.h"

//{{BLOCK(font_bn_gfx)

//======================================================================
//
//	font_bn_gfx, 960x8@4, 
//	+ 120 tiles not compressed
//	Total size: 3840 = 3840
//
//	Time-stamp: 2026-09-18, 12:51:48
//	Exported by Cearn's GBA Image Transmogrifier, v0.9.2
//	( http://www.coranac.com/projects/#grit )
//
//======================================================================

#ifndef GRIT_FONT_BN_GFX_H
#define GRIT_FONT_BN_GFX_H

#define font_bn_gfxTilesLen 3840
extern const bn::tile font_bn_gfxTiles[120];

#endif // GRIT_FONT_BN_GFX_H

//}}BLOCK(font_bn_gfx)

namespace bn::regular_bg_tiles_items
{
    constexpr inline regular_bg_tiles_item font(
            span<const tile>(font_bn_gfxTiles, 120), bpp_mode::BPP_4, compression_type::NONE);
}

#endif

