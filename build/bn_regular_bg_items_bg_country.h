#ifndef BN_REGULAR_BG_ITEMS_BG_COUNTRY_H
#define BN_REGULAR_BG_ITEMS_BG_COUNTRY_H

#include "bn_regular_bg_item.h"

//{{BLOCK(bg_country_bn_gfx)

//======================================================================
//
//	bg_country_bn_gfx, 512x512@4, 
//	+ palette 16 entries, not compressed
//	+ 652 tiles (t|f|p reduced) not compressed
//	+ regular map (in SBBs), not compressed, 64x64 
//	Total size: 32 + 20864 + 8192 = 29088
//
//	Time-stamp: 2026-09-17, 16:24:48
//	Exported by Cearn's GBA Image Transmogrifier, v0.9.2
//	( http://www.coranac.com/projects/#grit )
//
//======================================================================

#ifndef GRIT_BG_COUNTRY_BN_GFX_H
#define GRIT_BG_COUNTRY_BN_GFX_H

#define bg_country_bn_gfxTilesLen 20864
extern const bn::tile bg_country_bn_gfxTiles[652];

#define bg_country_bn_gfxMapLen 8192
extern const bn::regular_bg_map_cell bg_country_bn_gfxMap[4096];

#define bg_country_bn_gfxPalLen 32
extern const bn::color bg_country_bn_gfxPal[16];

#endif // GRIT_BG_COUNTRY_BN_GFX_H

//}}BLOCK(bg_country_bn_gfx)

namespace bn::regular_bg_items
{
    constexpr inline regular_bg_item bg_country(
            regular_bg_tiles_item(span<const tile>(bg_country_bn_gfxTiles, 652), bpp_mode::BPP_4, compression_type::NONE), 
            bg_palette_item(span<const color>(bg_country_bn_gfxPal, 16), bpp_mode::BPP_4, compression_type::NONE),
            regular_bg_map_item(bg_country_bn_gfxMap[0], size(64, 32), compression_type::NONE, 2, false));
}

#endif

