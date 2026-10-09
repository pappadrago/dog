#include "obj.h"
#include "globals.h"
#include "dog.h"
#include "bau.h"
#include "enemy.h"
#include "collision.h"
#include "bn_keypad.h"
#include "bn_math.h"
#include "bn_random.h"
#include "bn_sound_items.h"
#include "bn_log.h"
#include "schemi.h"

void live_obj::apply_friction()
{
    if (chr_vx > ZERO) {
        chr_vx -= onGround ? GROUND_FRICTION : AIR_FRICTION;
        if (chr_vx < ZERO) chr_vx = ZERO;
    }
    else if (chr_vx < ZERO) {
        chr_vx += onGround ? GROUND_FRICTION : AIR_FRICTION;
        if (chr_vx > ZERO) chr_vx = ZERO;
    }
    if (bn::abs(chr_vx) < bn::fixed(0.01)) {
        chr_vx = ZERO;
    }
}

void live_obj::apply_gravity()
{
    const collision_map_info& map = get_collision_map(g_schema);
    {
        atterrato_ora = false;
        testata_ora = false;
        chr_vy += GRAVITY;
        onGround = false;
        if (chr_vy > MAX_FALL)
            chr_vy = MAX_FALL;
        chr_y += chr_vy;

        int cx = chr_x.integer();
        int bh = box_halfdim.integer();

        int tx1 = (cx + bh) >> 4;
        int tx2 = (cx - bh) >> 4;

        if (tx1 < 0) tx1 = 0;
        if (tx2 < 0) tx2 = 0;
        if (tx1 > map.columns - 1) tx1 = map.columns - 1;
        if (tx2 > map.columns - 1) tx2 = map.columns - 1;

        bool falling = chr_vy > 0;

        // Punto di controllo verticale: piedi se scendo, testa se salgo
        int ty = (chr_y + (falling ? box_dim : -box_dim)).integer() >> 4;
        if (ty < 0) ty = 0;
        if (ty > map.rows - 1) ty = map.rows - 1;

        int row = ty * map.columns;
        uint8_t tile_dwn1 = map.data[row + tx1];
        uint8_t tile_dwn2 = map.data[row + tx2];

        if (tile_dwn1 == 1 || tile_dwn2 == 1)
        {
            if (chr_vy > 0)
            {
                // Atterraggio: i piedi si fermano sul bordo SUPERIORE della tile
                chr_y = bn::fixed(ty << 4) - box_dim;
                onGround = true;

                atterrato_ora = true;
                velocita_atterraggio = chr_vy;

            }
            else
            {
                // Testata: la testa si ferma sul bordo INFERIORE della tile
                chr_y = bn::fixed((ty << 4) + 16) + box_halfdim;
                testata_ora = true;
            }
            if(chr_vy != 0) {
             salto_in_corso = false;
            }
            chr_vy = ZERO;
            
        }
    }
}



void live_obj::apply_map() {
    // check se sbatto contro muro a sx o dx
    const collision_map_info& map = get_collision_map(g_schema);
    {
        int tx = (chr_x.integer() + (chr_vx > 0 ? box_halfdim.integer() : -box_halfdim.integer())) >> 4;

        if (tx < 0) tx = 0;
        if (tx > map.columns - 1) tx = map.columns - 1;

        int ty1 = ((chr_y).integer() + box_halfdim.integer()) >> 4;
        int ty2 = ((chr_y).integer() - 0) >> 4;

        uint8_t tile_side1 = map.data[ty1 * map.columns + tx];
        uint8_t tile_side2 = map.data[ty2 * map.columns + tx];
        if ((tile_side1 == 1 || tile_side2 == 1))
        {
            chr_x -= chr_vx;
            chr_vx = ZERO;
        }
    }


    if (chr_x <= 0) { chr_x = chr_vx = chr_accx = ZERO; }
    if (chr_x >= bn::fixed(map.columns << 4)) { chr_x = bn::fixed(map.columns << 4);chr_vx = chr_accx = ZERO; }

    if (chr_x <= 0) { chr_x = chr_vx = chr_accx = ZERO; }
    if (chr_x >= bn::fixed(map.columns << 4)) { chr_x = bn::fixed(map.columns << 4); chr_vx = ZERO; chr_accx = ZERO; }

    // Rileva un muro adiacente indipendentemente dalla direzione di marcia: serve al wall jump,
    // che deve valere anche restando fermi contro il muro, non solo premendo verso di esso.
// src/obj.cpp, dentro apply_map(), al posto del blocco muro_lato che avevi
    muro_lato = 0;
    {
        int cx = chr_x.integer();
        int bh = box_halfdim.integer();
        int margine = bh + 2;   // tolleranza: il resting point reale non è sempre perfettamente a contatto

        int txd = (cx + margine) >> 4;
        int txs = (cx - margine) >> 4;
        if (txd < 0) txd = 0; 
        if (txd > map.columns - 1) txd = map.columns - 1;
        if (txs < 0) txs = 0; 
        if (txs > map.columns - 1) txs = map.columns - 1;

        int ty1 = (chr_y.integer() + bh) >> 4;
        int ty2 = (chr_y.integer() - 0) >> 4;
        if (ty1 < 0) ty1 = 0; 
        if (ty1 > map.rows - 1) ty1 = map.rows - 1;
        if (ty2 < 0) ty2 = 0; 
        if (ty2 > map.rows - 1) ty2 = map.rows - 1;

        bool muro_destra = map.data[ty1 * map.columns + txd] == 1 || map.data[ty2 * map.columns + txd] == 1;
        bool muro_sinistra = map.data[ty1 * map.columns + txs] == 1 || map.data[ty2 * map.columns + txs] == 1;

        if (muro_destra)        muro_lato = DIR_RIGHT;
        else if (muro_sinistra) muro_lato = DIR_LEFT;

    }
}



bool is_solid_at(bn::fixed x, bn::fixed y, int schema)
{
    const collision_map_info& map = get_collision_map(schema);

    if (x < 0 || y < 0)
        return true;

    int tx = x.integer() >> 4;
    int ty = y.integer() >> 4;

    if (tx >= map.columns || ty >= map.rows)
        return true;

    return map.data[ty * map.columns + tx] == 1;
}