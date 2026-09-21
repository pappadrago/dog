#include "globals.h"
#include "bomb.h"
#include "dog.h"
#include "bn_math.h"
#include "bn_sprite_items_weapons.h"

bomb::bomb(bn::fixed x, bn::fixed y, bn::fixed vx, bn::fixed vy)
{
    chr_x = x;
    chr_y = y;
    chr_vx = vx;
    chr_vy = vy;

    box_dim = bn::fixed(8);
    box_halfdim = bn::fixed(4);

    sprite = bn::sprite_items::weapons.create_sprite(
        x - HALF_SCREEN_W, y - HALF_SCREEN_H, BOMB_TILE_INDEX);
    sprite->set_bg_priority(1);
    sprite->set_camera(g_camera);
}

void bomb::update()
{
    if (finished)
        return;

    if (exploded)
    {
        update_explosion();
        return;
    }

    age++;

    // --- Gravità + contatto col suolo ---
    apply_gravity(g_schema);

    if (onGround)
    {
        if (bounces >= BOMB_MAX_BOUNCES)
        {
            // terzo impatto: dopo i 2 rimbalzi la bomba esplode
            explode();
            return;
        }

        chr_vy = (bounces == 0) ? bn::fixed(-3.0) : bn::fixed(-2.0);
        chr_vx = chr_vx.multiplication(bn::fixed(0.75));
        bounces++;
    }

    // --- Movimento orizzontale (rimbalza contro i muri) ---
    bn::fixed old_vx = chr_vx;
    chr_x += chr_vx;
    apply_map(g_schema);
    if (chr_vx == bn::fixed(0) && old_vx != bn::fixed(0)) {
        chr_x -= old_vx;
        chr_vx = -old_vx.multiplication(bn::fixed(0.5));
    }

    // --- Sprite ---
    sprite->set_x(chr_x - HALF_SCREEN_W);
    sprite->set_y(chr_y - HALF_SCREEN_H);

    // dopo l'ultimo rimbalzo la bomba lampeggia (miccia accesa)
    sprite->set_visible(bounces < BOMB_MAX_BOUNCES || (age % 4) < 2);
}

void bomb::explode()
{
    exploded = true;
    explosion_ticks = BOMB_EXPLOSION_TICKS;
    chr_vx = bn::fixed(0);
    chr_vy = bn::fixed(0);

    // Danno al cane: stesso knockback usato per il contatto coi nemici
    if (g_dog->invulnerability == 0)
    {
        bn::fixed dx = g_dog->chr_x - chr_x;
        bn::fixed dy = (g_dog->chr_y + 6) - chr_y;   // hitbox del cane, come in collision.cpp

        if (bn::abs(dx) < BOMB_EXPLOSION_RADIUS + 8 &&
            bn::abs(dy) < BOMB_EXPLOSION_RADIUS + 13)
        {
            g_dog->chr_vx = (dx < 0) ? bn::fixed(-2.0) : bn::fixed(2.0);
            g_dog->chr_vy = bn::fixed(-3.0);
            g_dog->invulnerability = 60;
        }
    }
}

void bomb::update_explosion()
{
    explosion_ticks--;

    if (explosion_ticks <= 0)
    {
        sprite->set_visible(false);
        finished = true;
        return;
    }

    // Placeholder grafico: la bomba si ingrandisce e sfarfalla.
    // Sostituibile con un vero sprite/animazione di esplosione.
    bn::fixed elapsed(BOMB_EXPLOSION_TICKS - explosion_ticks);
    sprite->set_scale(bn::fixed(1) + elapsed.multiplication(bn::fixed(0.08)));
    sprite->set_visible(explosion_ticks % 3 != 0);
}
