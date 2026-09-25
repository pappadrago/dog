#ifndef BN_REGULAR_BG_ITEMS_S1FG_H
#define BN_REGULAR_BG_ITEMS_S1FG_H

#include "bn_regular_bg_item.h"

//{{BLOCK(s1fg_bn_gfx)

//======================================================================
//
//	s1fg_bn_gfx, 1536x512@4, 
//	+ palette 16 entries, not compressed
//	+ 186 tiles (t|f|p reduced) not compressed
//	+ regular map (flat), not compressed, 192x64 
//	Total size: 32 + 5952 + 24576 = 30560
//
//	Time-stamp: 2026-09-25, 20:43:14
//	Exported by Cearn's GBA Image Transmogrifier, v0.9.2
//	( http://www.coranac.com/projects/#grit )
//
//======================================================================

#ifndef GRIT_S1FG_BN_GFX_H
#define GRIT_S1FG_BN_GFX_H

#define s1fg_bn_gfxTilesLen 5952
extern const bn::tile s1fg_bn_gfxTiles[186];

#define s1fg_bn_gfxMapLen 24576
extern const bn::regular_bg_map_cell s1fg_bn_gfxMap[12288];

#define s1fg_bn_gfxPalLen 32
extern const bn::color s1fg_bn_gfxPal[16];

#endif // GRIT_S1FG_BN_GFX_H

//}}BLOCK(s1fg_bn_gfx)

namespace bn::regular_bg_items
{
    constexpr inline regular_bg_item s1fg(
            regular_bg_tiles_item(span<const tile>(s1fg_bn_gfxTiles, 186), bpp_mode::BPP_4, compression_type::NONE), 
            bg_palette_item(span<const color>(s1fg_bn_gfxPal, 16), bpp_mode::BPP_4, compression_type::NONE),
            regular_bg_map_item(s1fg_bn_gfxMap[0], size(192, 64), compression_type::NONE, 1, true));
}

#endif

