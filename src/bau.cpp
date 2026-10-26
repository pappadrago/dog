#include "globals.h"
#include "dog.h"
#include "collision.h"
#include "enemy.h"
#include "bn_sprite_items_fox1632.h"
#include "bn_log.h"
#include "cassa.h"
#include "trampolino.h"
#include "bn_sound_items.h"

bau::bau()
{
    sprite = bn::sprite_items::fox1632.create_sprite(0, 0, 0);
    sprite->set_camera(g_camera);
    sprite->set_bg_priority(1);
    sprite->set_visible(false);
}

void bau::do_spawn(int charge)
{
    if (ticks > 0 || ticks2Anim > 0) return;

    if (charge >= BAU_CHARGE_MAX) {
        charge = BAU_CHARGE_MAX;
        bn::sound_items::bau.play();
    }
    power = bn::fixed(charge).division(BAU_CHARGE_MAX);   // 0..1

    // se il cane è attaccato al muro, il bau parte in direzione opposta a quella del cane
    int bau_dir = (g_dog->muro_lato && g_dog->wall_sliding) ? -g_dog->dir : g_dog->dir;

    chr_x = g_dog->chr_x + bn::fixed(8).multiplication(bau_dir);
    chr_y = g_dog->chr_y;
    sprite->set_tiles(bn::sprite_items::fox1632.tiles_item(), charge < BAU_CHARGE_MAX / 2 ? 61 : 62);

    chr_vx = (4.0 + power.multiplication(4.0)).multiplication(bau_dir);
    ticks = 30;

    hit_half = bn::fixed(8) + power.multiplication(bn::fixed(6));         // 8 -> 14
    sprite->set_scale(bn::fixed(1) + power.multiplication(bn::fixed(0.75)));

    sprite->set_visible(true);
}
