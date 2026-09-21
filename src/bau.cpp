#include "globals.h"
#include "bau.h"
#include "dog.h"
#include "collision.h"
#include "enemy.h"
#include "bn_sprite_items_bau.h"
#include "bn_sound_items.h"

bau::bau()
{
    sprite = bn::sprite_items::bau.create_sprite(0, 0, 0);
    sprite->set_visible(false);

    sprite->set_camera(g_camera);
    sprite->set_bg_priority(1);
}

void bau::do_spawn()
{
    if (g_bau->ticks > 0) return; // Bau già attivo, non spawnarne un altro
    g_bau->chr_x = g_dog->chr_x + bn::fixed(32).multiplication(g_dog->dir);
    g_bau->chr_y = g_dog->chr_y;
    g_bau->sprite->set_tiles(bn::sprite_items::bau.tiles_item(), g_dog->dir == DIR_LEFT ? 1 : 0);
    g_bau->chr_vx = bn::fixed(g_dog->dir == DIR_LEFT ? -1.5 : 1.5);
    g_bau->ticks = BAU_DURATA;
    bn::sound_items::bau.play();
}

void bau::update()
{
    // --- Bau vs nemici ---
    if (ticks > 0)
    {
        chr_x+=chr_vx;
        sprite->set_x(chr_x - HALF_SCREEN_W);
        sprite->set_y(chr_y - HALF_SCREEN_H);
        sprite->set_visible(ticks > BAU_FLICK_TIME || ticks % 2);

        for (enemy* enem : *g_enemies)
        {
            if (enem->invulnerability > 0) continue;
            bool hit = check_collision_16(*enem->sprite, *sprite);
            if (hit)
            {
                enem->beHitByBark(enem->chr_x < g_dog->chr_x ? DIR_LEFT : DIR_RIGHT);
                enem->invulnerability = ticks + 1;
            }
        }

        ticks--;
        if (!ticks)
            sprite->set_visible(false);
    }

}
