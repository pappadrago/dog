#ifndef BN_REGULAR_BG_ITEMS_DAYPLATF_H
#define BN_REGULAR_BG_ITEMS_DAYPLATF_H

#include "bn_regular_bg_item.h"

//{{BLOCK(dayplatf_bn_gfx)

//======================================================================
//
//	dayplatf_bn_gfx, 1792x256@4, 
//	+ palette 16 entries, not compressed
//	+ 381 tiles (t|f|p reduced) not compressed
//	+ regular map (flat), not compressed, 224x32 
//	Total size: 32 + 12192 + 14336 = 26560
//
//	Time-stamp: 2026-05-06, 22:18:32
//	Exported by Cearn's GBA Image Transmogrifier, v0.9.2
//	( http://www.coranac.com/projects/#grit )
//
//======================================================================

#ifndef GRIT_DAYPLATF_BN_GFX_H
#define GRIT_DAYPLATF_BN_GFX_H

#define dayplatf_bn_gfxTilesLen 12192
extern const bn::tile dayplatf_bn_gfxTiles[381];

#define dayplatf_bn_gfxMapLen 14336
extern const bn::regular_bg_map_cell dayplatf_bn_gfxMap[7168];

#define dayplatf_bn_gfxPalLen 32
extern const bn::color dayplatf_bn_gfxPal[16];

#endif // GRIT_DAYPLATF_BN_GFX_H

//}}BLOCK(dayplatf_bn_gfx)

namespace bn::regular_bg_items
{
    constexpr inline regular_bg_item dayplatf(
            regular_bg_tiles_item(span<const tile>(dayplatf_bn_gfxTiles, 381), bpp_mode::BPP_4, compression_type::NONE), 
            bg_palette_item(span<const color>(dayplatf_bn_gfxPal, 16), bpp_mode::BPP_4, compression_type::NONE),
            regular_bg_map_item(dayplatf_bn_gfxMap[0], size(224, 32), compression_type::NONE, 1, true));
}

#endif

