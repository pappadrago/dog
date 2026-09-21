#ifndef BN_REGULAR_BG_ITEMS_COUNTRY_H
#define BN_REGULAR_BG_ITEMS_COUNTRY_H

#include "bn_regular_bg_item.h"

//{{BLOCK(country_bn_gfx)

//======================================================================
//
//	country_bn_gfx, 1792x256@4, 
//	+ palette 16 entries, not compressed
//	+ 517 tiles (t|f|p reduced) not compressed
//	+ regular map (flat), not compressed, 224x32 
//	Total size: 32 + 16544 + 14336 = 30912
//
//	Time-stamp: 2026-09-18, 12:51:48
//	Exported by Cearn's GBA Image Transmogrifier, v0.9.2
//	( http://www.coranac.com/projects/#grit )
//
//======================================================================

#ifndef GRIT_COUNTRY_BN_GFX_H
#define GRIT_COUNTRY_BN_GFX_H

#define country_bn_gfxTilesLen 16544
extern const bn::tile country_bn_gfxTiles[517];

#define country_bn_gfxMapLen 14336
extern const bn::regular_bg_map_cell country_bn_gfxMap[7168];

#define country_bn_gfxPalLen 32
extern const bn::color country_bn_gfxPal[16];

#endif // GRIT_COUNTRY_BN_GFX_H

//}}BLOCK(country_bn_gfx)

namespace bn::regular_bg_items
{
    constexpr inline regular_bg_item country(
            regular_bg_tiles_item(span<const tile>(country_bn_gfxTiles, 517), bpp_mode::BPP_4, compression_type::NONE), 
            bg_palette_item(span<const color>(country_bn_gfxPal, 16), bpp_mode::BPP_4, compression_type::NONE),
            regular_bg_map_item(country_bn_gfxMap[0], size(224, 32), compression_type::NONE, 1, true));
}

#endif

