// src/piattaforma.cpp
#include "piattaforma.h"
#include "schemi.h"

piattaforma::piattaforma(const piattaforma_def& d, platform_map& buffer, bn::regular_bg_map_ptr mappa_bg) :
    def(d), _buffer(&buffer), _mappa_bg(mappa_bg)
{
    solida = (d.stato_iniziale == 1);
    applica(solida ? d.tile_hw_solido : d.tile_hw_vuoto);
    imposta_collisione_rect(d.tile_x1, d.tile_y1, d.tile_x2, d.tile_y2, solida ? 1 : 0);
}

void piattaforma::commuta()
{
    solida = !solida;
    applica(solida ? def.tile_hw_solido : def.tile_hw_vuoto);
    imposta_collisione_rect(def.tile_x1, def.tile_y1, def.tile_x2, def.tile_y2, solida ? 1 : 0);
}

void piattaforma::applica(const uint16_t tiles_hw[4])
{
    auto& mi = _buffer->map_item;

    for (int ty = def.tile_y1; ty <= def.tile_y2; ty++)
    {
        for (int tx = def.tile_x1; tx <= def.tile_x2; tx++)
        {
            int hx = tx * 2, hy = ty * 2;   // 1 tile di gioco (16px) = blocco 2x2 di celle hardware (8px)

            auto scrivi = [&](int ox, int oy, uint16_t tile_index)
            {
                bn::regular_bg_map_cell& cella = _buffer->cells[mi.cell_index(hx + ox, hy + oy)];
                bn::regular_bg_map_cell_info info(cella);
                info.set_tile_index(tile_index);
                cella = info.cell();
            };

            scrivi(0, 0, tiles_hw[0]);
            scrivi(1, 0, tiles_hw[1]);
            scrivi(0, 1, tiles_hw[2]);
            scrivi(1, 1, tiles_hw[3]);
        }
    }

    _mappa_bg.reload_cells_ref();
}