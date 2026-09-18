#ifndef BN_REGULAR_BG_ITEMS_S1_H
#define BN_REGULAR_BG_ITEMS_S1_H

#include "bn_regular_bg_item.h"

//{{BLOCK(s1_bn_gfx)

//======================================================================
//
//	s1_bn_gfx, 1536x512@4, 
//	+ palette 16 entries, not compressed
//	+ 112 tiles (t|f|p reduced) not compressed
//	+ regular map (flat), not compressed, 192x64 
//	Total size: 32 + 3584 + 24576 = 28192
//
//	Time-stamp: 2026-09-17, 23:48:03
//	Exported by Cearn's GBA Image Transmogrifier, v0.9.2
//	( http://www.coranac.com/projects/#grit )
//
//======================================================================

#ifndef GRIT_S1_BN_GFX_H
#define GRIT_S1_BN_GFX_H

#define s1_bn_gfxTilesLen 3584
extern const bn::tile s1_bn_gfxTiles[112];

#define s1_bn_gfxMapLen 24576
extern const bn::regular_bg_map_cell s1_bn_gfxMap[12288];

#define s1_bn_gfxPalLen 32
extern const bn::color s1_bn_gfxPal[16];

#endif // GRIT_S1_BN_GFX_H

//}}BLOCK(s1_bn_gfx)

namespace bn::regular_bg_items
{
    constexpr inline regular_bg_item s1(
            regular_bg_tiles_item(span<const tile>(s1_bn_gfxTiles, 112), bpp_mode::BPP_4, compression_type::NONE), 
            bg_palette_item(span<const color>(s1_bn_gfxPal, 16), bpp_mode::BPP_4, compression_type::NONE),
            regular_bg_map_item(s1_bn_gfxMap[0], size(192, 64), compression_type::NONE, 1, true));
}

#endif

