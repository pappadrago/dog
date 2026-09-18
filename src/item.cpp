#include "globals.h"
#include "item.h"
#include "bau.h"
#include "dog.h"
#include "bn_random.h"
#include "bn_sprite_items_items.h"
#include "bn_math.h"
#include "bn_log.h"

item::item()
{

    int N = g_rng.get_int(10);

    sprite = bn::sprite_items::items.create_sprite(chr_x, chr_y, N);

    sprite->set_bg_priority(1);
    sprite->set_camera(g_camera);

    chr_x = g_rng.get_int(MAP_W);
    chr_y = 512 - 200;
    do_spawn();
    box_dim = bn::fixed(8);
    box_halfdim = bn::fixed(4);

    action = ACTION_MOVING;
}


void item::do_spawn()
{
    chr_x = g_rng.get_int(MAP_W);
    chr_y = 200;
}

void item::update()
{
    apply_gravity();
    chr_x += chr_vx;
    if (chr_vx > 0) {
        chr_vx += FRICTION;
        if (chr_vx < bn::fixed(0)) chr_vx = bn::fixed(0);
    }
    else if (chr_vx < 0) {
        chr_vx -= FRICTION;
        if (chr_vx > bn::fixed(0)) chr_vx = bn::fixed(0);
    }
    if (bn::abs(chr_vx) < bn::fixed(0.01)) {
        chr_vx = bn::fixed(0);
    }
    apply_map();

    ticks2action++;

    bn::fixed dy = bn::degrees_lut_sin_safe(ticks2action << 3) * 4 - 2;
    bn::fixed dx = bn::degrees_lut_cos_safe(ticks2action << 2) * 4 - 2;
    sprite->set_x(chr_x + dx - HALF_SCREEN_W);
    sprite->set_y(chr_y + dy - HALF_SCREEN_H);

    if (invulnerability > 0)
        invulnerability--;

    if (invulnerability == 1) {
        do_spawn();
    }

    if (invulnerability > 0)
        sprite->set_visible(invulnerability % 2);
    else
        sprite->set_visible(true);
}


void item::bounce(int _dir)
{
    dir = _dir;
    chr_vx = bn::fixed(2.0).multiplication(dir);
    chr_vy = bn::fixed(-3.0);

    ticks2action = 240;
    invulnerability = 60;
}

