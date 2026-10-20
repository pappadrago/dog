#ifndef ROUND_6X6_SPRITE_FONT_H
#define ROUND_6X6_SPRITE_FONT_H

#include "bn_sprite_font.h"
#include "bn_utf8_characters_map.h"
#include "bn_sprite_items_font.h"

namespace round_font
{

// Nessun carattere UTF-8 extra (il font di partenza ha solo ASCII 32..126)
constexpr bn::span<const bn::utf8_character> utf8_characters_span;
constexpr auto utf8_characters_map = bn::utf8_characters_map<utf8_characters_span>();

constexpr int8_t character_widths[] = {
    4,  // 32  
    4,  // 33 !
    6,  // 34 "
    6,  // 35 #
    5,  // 36 $
    6,  // 37 %
    4,  // 38 &
    2,  // 39 '
    6,  // 40 (
    6,  // 41 )
    6,  // 42 *
    6,  // 43 +
    3,  // 44 ,
    6,  // 45 -
    3,  // 46 .
    6,  // 47 /
    6,  // 48 0
    6,  // 49 1
    6,  // 50 2
    6,  // 51 3
    6,  // 52 4
    6,  // 53 5
    6,  // 54 6
    6,  // 55 7
    6,  // 56 8
    6,  // 57 9
    3,  // 58 :
    6,  // 59 ;
    6,  // 60 <
    6,  // 61 =
    6,  // 62 >
    5,  // 63 ?
    6,  // 64 @
    6,  // 65 A
    6,  // 66 B
    6,  // 67 C
    6,  // 68 D
    6,  // 69 E
    6,  // 70 F
    6,  // 71 G
    6,  // 72 H
    6,  // 73 I
    6,  // 74 J
    6,  // 75 K
    6,  // 76 L
    6,  // 77 M
    6,  // 78 N
    6,  // 79 O
    6,  // 80 P
    6,  // 81 Q
    6,  // 82 R
    6,  // 83 S
    6,  // 84 T
    6,  // 85 U
    6,  // 86 V
    6,  // 87 W
    6,  // 88 X
    6,  // 89 Y
    6,  // 90 Z
    6,  // 91 [
    6,  // 92
    6,  // 93 ]
    6,  // 94 ^
    6,  // 95 _
    6,  // 96 `
    5,  // 97 a
    5,  // 98 b
    5,  // 99 c
    5,  // 100 d
    5,  // 101 e
    5,  // 102 f
    5,  // 103 g
    5,  // 104 h
    2,  // 105 i
    5,  // 106 j
    5,  // 107 k
    4,  // 108 l
    5,  // 109 m
    5,  // 110 n
    5,  // 111 o
    5,  // 112 p
    5,  // 113 q
    5,  // 114 r
    5,  // 115 s
    4,  // 116 t
    5,  // 117 u
    5,  // 118 v
    5,  // 119 w
    5,  // 120 x
    5,  // 121 y
    5,  // 122 z
    6,  // 123 {
    6,  // 124 |
    6,  // 125 }
    6,  // 126 ~
};

constexpr bn::sprite_font round_6x6_sprite_font(
        bn::sprite_items::font, utf8_characters_map.reference(), character_widths);

}

#endif
