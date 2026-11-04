#include "globals.h"
#include "bomb.h"
#include "dog.h"
#include "bn_math.h"
#include "bn_sprite_items_weapons.h"
#include "bn_sprite_items_diecloud.h"
#include "collision.h"
#include <bn_sound_items.h>

bomb::bomb(const bomb_def& def)
{
    tipoProjec = def.tipoProjec;
    chr_x = def.x;
    chr_y = def.y;
    chr_vx = def.vx;
    chr_vy = def.vy;
    max_bounces = def.max_bounces;
    ignoreGravity = def.ignoreGravity;
    ignoreWalls = def.ignoreWalls;

    box_dim = bn::fixed(8);
    box_halfdim = bn::fixed(4);

    if (tipoProjec == BOMB_MINA || tipoProjec == BOMB_BOMB || tipoProjec == BOMB_FULMINE)
        sprite = bn::sprite_items::diecloud.create_sprite(chr_x - HALF_SCREEN_W, chr_y - HALF_SCREEN_H, 0);
    else
        sprite = bn::sprite_items::weapons.create_sprite(chr_x - HALF_SCREEN_W, chr_y - HALF_SCREEN_H, def.tileIndex);

    if (def.angle != 0)
        sprite->set_rotation_angle_safe(def.angle);
    else if (tipoProjec != BOMB_BOMB)
        sprite->set_horizontal_flip(chr_vx < 0);
    sprite->set_bg_priority(tipoProjec == BOMB_BOMB ? 1 : 0);
    sprite->set_camera(g_camera);

    int B = 9;
    actionExplode = bn::create_sprite_animate_action_once(
        *sprite, BOMB_EXPLOSION_FRAME_TICKS, bn::sprite_items::diecloud.tiles_item(), B + 0, B + 1, B + 2, B + 3, B + 4, B + 5, B + 6, B + 7, B + 8);
}

bomb* spawn_bomb(const bomb_def& def, int direction)
{
    if (!g_bombs || g_bombs->full())
        return nullptr;

    bomb* projectile = new bomb(def);
    if (direction != 0)
        projectile->dir = direction;
    g_bombs->push_back(projectile);
    return projectile;
}

namespace bomb_config {
    constexpr bn::fixed GROUND_BOUNCE_DAMPING = bn::fixed(0.75); // energia persa al rimbalzo al suolo
    constexpr bn::fixed WALL_BOUNCE_DAMPING = bn::fixed(0.5);  // energia persa al rimbalzo laterale
    constexpr int         DOG_INVULNERABILITY = 60;             // frame di i-frames dopo un colpo
    constexpr int         DOG_DAMAGE = 3;              // danno base della bomba
    constexpr int         FUSE_BLINK_TICKS = 30;             // soglia "miccia accesa"
}

void bomb::hide_and_finish()
{

    sprite->set_visible(false);
    sprite->remove_affine_mat();

    finished = true;
}

void bomb::explode()
{
    exploded = true;
    screen_shake(GROUND_POUND_SHAKE_TICKS, GROUND_POUND_SHAKE_AMP);
    chr_vx = chr_vy = ZERO;
    sprite->set_visible(true);
    actionExplode->reset();
    bn::sound_items::bomb.play();
}

void bomb::update_explosion()
{
    if (!actionExplode->done()) {
        actionExplode->update();
    }

    if (actionExplode->done())
    {
        sprite->set_visible(false);
        finished = true;
        return;
    }

    // Danno al cane
    if (g_dog->invulnerability == 0)
    {
        bn::fixed dx = g_dog->chr_x - chr_x;
        bn::fixed dy = (g_dog->chr_y + 6) - chr_y;   // hitbox del cane, come in collision.cpp

        if (bn::abs(dx) < BOMB_EXPLOSION_RADIUS + 8 &&
            bn::abs(dy) < BOMB_EXPLOSION_RADIUS + 13)
        {
            g_dog->dog_damage_and_jump(15, dx < 0);
        }
    }

}
