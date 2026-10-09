#include "lava.h"
#include "globals.h"
#include "dog.h"
#include "enemy.h"
#include "bn_regular_bg_map_cell_info.h"
#include "bn_log.h"

lava_zona::lava_zona(const lava_zona_def& d, platform_map& buffer, bn::regular_bg_map_ptr mappa_bg) :
    def(d), _buffer(&buffer), _mappa_bg(mappa_bg)
{
    ticks = d.velocita;
    applica(d.frame_hw[0]);
}

void lava_zona::applica(const uint16_t tiles_hw[4])
{
    auto& mi = _buffer->map_item;

    for (int ty = def.tile_y1; ty <= def.tile_y2; ty++)
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

    _mappa_bg.reload_cells_ref();
}

bool lava_zona::dentro(bn::fixed x, bn::fixed y) const
{
    bn::fixed x1 = bn::fixed(def.tile_x1 * 16);
    bn::fixed y1 = bn::fixed(def.tile_y1 * 16);
    bn::fixed x2 = bn::fixed(def.tile_x2 * 16 + 16);
    bn::fixed y2 = bn::fixed(def.tile_y2 * 16 + 16);

    return x > x1 && x < x2 && y > y1 && y < y2;
}

bool lava_zona::dog_dentro() const
{
    return dentro(g_dog->chr_x, g_dog->chr_y + 6);
}

void lava_zona::update()
{
    if (ticks > 0)
        ticks--;
    else
    {
        frame_corrente = (frame_corrente + 1) % def.num_frame;
        applica(def.frame_hw[frame_corrente]);
        ticks = def.velocita;
    }

    bool dog_inside = dog_dentro();
    if (dog_inside && g_dog->invulnerability == 0)
    {
        g_dog->chr_vy = DOG_RIMBALZO_LAVA;
        g_dog->dir = g_rng.get_bool() ? DIR_LEFT : DIR_RIGHT;
        g_dog->invulnerability = 60;
        g_dog->dog_damage(def.danno);
    }

    for (enemy* e : *g_enemies)
    {
        if (e->distrutto) continue;
        if (dentro(e->chr_x, e->chr_y + 6))
        {
            e->distrutto = true;
            if (e->sprite) e->sprite->set_visible(false);
            if (e->weaponSprite) e->weaponSprite->set_visible(false);
        }
    }
}