#ifndef BN_REGULAR_BG_ITEMS_S2FG_H
#define BN_REGULAR_BG_ITEMS_S2FG_H

#include "bn_regular_bg_item.h"

//{{BLOCK(s2fg_bn_gfx)

//======================================================================
//
//	s2fg_bn_gfx, 1536x512@4, 
//	+ palette 16 entries, not compressed
//	+ 173 tiles (t|f|p reduced) not compressed
//	+ regular map (flat), not compressed, 192x64 
//	Total size: 32 + 5536 + 24576 = 30144
//
//	Time-stamp: 2026-09-21, 21:17:55
//	Exported by Cearn's GBA Image Transmogrifier, v0.9.2
//	( http://www.coranac.com/projects/#grit )
//
//======================================================================

#ifndef GRIT_S2FG_BN_GFX_H
#define GRIT_S2FG_BN_GFX_H

#define s2fg_bn_gfxTilesLen 5536
extern const bn::tile s2fg_bn_gfxTiles[173];

#define s2fg_bn_gfxMapLen 24576
extern const bn::regular_bg_map_cell s2fg_bn_gfxMap[12288];

#define s2fg_bn_gfxPalLen 32
extern const bn::color s2fg_bn_gfxPal[16];

#endif // GRIT_S2FG_BN_GFX_H

//}}BLOCK(s2fg_bn_gfx)

namespace bn::regular_bg_items
{
    constexpr inline regular_bg_item s2fg(
            regular_bg_tiles_item(span<const tile>(s2fg_bn_gfxTiles, 173), bpp_mode::BPP_4, compression_type::NONE), 
            bg_palette_item(span<const color>(s2fg_bn_gfxPal, 16), bpp_mode::BPP_4, compression_type::NONE),
            regular_bg_map_item(s2fg_bn_gfxMap[0], size(192, 64), compression_type::NONE, 1, true));
}

#endif

