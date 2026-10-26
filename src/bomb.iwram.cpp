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

        if (onGround && tipoProjec == BOMB_BOMB)
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
        g_dog->chr_vx = (g_dog->chr_x < chr_x) ? SMALL_VX_L : SMALL_VX_R;
        g_dog->chr_vy = SMALL_JUMP_VY;
        g_dog->invulnerability = 60;

        if (tipoProjec == BOMB_BOMB){
            explode(); // dopo l'animazione di esplosione, si nasconderà da solo
            g_dog->dog_damage(15);
        }
        else {
            g_dog->dog_damage(3);
            if (tipoProjec != BOMB_DAGGER) { // se non è una spada, nascondi
                hide_and_finish();
            }
        }
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
