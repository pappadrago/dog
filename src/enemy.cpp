#include "globals.h"
#include "enemy.h"
#include "bau.h"
#include "dog.h"
#include "bn_random.h"
#include "bn_sprite_items_weapons.h"
#include "bn_sprite_items_enemies.h"
#include "bn_sprite_items_enemies2.h"
#include "bn_math.h"
#include "bn_log.h"

enemy::enemy()
{
    if (g_rng.get_bool() || true)
    {

        int N = g_rng.get_int(10) * 3;

        sprite = bn::sprite_items::enemies.create_sprite(chr_x, chr_y, 0);
        actionStand = bn::create_sprite_animate_action_forever(
            *sprite, 6, bn::sprite_items::enemies.tiles_item(), N + 1, N + 1);
        actionWalk = bn::create_sprite_animate_action_forever(
            *sprite, 3, bn::sprite_items::enemies.tiles_item(), N + 0, N + 1, N + 2, N + 1);

        weaponSprite = bn::sprite_items::weapons.create_sprite(chr_x, chr_y, g_rng.get_int(10));
        weaponSprite->set_bg_priority(1);
        weaponSprite->set_camera(g_camera);
    }
    else {
        {

            int N = g_rng.get_int(10) * 3;

            sprite = bn::sprite_items::enemies2.create_sprite(chr_x, chr_y, 0);
            actionStand = bn::create_sprite_animate_action_forever(
                *sprite, 6, bn::sprite_items::enemies2.tiles_item(), N + 1, N + 1);
            actionWalk = bn::create_sprite_animate_action_forever(
                *sprite, 3, bn::sprite_items::enemies2.tiles_item(), N + 0, N + 1, N + 2, N + 1);
        }
    }
    sprite->set_bg_priority(1);
    sprite->set_camera(g_camera);

    do_spawn();
    box_dim = bn::fixed(8);
    box_halfdim = bn::fixed(4);
}


void enemy::do_powerup()
{
    max_vx += bn::fixed(0.2);
}


void enemy::do_spawn()
{
    chr_y = 512 - 200;
    chr_x = g_rng.get_int(MAP_W);
    dir = g_rng.get_bool() ? DIR_LEFT : DIR_RIGHT;
    ticks2action = 120 + 60 * g_rng.get_int(5);
}

void enemy::update()
{
    chr_vx += bn::fixed(0.05).multiplication(dir);
    chr_x += cap(chr_vx, max_vx);
    apply_map();
    apply_gravity();

    if (weaponSprite && weaponTicks > 0) {

        bn::fixed delta = weapon_time - weaponTicks;
        delta = delta.division(weapon_time);

        weaponSprite->set_rotation_angle_safe(bn::fixed(14.0).multiplication(weaponDir).multiplication(weaponTicks));
        weaponSprite->set_x(
            wx_base - HALF_SCREEN_W +
            bn::fixed(8.0).multiplication(weaponDir) +
            SCREEN_W.multiplication(delta).multiplication(weaponDir));
        weaponSprite->set_y(
            wy_base - HALF_SCREEN_H + bn::degrees_lut_sin_safe(weaponTicks << 3).multiplication(bn::fixed(10.0))
        );
    }



    if (invulnerability > 0) invulnerability--;
    if (weaponTicks > 0) weaponTicks--;

    if (ticks2action > 0)
    {
        ticks2action--;
        if (ticks2action == 0)
        {

            dir = g_rng.get_bool() ? DIR_LEFT : DIR_RIGHT;
            chr_vx = bn::fixed(0);
            ticks2action = 120 + 60 * g_rng.get_int(5);
            if (weaponSprite && weaponTicks == 0) {
                wx_base = chr_x;
                wy_base = chr_y;
                weaponTicks = weapon_time.integer();
                weaponDir = (g_dog->chr_x > chr_x ? DIR_RIGHT : DIR_LEFT);
                ticks2action = weaponTicks;
            }
            if (g_rng.get_bool()) {
                chr_vy = bn::fixed(-4.0);
                max_vx = g_rng.get_fixed(bn::fixed(2.0)) + bn::fixed(1.0);
            }


        }
    }

    sprite->set_x(chr_x - HALF_SCREEN_W);
    sprite->set_y(chr_y - HALF_SCREEN_H);
    sprite->set_horizontal_flip(dir == DIR_LEFT);

    if (weaponSprite && weaponTicks == 0) {
        weaponSprite->set_x(chr_x - HALF_SCREEN_W + bn::fixed(8.0).multiplication(dir));
        weaponSprite->set_y(chr_y - HALF_SCREEN_H);
        weaponSprite->set_horizontal_flip(dir == DIR_LEFT);
    }
    if (onGround && chr_vx.integer() == 0)
        actionStand->update();
    else
        actionWalk->update();

    if (invulnerability > 0)
        sprite->set_visible(invulnerability % 2);
    else
        sprite->set_visible(true);

    if (weaponTicks > 0 && weaponTicks < 30)
        weaponSprite->set_visible(weaponTicks % 2);
    else
        weaponSprite->set_visible(true);
}

void enemy::bounce(int _dir)
{
    dir = _dir;
    chr_vy = bn::fixed(-2.0);


    if (weaponTicks == 0) {
        invulnerability = 60;
        ticks2action = 240;
        chr_vx = bn::fixed(2.0).multiplication(dir);
    }
    else {
        weaponTicks = 0;
        ticks2action = 30;
    }
}


