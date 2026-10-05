#pragma once
#include "platform_map.h"
#include "bn_regular_bg_map_ptr.h"
#include <cstdint>

#define LAVA_TILE 13

struct lava_zona_def
{
    int16_t  tile_x1, tile_y1, tile_x2, tile_y2;   // rettangolo in tile di gioco (16px)
    uint16_t frame_hw[3][4];                        // fino a 3 frame, ciascuno coi 4 indici hardware (TL,TR,BL,BR)
    uint8_t  num_frame;
    uint8_t  velocita;                              // tick tra un frame e il successivo
    int16_t  danno;                                 // danno al cane per contatto
};

class lava_zona
{
public:
    lava_zona_def def;
    uint8_t frame_corrente = 0;
    int ticks;

    lava_zona(const lava_zona_def& d, platform_map& buffer, bn::regular_bg_map_ptr mappa_bg);
    void update();

private:
    platform_map* _buffer;
    bn::regular_bg_map_ptr _mappa_bg;
    void applica(const uint16_t tiles_hw[4]);
    bool dog_dentro() const;
};