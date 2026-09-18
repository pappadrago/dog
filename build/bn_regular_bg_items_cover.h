#ifndef BN_REGULAR_BG_ITEMS_COVER_H
#define BN_REGULAR_BG_ITEMS_COVER_H

#include "bn_regular_bg_item.h"

//{{BLOCK(cover_bn_gfx)

//======================================================================
//
//	cover_bn_gfx, 256x256@4, 
//	+ palette 16 entries, not compressed
//	+ 681 tiles (t|f|p reduced) not compressed
//	+ regular map (flat), not compressed, 32x32 
//	Total size: 32 + 21792 + 2048 = 23872
//
//	Time-stamp: 2026-09-17, 16:24:48
//	Exported by Cearn's GBA Image Transmogrifier, v0.9.2
//	( http://www.coranac.com/projects/#grit )
//
//======================================================================

#ifndef GRIT_COVER_BN_GFX_H
#define GRIT_COVER_BN_GFX_H

#define cover_bn_gfxTilesLen 21792
extern const bn::tile cover_bn_gfxTiles[681];

#define cover_bn_gfxMapLen 2048
extern const bn::regular_bg_map_cell cover_bn_gfxMap[1024];

#define cover_bn_gfxPalLen 32
extern const bn::color cover_bn_gfxPal[16];

#endif // GRIT_COVER_BN_GFX_H

//}}BLOCK(cover_bn_gfx)

namespace bn::regular_bg_items
{
    constexpr inline regular_bg_item cover(
            regular_bg_tiles_item(span<const tile>(cover_bn_gfxTiles, 681), bpp_mode::BPP_4, compression_type::NONE), 
            bg_palette_item(span<const color>(cover_bn_gfxPal, 16), bpp_mode::BPP_4, compression_type::NONE),
            regular_bg_map_item(cover_bn_gfxMap[0], size(32, 32), compression_type::NONE, 1, false));
}

#endif

