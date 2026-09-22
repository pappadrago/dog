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
#include "bn_sprite_items_dog.h"
#include "bn_sprite_items_dog2.h"
#include "bn_sprite_items_dog3.h"
#include "bn_sprite_items_dog4.h"
#include "bn_sprite_items_dog5.h"
#include "bn_sprite_items_bau.h"
#include "bn_log.h"
#include "schemi.h"

void live_obj::apply_friction()
{
    if (chr_vx > 0) {
        chr_vx -= onGround ? GROUND_FRICTION : AIR_FRICTION;
        if (chr_vx < bn::fixed(0)) chr_vx = bn::fixed(0);
    }
    else if (chr_vx < 0) {
        chr_vx += onGround ? GROUND_FRICTION : AIR_FRICTION;
        if (chr_vx > bn::fixed(0)) chr_vx = bn::fixed(0);
    }
    if (bn::abs(chr_vx) < bn::fixed(0.01)) {
        chr_vx = bn::fixed(0);
    }

}

void live_obj::apply_gravity(int schema)
{

    // --- Fisica verticale ---
    const collision_map_info& map = get_collision_map(schema);
    {
        chr_vy += GRAVITY;
        onGround = false;
        if (chr_vy > MAX_FALL) chr_vy = MAX_FALL;
        chr_y += chr_vy;

        // Legge tile mappa sotto il cane 
        int tx1 = (chr_x.integer() + box_halfdim.integer()) >> 4;
        int tx2 = (chr_x.integer() - box_halfdim.integer()) >> 4;

        if (tx1 < 0) tx1 = 0;
        if (tx2 < 0) tx2 = 0;
        if (tx1 > map.columns - 1) tx1 = map.columns - 1;
        if (tx2 > map.columns - 1) tx2 = map.columns - 1;


        u_int16_t ty = ((chr_y + (chr_vy > 0 ? box_dim : bn::fixed(0.0))).division(16.0)).integer();

        uint8_t tile_dwn1 = map.data[ty * map.columns + tx1];
        uint8_t tile_dwn2 = map.data[ty * map.columns + tx2];

        if ((tile_dwn1 == 1 || tile_dwn2 == 1))
        {
            chr_y = bn::fixed(ty << 4) + (chr_vy > 0 ? -box_dim : box_dim);

            u_int16_t y = (ty * 16) + (chr_vy > 0 ? -box_dim.floor_integer() : box_dim.floor_integer());

            if (chr_vy > 0)
                onGround = true;
            chr_vy = 0;
            chr_y = bn::fixed(y);
        }
    }

}

void live_obj::apply_map(int schema) {
    // check se sbatto contro muro a sx o dx
    const collision_map_info& map = get_collision_map(schema);
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
            chr_vx = bn::fixed(0);
        }
    }


    if (chr_x <= 0) { chr_x = bn::fixed(0);chr_vx = bn::fixed(0); chr_accx = bn::fixed(0); }
    if (chr_x >= bn::fixed(map.columns<<4)) { chr_x = bn::fixed(map.columns<<4);chr_vx = bn::fixed(0); chr_accx = bn::fixed(0); }



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