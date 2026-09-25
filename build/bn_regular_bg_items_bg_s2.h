#ifndef BN_REGULAR_BG_ITEMS_BG_S2_H
#define BN_REGULAR_BG_ITEMS_BG_S2_H

#include "bn_regular_bg_item.h"

//{{BLOCK(bg_s2_bn_gfx)

//======================================================================
//
//	bg_s2_bn_gfx, 512x512@4, 
//	+ palette 16 entries, not compressed
//	+ 466 tiles (t|f|p reduced) not compressed
//	+ regular map (in SBBs), not compressed, 64x64 
//	Total size: 32 + 14912 + 8192 = 23136
//
//	Time-stamp: 2026-09-25, 20:43:14
//	Exported by Cearn's GBA Image Transmogrifier, v0.9.2
//	( http://www.coranac.com/projects/#grit )
//
//======================================================================

#ifndef GRIT_BG_S2_BN_GFX_H
#define GRIT_BG_S2_BN_GFX_H

#define bg_s2_bn_gfxTilesLen 14912
extern const bn::tile bg_s2_bn_gfxTiles[466];

#define bg_s2_bn_gfxMapLen 8192
extern const bn::regular_bg_map_cell bg_s2_bn_gfxMap[4096];

#define bg_s2_bn_gfxPalLen 32
extern const bn::color bg_s2_bn_gfxPal[16];

#endif // GRIT_BG_S2_BN_GFX_H

//}}BLOCK(bg_s2_bn_gfx)

namespace bn::regular_bg_items
{
    constexpr inline regular_bg_item bg_s2(
            regular_bg_tiles_item(span<const tile>(bg_s2_bn_gfxTiles, 466), bpp_mode::BPP_4, compression_type::NONE), 
            bg_palette_item(span<const color>(bg_s2_bn_gfxPal, 16), bpp_mode::BPP_4, compression_type::NONE),
            regular_bg_map_item(bg_s2_bn_gfxMap[0], size(64, 32), compression_type::NONE, 2, false));
}

#endif

