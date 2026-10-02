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
#include "bn_sprite_items_fox1632.h"
#include "bn_sprite_items_zombie.h"
#include "bn_sprite_items_dog5.h"
#include "bn_log.h"


dog::dog(int n)
{


    spriteItems = bn::sprite_items::fox1632;
    max_vx += bn::fixed(0.5);         // velocità massima più alta


    sprite = spriteItems->create_sprite(chr_x - HALF_SCREEN_W, chr_y - HALF_SCREEN_H, 0);

    sprite->set_bg_priority(1);

    actionStand = bn::create_sprite_animate_action_forever(
        *sprite, 8, spriteItems->tiles_item(), 0, 1, 2, 3, 4);
    actionWalk = bn::create_sprite_animate_action_forever(
        *sprite, 2, spriteItems->tiles_item(), 5, 6, 7, 8, 9, 10, 11, 12);


    g_dog->sprite->set_camera(g_camera);
    g_dog->sprite->set_bg_priority(2);

    polvere_sprite = bn::sprite_items::fox1632.create_sprite(0, 0, 0);
    polvere_sprite->set_camera(g_camera);
    polvere_sprite->set_bg_priority(3);
    polvere_anim = bn::create_sprite_animate_action_once(
        *polvere_sprite, 3, bn::sprite_items::fox1632.tiles_item(), 56, 57, 58, 59);   // placeholder: 4 frame di sbuffo

    box_dim = bn::fixed(8);
    box_halfdim = bn::fixed(4);        
}

void dog::update()
{
    if (bn::keypad::b_pressed() && onGround)
    {
        onGround = false;
        chr_vy = bn::fixed(-4.0) - bonus_salto;   // bonus_salto positivo → salto più alto
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

                bool hit;
                bool hitByMelee = false;
                if (enem->melee_damage && enem->currentAction == ACTION_ATTACK) {
                    hit = check_collision_melee(*enem);
                    hitByMelee = true;
                }
                else
                    hit = check_collision_16(*enem);

                if (hit)
                {
                    bn::fixed new_vx = (chr_x < enem->chr_x) ? bn::fixed(-2.0) : bn::fixed(2.0);
                    chr_vy = bn::fixed(-2.0);
                    invulnerability = 60;

                    if (hitByMelee) {
                        dog_damage(enem->melee_damage);
                    }
                    else {
                        enem->beHitByDog();
                        dog_damage(enem->contact_damage);
                    }

                    chr_vx = new_vx;
                }
            }

            if (enem->weaponSprite && enem->weaponTicks > 0) {
                bool hit = check_collision_16(*enem->weaponSprite);
                if (hit)
                {
                    chr_vx = (chr_x < enem->wx_base) ? bn::fixed(-2.0) : bn::fixed(2.0);
                    chr_vy = bn::fixed(-2.0);
                    invulnerability = 60;
                    if (enem->weaponSprite)
                        enem->weaponTicks = 0;

                    dog_damage(3);
                }
            }
        }

        for (item* _item : *g_items)
        {
            if (_item->raccolto) continue;
            bool hit = check_collision_16(*_item->sprite);
            if (hit)
                _item->raccogli();
        }

        if (g_boss.has_value() && g_boss->invulnerability == 0)
        {
            bool hit = false;
            bool hitByMelee = false;
            if (g_boss->melee_damage && g_boss->currentAction == ACTION_ATTACK && g_boss->ticks_attacco_melee <= 16) {
                hit = check_collision_melee_boss(*g_boss);
                hitByMelee = true;
            }
            // else  hit = check_collision_16_boss(*g_boss);

            if (hit)
            {
                bn::fixed new_vx = (chr_x < g_boss->chr_x) ? bn::fixed(-2.0) : bn::fixed(2.0);
                chr_vy = bn::fixed(-2.0);
                invulnerability = 60;
                dog_damage(hitByMelee ? g_boss->melee_damage : g_boss->contact_damage);
                chr_vx = new_vx;
            }
            else            if (g_boss->weaponSprite && g_boss->weaponTicks > 0) {
                bool hit = check_collision_16(*g_boss->weaponSprite);
                if (hit)
                {
                    chr_vx = (chr_x < g_boss->wx_base) ? bn::fixed(-2.0) : bn::fixed(2.0);
                    chr_vy = bn::fixed(-2.0);
                    invulnerability = 60;
                    if (g_boss->weaponSprite) {
                        g_boss->weaponTicks = 0;
                        g_boss->weaponSprite->set_visible(false);
                    }

                    dog_damage(3);
                }
            }
        }
    }


    if (atterrato_ora && velocita_atterraggio > CADUTA_DURA_SOGLIA)
    {
        stordito_ticks = STORDIMENTO_CADUTA_TICKS;
        polvere_anim->reset();
        polvere_sprite->set_visible(true);
        polvere_sprite->set_x(chr_x - HALF_SCREEN_W);
        polvere_sprite->set_y(chr_y - HALF_SCREEN_H );

    }

    if (!polvere_anim->done())
        polvere_anim->update();
    else
        polvere_sprite->set_visible(false);


    if (bn::keypad::right_held()) {
        dir = DIR_RIGHT;
        chr_vx += (onGround ? GROUND_ACCEL : AIR_ACCEL);
    }
    else if (bn::keypad::left_held()) {
        dir = DIR_LEFT;
        chr_vx -= (onGround ? GROUND_ACCEL : AIR_ACCEL);
    }

    bn::fixed run = bn::keypad::l_held() ? bn::fixed(.8) : bn::fixed(.0);
    chr_vx = cap(chr_vx, max_vx + run + bonus_corsa);
    chr_x += chr_vx;
    apply_map();
    // --- Fisica verticale ---
    apply_gravity();
    apply_friction();

    // --- Sprite ---
    sprite->set_x(chr_x - HALF_SCREEN_W);
    sprite->set_y(chr_y - HALF_SCREEN_H);
    sprite->set_horizontal_flip(dir == DIR_LEFT);

    if (!onGround || chr_vx == 0)
        actionWalk->reset();
    if (!onGround)
        sprite->set_tiles(spriteItems->tiles_item(), 8 + (chr_vy > 0 ? 1 : 0));
    else if (onGround && chr_vx != 0)
        actionWalk->update();
    else
        actionStand->update();

    if (invulnerability)
        sprite->set_visible(invulnerability % 2);
}

void dog::dog_damage(int amount)
{
    life -= amount;
    if (life < 0)
        life = 0;
}

void dog::applica_powerup(uint8_t attributo)
{
    if (attributo & POWERUP_CORSA) {
        if (bonus_corsa == bn::fixed(0))
            bonus_corsa = bn::fixed(0.8);
    }
    if (attributo & POWERUP_SALTO) {
        if (bonus_salto == bn::fixed(0))
            bonus_salto = bn::fixed(0.8);
    }
    if (attributo & POWERUP_BAU) {
        if (bonus_bau == bn::fixed(0))
            bonus_bau = bn::fixed(1.0);
    }
    if (attributo & POWERUP_RESISTENZA) {
        if (bonus_resistenza == bn::fixed(0))
            bonus_resistenza += 30;
    }
}