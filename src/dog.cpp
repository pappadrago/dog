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
#include "bn_sprite_items_fox.h"
#include "bn_sprite_items_fox1632.h"
#include "bn_sprite_items_zombie.h"
#include "bn_log.h"


dog::dog()
{
    spriteItems = bn::sprite_items::fox1632;
    max_vx += bn::fixed(0.5);         // velocità massima più alta

    sprite = spriteItems->create_sprite(chr_x - HALF_SCREEN_W, chr_y - HALF_SCREEN_H, 0);

    sprite->set_bg_priority(1);

    actionStand = bn::create_sprite_animate_action_forever(
        *sprite, 8, spriteItems->tiles_item(), 0, 1, 2, 3, 4);
    actionWalk = bn::create_sprite_animate_action_forever(
        *sprite, 2, spriteItems->tiles_item(), 5, 6, 7, 8, 9, 10, 11, 12);

    actionIdle = bn::create_sprite_animate_action_forever(
        *sprite, 5, spriteItems->tiles_item(), 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55);   // placeholder: idle con coda che si muove


    g_dog->sprite->set_camera(g_camera);
    g_dog->sprite->set_bg_priority(2);

    polvere_sprite = bn::sprite_items::fox1632.create_sprite(0, 0, 0);
    polvere_sprite->set_camera(g_camera);
    polvere_sprite->set_bg_priority(3);
    polvere_anim = bn::create_sprite_animate_action_once(
        *polvere_sprite, 2, bn::sprite_items::fox1632.tiles_item(), 56, 57, 58, 59);   // 4 frame di sbuffo

    box_dim = bn::fixed(8);
    box_halfdim = bn::fixed(4);

    actionWallJump = bn::create_sprite_animate_action_once(
        *sprite, 3, spriteItems->tiles_item(), 20, 21, 22);

    actionColpito = bn::create_sprite_animate_action_once(
        *sprite, 3, spriteItems->tiles_item(), 13, 14, 15, 16, 17);

    actionAtterraggio = bn::create_sprite_animate_action_once(
        *sprite, 3, spriteItems->tiles_item(), 24, 25, 26, 27);

    actionStacco = bn::create_sprite_animate_action_once(
        *sprite, 3, spriteItems->tiles_item(), 21, 21);
}

void dog::update()
{
    //Dato che in main.cpp g_dog->update() gira prima di lava_zona::update() nello stesso frame (verificato nel tuo game_step),
    //  resettare qui a ogni frame e lasciare che un'eventuale zona di lava la rialzi più tardi nello stesso giro 
    // produce il comportamento corretto
    sprite->set_bg_priority(1);

    if (bn::keypad::b_pressed())
    {
        if (onGround)
        {
            onGround = false;
            chr_vy = bn::fixed(-4.0) - bonus_salto;
            doppio_salto_disponibile = ha_doppio_salto;

            actionStacco->reset();
            stacco_anim_ticks = STACCO_ANIM_TICKS;
        }
        else if (ha_wall_jump && muro_lato != 0)
        {
            int direzione_spinta = (muro_lato == DIR_RIGHT) ? DIR_LEFT : DIR_RIGHT;

            chr_vx = WALL_JUMP_VX.multiplication(bn::fixed(direzione_spinta));
            chr_vy = WALL_JUMP_VY;
            dir = direzione_spinta;

            actionWallJump->reset();
            wall_jump_anim_ticks = WALL_JUMP_ANIM_TICKS;


            polvere_anim->reset();
            polvere_sprite->set_visible(true);
            polvere_sprite->set_x(chr_x - HALF_SCREEN_W);
            polvere_sprite->set_y(chr_y - HALF_SCREEN_H);   // sopra la testa, non ai piedi
            polvere_sprite->set_rotation_angle_safe(-90 * dir);
            polvere_sprite->set_vertical_flip(false);
            muro_lato = 0;   // consumato: serve toccare di nuovo un muro per un secondo wall jump
            doppio_salto_disponibile = ha_doppio_salto;
        }
        else if (doppio_salto_disponibile)
        {
            chr_vy = bn::fixed(-2.5) - bonus_salto;
            doppio_salto_disponibile = false;

            actionWallJump->reset();   // riuso la stessa animazione del wall jump come "guizzo a mezz'aria"; vedi nota sotto
            wall_jump_anim_ticks = WALL_JUMP_ANIM_TICKS;

            polvere_anim->reset();
            polvere_sprite->set_visible(true);
            polvere_sprite->set_x(chr_x - HALF_SCREEN_W);
            polvere_sprite->set_y(chr_y - HALF_SCREEN_H);   // sopra la testa, non ai piedi
            polvere_sprite->set_rotation_angle_safe(0);
            polvere_sprite->set_vertical_flip(false);
        }
    }

    if (bn::keypad::r_pressed())
    {

    }

    bn::fixed runningExtraSpeed = bn::keypad::l_held() ? bn::fixed(.8) : ZERO;

    if (bn::keypad::right_held()) {
        dir = DIR_RIGHT;
        chr_vx += (onGround ? GROUND_ACCEL : AIR_ACCEL);
    }
    else if (bn::keypad::left_held()) {
        dir = DIR_LEFT;
        chr_vx -= (onGround ? GROUND_ACCEL : AIR_ACCEL);
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
            else if (g_boss->weaponSprite && g_boss->weaponTicks > 0) {
                hit = check_collision_16(*g_boss->weaponSprite);
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

    if (atterrato_ora)
        doppio_salto_disponibile = ha_doppio_salto;

    if (atterrato_ora && velocita_atterraggio > CADUTA_DURA_SOGLIA)
    {

        actionAtterraggio->reset();
        atterraggio_anim_ticks = ATTERRAGGIO_ANIM_TICKS;

        stordito_ticks = STORDIMENTO_CADUTA_TICKS;
        polvere_anim->reset();
        polvere_sprite->set_visible(true);
        polvere_sprite->set_x(chr_x - HALF_SCREEN_W);
        polvere_sprite->set_y(chr_y - HALF_SCREEN_H);
        polvere_sprite->set_vertical_flip(false);
        polvere_sprite->set_rotation_angle_safe(0);
    }
    else if (testata_ora)
    {
        stordito_ticks = STORDIMENTO_CADUTA_TICKS;
        polvere_anim->reset();
        polvere_sprite->set_visible(true);
        polvere_sprite->set_x(chr_x - HALF_SCREEN_W);
        polvere_sprite->set_y(chr_y + 4 - HALF_SCREEN_H);   // sopra la testa, non ai piedi
        polvere_sprite->set_vertical_flip(true);
        polvere_sprite->set_rotation_angle_safe(0);
    }

    if (!polvere_anim->done())
        polvere_anim->update();
    else
        polvere_sprite->set_visible(false);

    if (wall_jump_anim_ticks == 0)
        chr_vx = cap(chr_vx, max_vx + runningExtraSpeed + bonus_corsa);
    chr_x += chr_vx;
    apply_map();
    // --- Fisica verticale ---
    apply_friction();
    apply_gravity();

    if (chr_vx == 0 && chr_vy == 0)
        idle_ticks++;
    else {
        idle_ticks = 0;
        actionIdle->reset();
    }

    // --- Sprite ---
    sprite->set_x(chr_x - HALF_SCREEN_W);
    sprite->set_y(chr_y - HALF_SCREEN_H);
    sprite->set_horizontal_flip(dir == DIR_LEFT);

 

}

void dog::animations(){
       if (!onGround || chr_vx == 0)
        actionWalk->reset();

    if (colpito_anim_ticks > 0)
    {
        colpito_anim_ticks--;
        if (!actionColpito->done())
            actionColpito->update();
    }
    else if (atterraggio_anim_ticks > 0)
    {
        atterraggio_anim_ticks--;
        if (!actionAtterraggio->done())
            actionAtterraggio->update();
    }
    else if (stacco_anim_ticks > 0)
    {
        stacco_anim_ticks--;
        if (!actionStacco->done())
            actionStacco->update();
    }
    else if (idle_ticks > 120) {
        actionIdle->update();
        if (idle_ticks > 360) {
            idle_ticks = 0;
            actionIdle->reset();
        }
    }
    else if (wall_jump_anim_ticks > 0)
    {
        wall_jump_anim_ticks--;
        if (!actionWallJump->done())
            actionWallJump->update();
    }
    else if (!onGround)
        sprite->set_tiles(spriteItems->tiles_item(),  (chr_vy > 0 ? 23 : 21));
    else if (onGround && chr_vx != 0)
        actionWalk->update();
    else
        actionStand->update();

    if (invulnerability)
        sprite->set_visible(invulnerability % 2);

}

void dog::dog_damage(int amount)
{
    actionColpito->reset();
    colpito_anim_ticks = COLPITO_ANIM_TICKS;
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
    if (attributo & POWERUP_DOPPIO_SALTO) {
        ha_doppio_salto = true;
    }
    if (attributo & POWERUP_WALL_JUMP) {
        ha_wall_jump = true;
    }
}