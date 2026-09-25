#ifndef BN_REGULAR_BG_ITEMS_TEXT_BG_H
#define BN_REGULAR_BG_ITEMS_TEXT_BG_H

#include "bn_regular_bg_item.h"

//{{BLOCK(text_bg_bn_gfx)

//======================================================================
//
//	text_bg_bn_gfx, 256x256@4, 
//	+ palette 16 entries, not compressed
//	+ 1 tiles (t|f|p reduced) not compressed
//	+ regular map (flat), not compressed, 32x32 
//	Total size: 32 + 32 + 2048 = 2112
//
//	Time-stamp: 2026-09-25, 20:43:14
//	Exported by Cearn's GBA Image Transmogrifier, v0.9.2
//	( http://www.coranac.com/projects/#grit )
//
//======================================================================

#ifndef GRIT_TEXT_BG_BN_GFX_H
#define GRIT_TEXT_BG_BN_GFX_H

#define text_bg_bn_gfxTilesLen 32
extern const bn::tile text_bg_bn_gfxTiles[1];

#define text_bg_bn_gfxMapLen 2048
extern const bn::regular_bg_map_cell text_bg_bn_gfxMap[1024];

#define text_bg_bn_gfxPalLen 32
extern const bn::color text_bg_bn_gfxPal[16];

#endif // GRIT_TEXT_BG_BN_GFX_H

//}}BLOCK(text_bg_bn_gfx)

namespace bn::regular_bg_items
{
    constexpr inline regular_bg_item text_bg(
            regular_bg_tiles_item(span<const tile>(text_bg_bn_gfxTiles, 1), bpp_mode::BPP_4, compression_type::NONE), 
            bg_palette_item(span<const color>(text_bg_bn_gfxPal, 16), bpp_mode::BPP_4, compression_type::NONE),
            regular_bg_map_item(text_bg_bn_gfxMap[0], size(32, 32), compression_type::NONE, 1, false));
}

#endif

