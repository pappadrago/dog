#include "globals.h"
#include "bomb.h"
#include "dog.h"
#include "bn_math.h"
#include "bn_sprite_items_weapons.h"
#include "bn_sprite_items_diecloud.h"
#include "collision.h"


BN_CODE_IWRAM void bomb::update()
{
    if (finished)
        return;

    if (exploded)
    {
        update_explosion();
        return;
    }

    int lifetime = 0;
    switch (tipoProjec) {
    case BOMB_DAGGER:
        lifetime = BOMB_DAGGER_LIFETIME;
        break;
    case BOMB_ARROW:
    case BOMB_FIREBALL:
        lifetime = BOMB_PROJECTILE_LIFETIME;
        break;
    case BOMB_MINA:
        lifetime = BOMB_MINA_LIFETIME;
        break;
    default:
        break;
    }

    if (lifetime > 0 && ticks2action >= lifetime - 1) {
        if (tipoProjec == BOMB_MINA)
            explode();
        else
            hide_and_finish();
        return;
    }
    ticks2action++;

    // --- Gravità + contatto col suolo ---
    if (!ignoreGravity) {
        apply_gravity();

        if (tipoProjec == BOMB_MINA && atterrato_ora)
        {
            if (velocita_atterraggio <= bn::fixed(1))
            {
                chr_vx = chr_vy = ZERO;
            }
            else
            {
                chr_vy = -velocita_atterraggio.multiplication(bn::fixed(0.5));
                chr_vx = chr_vx.multiplication(bn::fixed(0.75));
                bounces++;
            }
        }
        else if (onGround && tipoProjec == BOMB_BOMB)
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
    if (ignoreGravity)
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
        switch (tipoProjec) {
        case BOMB_MINA:
            if (lifetime - ticks2action > BOMB_MINA_FUSE_TICKS) {
                ticks2action = lifetime - BOMB_MINA_FUSE_TICKS;
            }
            break;
        case BOMB_BOMB:
            explode(); // dopo l'animazione di esplosione, si nasconderà da solo
            g_dog->dog_damage_and_jump(15, g_dog->chr_x < chr_x);
            break;
        default:
            g_dog->dog_damage_and_jump(3, g_dog->chr_x < chr_x);
            if (tipoProjec != BOMB_DAGGER) { // se non è una spada, nascondi
                hide_and_finish();
            }
            break;
        }

    }


    // --- Sprite ---
    sprite->set_x(chr_x - HALF_SCREEN_W);
    sprite->set_y(chr_y - HALF_SCREEN_H);

    if (tipoProjec == BOMB_DAGGER) {
        sprite->set_rotation_angle_safe(-dir * ticks2action << 4);
    }

    // dopo l'ultimo rimbalzo la bomba lampeggia (miccia accesa)
    int remaining_lifetime = lifetime - ticks2action;
    if (remaining_lifetime > 0 && remaining_lifetime < BOMB_MINA_FUSE_TICKS) {
        int blink_period = (1 + remaining_lifetime / 10) << 1;
        sprite->set_visible((ticks2action % blink_period) < (blink_period >> 1));
    }
    else if (tipoProjec != BOMB_MINA)
        sprite->set_visible(bounces < BOMB_MAX_BOUNCES || (ticks2action % 4) < 2);

}
