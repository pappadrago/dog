#include "globals.h"
#include "dog.h"
#include "collision.h"
#include "enemy.h"
#include "bn_sprite_items_fox1632.h"
#include "bn_sound_items.h"

bau::bau()
{
    sprite = bn::sprite_items::fox1632.create_sprite(0, 0, 0);
    sprite->set_visible(false);

    sprite->set_camera(g_camera);
    sprite->set_bg_priority(1);
}

void bau::do_spawn(int charge)
{
    if (ticks > 0) return;

    bn::fixed x = g_dog->chr_x + bn::fixed(16).multiplication(g_dog->dir);
    bn::fixed bordo = x + box_halfdim.multiplication(bn::fixed(g_dog->dir));
    if (is_solid_at(bordo, g_dog->chr_y, g_schema))
        x = g_dog->chr_x;   // cane contro il muro: parte dalla sua posizione, non dentro la parete


    if (charge > BAU_CHARGE_MAX) charge = BAU_CHARGE_MAX;
    power = bn::fixed(charge).division(BAU_CHARGE_MAX);   // 0..1

    // se il cane è attaccato al muro, il bau parte in direzione opposta a quella del cane
    int bau_dir = (g_dog->muro_lato && g_dog->wall_sliding) ? -g_dog->dir : g_dog->dir;

    chr_x = g_dog->chr_x + bn::fixed(16).multiplication(bau_dir);
    chr_y = g_dog->chr_y;
    sprite->set_tiles(bn::sprite_items::fox1632.tiles_item(), charge < BAU_CHARGE_MAX / 2 ? 61 : 62);

    bn::fixed speed = bn::fixed(.5) + power.multiplication(bn::fixed(1.0));  // .5 -> 1.5
    chr_vx = speed.multiplication(bau_dir);

    ticks = BAU_DURATA + (power.multiplication(BAU_DURATA)).integer(); // 30 -> 60
    hit_half = bn::fixed(8) + power.multiplication(bn::fixed(6));         // 8 -> 14
    sprite->set_scale(bn::fixed(1) + power.multiplication(bn::fixed(0.75)));

    bn::sound_items::bau.play();
}


void bau::update()
{
    if (ticks > 0)
    {
        if (chr_vx != 0)
        {
            bn::fixed prossimo_x = chr_x + chr_vx;
            bn::fixed bordo = prossimo_x + (chr_vx > 0 ? box_halfdim : -box_halfdim);

            if (is_solid_at(bordo, chr_y, g_schema)) {
                chr_vx = 0;          // si ferma contro il muro
                ticks = 1;
            }
            else
                chr_x = prossimo_x;
        }
        sprite->set_x(chr_x - HALF_SCREEN_W);
        sprite->set_y(chr_y - HALF_SCREEN_H);
        sprite->set_visible(ticks > BAU_FLICK_TIME || ticks % 2);

        bn::fixed reach = hit_half + 8;   // 8 = mezza hitbox del nemico
        for (enemy* enem : *g_enemies)
        {
            if (enem->invulnerability > 0) continue;

            bool hit = bn::abs(enem->sprite->x() - sprite->x()) < reach &&
                bn::abs(enem->sprite->y() - sprite->y()) < reach;
            if (hit)
            {
                enem->beHitByBark(enem->chr_x < g_dog->chr_x ? DIR_LEFT : DIR_RIGHT, power);
                enem->invulnerability = ticks + 1;
            }
        }

        ticks--;
        if (!ticks)
            sprite->set_visible(false);
    }
}