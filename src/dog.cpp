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
#include "bn_sprite_items_fox.h"
#include "bn_sprite_items_dog5.h"
#include "bn_sprite_items_bau.h"
#include "bn_log.h"


dog::dog(int n)
{
    switch (n)
    {
    case 0: spriteItems = bn::sprite_items::dog;  break;
    case 1: spriteItems = bn::sprite_items::fox; break;
    case 2: spriteItems = bn::sprite_items::dog5; break;
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
            if (enem->invulnerability == 0) {

                bool hit = check_collision_16(*enem->sprite, *this);
                if (hit)
                {
                    chr_vx = (chr_x < enem->chr_x) ? bn::fixed(-2.0) : bn::fixed(2.0);
                    chr_vy = bn::fixed(-2.0);
                    invulnerability = 60;
                    if (enem->weaponSprite)
                        enem->weaponTicks = 0;

                    take_damage(1);
                }
            }

            if (enem->weaponSprite && enem->weaponTicks > 0) {
                bool hit = check_collision_16(*enem->weaponSprite, *this);
                if (hit)
                {
                    chr_vx = (chr_x < enem->wx_base) ? bn::fixed(-2.0) : bn::fixed(2.0);
                    chr_vy = bn::fixed(-2.0);
                    invulnerability = 60;
                    if (enem->weaponSprite)
                        enem->weaponTicks = 0;

                    take_damage(3);
                }
            }
        }

        for (item* _item : *g_items)
        {
            if (_item->raccolto) continue;
            bool hit = check_collision_16(*_item->sprite, *this);
            if (hit)
                _item->raccogli();
        }
    }

    // --- Fisica verticale ---
    apply_gravity(g_schema);

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

    bn::fixed run = bn::keypad::l_held() ? bn::fixed(.5) : bn::fixed(.0);
    chr_vx = cap(chr_vx, max_vx + run);
    chr_x += chr_vx;
    apply_map(g_schema);

    // --- Sprite ---
    sprite->set_x(chr_x - HALF_SCREEN_W);
    sprite->set_y(chr_y - HALF_SCREEN_H);
    sprite->set_horizontal_flip(dir == DIR_LEFT);

    if (!onGround || chr_vx == 0)
        actionWalk->reset();
    if (!onGround)
        sprite->set_tiles(spriteItems->tiles_item(), 9 + (chr_vy > 0 ? 1 : 0));
    else if (onGround && chr_vx != 0)
        actionWalk->update();
    else
        actionStand->update();

    if (invulnerability)
        sprite->set_visible(invulnerability % 2);
}

void dog::take_damage(int amount)
{
    life -= amount;
    if (life < 0)
        life = 0;
}

void dog::applica_powerup(uint8_t sotto_tipo)
{
    switch (sotto_tipo)
    {
    case POWERUP_CORSA:
        bonus_corsa += bn::fixed(0.5);
        max_vx += bn::fixed(0.5);
        break;
    case POWERUP_SALTO:
        bonus_salto += bn::fixed(0.5);
        break;
    case POWERUP_BAU:
        bonus_bau += bn::fixed(1.0);
        break;
    case POWERUP_RESISTENZA:
        bonus_resistenza += 30;
        break;
    default:
        break;
    }
}