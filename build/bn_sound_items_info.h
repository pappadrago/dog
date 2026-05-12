#ifndef BN_SOUND_ITEMS_INFO_H
#define BN_SOUND_ITEMS_INFO_H

#include "bn_span.h"
#include "bn_sound_item.h"
#include "bn_string_view.h"

namespace bn::sound_items_info
{
    constexpr inline pair<sound_item, string_view> array[] = {
        make_pair(sound_item(0), string_view("bomb")),
        make_pair(sound_item(1), string_view("coin")),
        make_pair(sound_item(2), string_view("die")),
        make_pair(sound_item(3), string_view("hit")),
        make_pair(sound_item(4), string_view("jump")),
        make_pair(sound_item(5), string_view("milestone")),
        make_pair(sound_item(33), string_view("shot_01")),
    };

    constexpr inline span<const pair<sound_item, string_view>> span(array);
}

#endif

