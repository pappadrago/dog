#include "piattaforma.h"
#include "schemi.h"

static constexpr int TICKS_TRA_TILE = 30;   // 1 secondo a 60fps

piattaforma::piattaforma(const piattaforma_def& d, platform_map& buffer, bn::regular_bg_map_ptr mappa_bg) :
    def(d), _buffer(&buffer), _mappa_bg(mappa_bg)
{
    solida = (d.stato_iniziale == 1);
    applica_tutte(solida ? d.tile_hw_solido : d.tile_hw_vuoto);   // stato di partenza: istantaneo
    imposta_collisione_rect(d.tile_x1, d.tile_y1, d.tile_x2, d.tile_y2, solida ? 1 : 0);
}

void piattaforma::commuta()
{
    if (in_transizione) return;   // già in corso: ignora un secondo trigger mentre la prima gira

    solida = !solida;             // da qui in poi "solida" è lo stato di ARRIVO, non quello attuale sullo schermo
    in_transizione = true;
    indice_tile_corrente = 0;
    ticks_prossima_tile = 0;      // la prima tile scatta al prossimo update(), le successive ogni TICKS_TRA_TILE
}

void piattaforma::update()
{
    if (!in_transizione) return;

    if (ticks_prossima_tile > 0)
    {
        ticks_prossima_tile--;
        return;
    }

    int larghezza = def.tile_x2 - def.tile_x1 + 1;
    int altezza   = def.tile_y2 - def.tile_y1 + 1;
    int totale    = larghezza * altezza;

    int tx = def.tile_x1 + (indice_tile_corrente % larghezza);
    int ty = def.tile_y1 + (indice_tile_corrente / larghezza);

    applica_una_tile(tx, ty, solida ? def.tile_hw_solido : def.tile_hw_vuoto);

    indice_tile_corrente++;
    if (indice_tile_corrente >= totale)
        in_transizione = false;
    else
        ticks_prossima_tile = TICKS_TRA_TILE;
}

void piattaforma::applica_una_tile(int tx, int ty, const uint16_t tiles_hw[4])
{
    auto& mi = _buffer->map_item;
    int hx = tx * 2, hy = ty * 2;

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

    _mappa_bg.reload_cells_ref();
    imposta_collisione(tx, ty, solida ? 1 : 0);   // la collisione di QUESTA tile cambia nello stesso istante del suo aspetto
}

void piattaforma::applica_tutte(const uint16_t tiles_hw[4])
{
    for (int ty = def.tile_y1; ty <= def.tile_y2; ty++)
        for (int tx = def.tile_x1; tx <= def.tile_x2; tx++)
        {
            auto& mi = _buffer->map_item;
            int hx = tx * 2, hy = ty * 2;

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

    _mappa_bg.reload_cells_ref();
}