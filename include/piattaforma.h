#pragma once
#include "bn_regular_bg_map_ptr.h"
#include "bn_regular_bg_map_cell_info.h"
#include "platform_map.h"
#include <cstdint>

struct piattaforma_def
{
    int16_t  tile_x1, tile_y1, tile_x2, tile_y2;
    uint16_t tile_hw_solido[4];
    uint16_t tile_hw_vuoto[4];
    uint8_t  stato_iniziale;
    uint8_t  attiva_con;
};

class piattaforma
{
public:
    piattaforma_def def;
    bool solida;

    piattaforma(const piattaforma_def& d, platform_map& buffer, bn::regular_bg_map_ptr mappa_bg);

    void commuta();   // avvia la transizione: non applica più tutto subito
    void update();    // da chiamare ogni frame: avanza la transizione, una tile alla volta

private:
    platform_map* _buffer;
    bn::regular_bg_map_ptr _mappa_bg;

    bool in_transizione = false;
    int  indice_tile_corrente = 0;
    int  ticks_prossima_tile = 0;

    void applica_tutte(const uint16_t tiles_hw[4]);
    void applica_una_tile(int tx, int ty, const uint16_t tiles_hw[4]);
};