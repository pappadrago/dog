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
}

void bau::do_spawn()
{
    if (g_bau->ticks > 0) return; // Bau già attivo, non spawnarne un altro
    g_bau->chr_x = g_dog->chr_x + bn::fixed(32).multiplication(g_dog->dir);
    g_bau->chr_y = g_dog->chr_y;
    g_bau->sprite->set_tiles(bn::sprite_items::bau.tiles_item(), g_dog->dir == DIR_LEFT ? 1 : 0);
    g_bau->ticks = 120;
    bn::sound_items::bau.play();
}

void bau::update()
{
    // --- Bau vs nemici ---
    if (ticks > 0)
    {
        sprite->set_x(chr_x - HALF_SCREEN_W);
        sprite->set_y(chr_y - HALF_SCREEN_H);
        sprite->set_visible(ticks > 60 || ticks % 2);


        for (enemy* enem : *g_enemies)
        {
            if (enem->invulnerability > 0) continue;
            bool hit = check_collision_16(*enem->sprite, *sprite);
            if (hit)
            {
                enem->bounce(enem->chr_x < chr_x ? DIR_LEFT : DIR_RIGHT);
            }
        }

        ticks--;
        if (!ticks) sprite->set_visible(false);
    }

}
