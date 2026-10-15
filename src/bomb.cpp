#include "globals.h"
#include "bomb.h"
#include "dog.h"
#include "bn_math.h"
#include "bn_sprite_items_weapons.h"
#include "bn_sprite_items_diecloud.h"
#include "collision.h"

bomb::bomb(bn::fixed x, bn::fixed y, bn::fixed vx, bn::fixed vy, int _max_bounces) :
    max_bounces(_max_bounces)
{
    chr_x = x;
    chr_y = y;
    chr_vx = vx;
    chr_vy = vy;

    box_dim = bn::fixed(8);
    box_halfdim = bn::fixed(4);

    sprite = bn::sprite_items::diecloud.create_sprite(x - HALF_SCREEN_W, y - HALF_SCREEN_H, 0);
    sprite->set_bg_priority(1);
    sprite->set_camera(g_camera);

    int B = 9;
    actionExplode = bn::create_sprite_animate_action_once(
        *sprite, BOMB_EXPLOSION_FRAME_TICKS, bn::sprite_items::diecloud.tiles_item(), B + 0, B + 1, B + 2, B + 3, B + 4, B + 5, B + 6, B + 7, B + 8);

    tipoProjec = BOMB_BOMB;
}

bomb::bomb(const bomb_def& def)
{
    tipoProjec = def.tipoProjec;
    chr_x = def.x;
    chr_y = def.y;
    chr_vx = def.vx;
    chr_vy = def.vy;
    max_bounces = def.max_bounces;
    ignoreWalls = def.ignoreWalls;
    ignoreGravity = def.ignoreGravity;
    ttl = def.ttl;

    box_dim = bn::fixed(8);
    box_halfdim = bn::fixed(4);

    sprite = bn::sprite_items::weapons.create_sprite(chr_x - HALF_SCREEN_W, chr_y - HALF_SCREEN_H, def.tileIndex);

    if (def.angle != 0)
        sprite->set_rotation_angle_safe(def.angle);
    sprite->set_bg_priority(0);
    sprite->set_camera(g_camera);

    int B = 9;
    actionExplode = bn::create_sprite_animate_action_once(
        *sprite, BOMB_EXPLOSION_FRAME_TICKS, bn::sprite_items::diecloud.tiles_item(), B + 0, B + 1, B + 2, B + 3, B + 4, B + 5, B + 6, B + 7, B + 8);
}

namespace bomb_config {
    constexpr bn::fixed GROUND_BOUNCE_DAMPING = bn::fixed(0.75); // energia persa al rimbalzo al suolo
    constexpr bn::fixed WALL_BOUNCE_DAMPING = bn::fixed(0.5);  // energia persa al rimbalzo laterale
    constexpr int         DOG_INVULNERABILITY = 60;             // frame di i-frames dopo un colpo
    constexpr int         DOG_Damage = 3;              // danno base della bomba
    constexpr int         FUSE_BLINK_TICKS = 30;             // soglia "miccia accesa"
}

void bomb::hide_and_finish()
{

    sprite->set_visible(false);
    sprite->remove_affine_mat();

    finished = true;
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
    if (ttl > 0) {
        ttl--;
        if (ttl == 0) {
            hide_and_finish();
            return;
        }
    }
    age++;

    // --- Gravità + contatto col suolo ---
    if (!ignoreGravity) {
        apply_gravity();

        if (onGround)
        {
            if (bounces >= max_bounces)
            {
                // terzo impatto: dopo i 2 rimbalzi la bomba esplode
                explode();
                return;
            }

            chr_vy = (bounces == 0) ? ENEMY_MEDIUM_JUMP_VY : ENEMY_SMALL_JUMP_VY;
            chr_vx = chr_vx.multiplication(bn::fixed(0.75));
            bounces++;
        }
    }

    // --- Movimento orizzontale (rimbalza contro i muri) ---
    bn::fixed old_vx = chr_vx;
    chr_x += chr_vx;
    chr_y += chr_vy;
    if (!ignoreWalls) {
        apply_map();
        if (chr_vx == bn::fixed(0) && old_vx != bn::fixed(0)) {
            chr_y -= chr_vy;
            chr_vy = ZERO;
            if (max_bounces > 0) {
                chr_x -= old_vx;
                chr_vx = -old_vx.multiplication(bn::fixed(0.5));
            }
        }
    }


    bool hit = check_collision_16(*sprite);
    if (hit)
    {
        g_dog->chr_vx = (g_dog->chr_x < chr_x) ? SMALL_VX_L : SMALL_VX_R;
        g_dog->chr_vy = SMALL_JUMP_VY;
        g_dog->invulnerability = 60;
        if (tipoProjec != BOMB_DAGGER) {
            hide_and_finish();
        }
        g_dog->dog_damage(3);
    }


    // --- Sprite ---
    sprite->set_x(chr_x - HALF_SCREEN_W);
    sprite->set_y(chr_y - HALF_SCREEN_H);

    if (tipoProjec == BOMB_DAGGER) {
        sprite->set_rotation_angle_safe(-dir * age << 4);
    }

    // dopo l'ultimo rimbalzo la bomba lampeggia (miccia accesa)
    if (ttl > 0 && ttl < 30)
        sprite->set_visible(ttl % 2);
    else
        sprite->set_visible(bounces < BOMB_MAX_BOUNCES || (age % 4) < 2);

}

void bomb::explode()
{
    exploded = true;
    explosion_ticks = BOMB_EXPLOSION_TICKS;
    screen_shake(GROUND_POUND_SHAKE_TICKS, GROUND_POUND_SHAKE_AMP);
    chr_vx = chr_vy = ZERO;
    sprite->set_visible(true);
    actionExplode->reset();
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

    if (!actionExplode->done()) {
        actionExplode->update();
    }

    // Danno al cane: stesso knockback usato per il contatto coi nemici
    if (g_dog->invulnerability == 0)
    {
        bn::fixed dx = g_dog->chr_x - chr_x;
        bn::fixed dy = (g_dog->chr_y + 6) - chr_y;   // hitbox del cane, come in collision.cpp

        if (bn::abs(dx) < BOMB_EXPLOSION_RADIUS + 8 &&
            bn::abs(dy) < BOMB_EXPLOSION_RADIUS + 13)
        {
            g_dog->chr_vx = (dx < 0) ? bn::fixed(-2.0) : bn::fixed(2.0);
            g_dog->chr_vy = MEDIUM_JUMP_VY;
            g_dog->invulnerability = 60;
        }
    }

}
