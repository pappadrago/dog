#include "globals.h"
#include "dog.h"
#include "bau.h"
#include "enemy.h"
#include "item.h"
#include "collision.h"
#include "bn_keypad.h"
#include "bn_math.h"
#include "bn_random.h"
#include "bn_sound_items.h"
#include "bn_sprite_items_dog.h"
#include "bn_sprite_items_dog2.h"
#include "bn_sprite_items_dog3.h"
#include "bn_sprite_items_dog4.h"
#include "bn_sprite_items_dog5.h"
#include "bn_sprite_items_bau.h"
#include "s1.h"
#include "bn_log.h"


dog::dog(int n)
{
    switch (n)
    {
    case 0: spriteItems = bn::sprite_items::dog;  break;
    case 1: spriteItems = bn::sprite_items::dog2; break;
    case 2: spriteItems = bn::sprite_items::dog3; break;
    case 3: spriteItems = bn::sprite_items::dog4; break;
    case 4: spriteItems = bn::sprite_items::dog5; break;
    default: break;
    }
    sprite = spriteItems->create_sprite(chr_x - HALF_SCREEN_W, chr_y - HALF_SCREEN_H, 0);

    sprite->set_bg_priority(1);

    actionStand = bn::create_sprite_animate_action_forever(
        *sprite, 5, spriteItems->tiles_item(), 0, 1, 2, 3, 4);
    actionWalk = bn::create_sprite_animate_action_forever(
        *sprite, 3, spriteItems->tiles_item(), 6, 7, 8, 9, 10, 11);

    g_dog->sprite->set_camera(g_camera);
    g_dog->sprite->set_bg_priority(2);

}

void dog::update()
{
    if (bn::keypad::b_pressed() && onGround)
    {
        onGround = false;
        chr_vy = bn::fixed(-5.0);
    }

    if (bn::keypad::a_pressed() && g_bau->ticks == 0)
    {
        g_bau->do_spawn();
    }

    if (invulnerability > 0)
        invulnerability--;

    // --- Dog vs nemici ---
    if (!invulnerability)
    {
        for (enemy* enem : *g_enemies)
        {
            if (enem->invulnerability > 0) continue;
            if (enem->weaponSprite && enem->weaponTicks == 0) continue;

            bool hit = check_collision_16(
                enem->weaponSprite ?
                *enem->weaponSprite :
                *enem->sprite, *this);
            if (hit)
            {
                chr_vx = (chr_x < enem->chr_x) ? bn::fixed(-2.0) : bn::fixed(2.0);
                chr_vy = bn::fixed(-2.0);
                invulnerability = 60;
                if (enem->weaponSprite)
                    enem->weaponTicks = 0;
            }
        }
        for (item* _item : *g_items)
        {
            if (_item->invulnerability > 0) continue;

            bool hit = check_collision_16(*_item->sprite, *this);
            if (hit)
            {
                _item->bounce(chr_x < _item->chr_x ? DIR_RIGHT : DIR_LEFT);
            }
        }
    }


    // --- Fisica verticale ---
    apply_gravity();

    if (bn::keypad::right_held()) {
        dir = DIR_RIGHT;
        chr_vx += (onGround ? GROUND_ACCEL : AIR_ACCEL);
    }
    else if (bn::keypad::left_held()) {
        dir = DIR_LEFT;
        chr_vx -= (onGround ? GROUND_ACCEL : AIR_ACCEL);
    }
    else {
        apply_friction();
    }
    chr_vx = cap(chr_vx, max_vx);
    chr_x += chr_vx;
    apply_map();

    // --- Sprite ---
    sprite->set_x(chr_x - HALF_SCREEN_W);
    sprite->set_y(chr_y - HALF_SCREEN_H);
    sprite->set_horizontal_flip(dir == DIR_LEFT);

    if (!onGround || chr_vx == 0)
        actionWalk->reset();
    if (!onGround)
        sprite->set_tiles(spriteItems->tiles_item(), 9);
    else if (onGround && chr_vx != 0)
        actionWalk->update();
    else
        actionStand->update();

    if (invulnerability)
        sprite->set_visible(invulnerability % 2);
}
