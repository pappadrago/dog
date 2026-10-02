// include/piattaforma.h
#pragma once
#include "bn_regular_bg_map_ptr.h"
#include "bn_regular_bg_map_cell_info.h"
#include "platform_map.h"
#include <cstdint>

struct piattaforma_def
{
    int16_t  tile_x1, tile_y1, tile_x2, tile_y2;   // rettangolo in tile di gioco (16px)
    uint16_t tile_hw_solido[4];   // 4 indici hardware (8px): alto-sx, alto-dx, basso-sx, basso-dx
    uint16_t tile_hw_vuoto[4];    // idem, aspetto "trasparente"
    uint8_t  stato_iniziale;      // 0 = comincia trasparente, 1 = comincia solida
    uint8_t  attiva_con;          // sotto_tipo dell'item ITEM_TIPO_SBLOCCO che la commuta
};

class piattaforma
{
public:
    piattaforma_def def;
    bool solida;

    piattaforma(const piattaforma_def& d, platform_map& buffer, bn::regular_bg_map_ptr mappa_bg);
    void commuta();

private:
    platform_map* _buffer;
    bn::regular_bg_map_ptr _mappa_bg;
    void applica(const uint16_t tiles_hw[4]);
};