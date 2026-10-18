#include "globals.h"
#include "constanti_fisica.h"
#include "dog.h"
#include "bau.h"
#include "enemy.h"
#include "item.h"
#include "lava.h"
#include "collision.h"
#include "bn_keypad.h"
#include "bn_math.h"
#include "bn_random.h"
#include "bn_sound_items.h"
#include "bn_sprite_items_fox.h"
#include "bn_sprite_items_fox1632.h"
#include "bn_sprite_items_zombie.h"
#include "bn_sprite_items_lavabubble.h"
#include "bn_log.h"


void dog::start_polvere_anim(bn::fixed x, bn::fixed y, bool groundPound, bool vertical_flip, int rotation_angle) {
    groundPound ? ground_pound_anim->reset() : polvere_anim->reset();
    polvere_sprite->set_visible(true);
    polvere_sprite->set_x(x);
    polvere_sprite->set_y(y);
    polvere_sprite->set_vertical_flip(vertical_flip);
    polvere_sprite->set_rotation_angle_safe(rotation_angle);
}

dog::dog()
{
    spriteItems = bn::sprite_items::fox1632;

    sprite = spriteItems->create_sprite(chr_x - HALF_SCREEN_W, chr_y - HALF_SCREEN_H, 0);

    sprite->set_bg_priority(1);

    actionStand = bn::create_sprite_animate_action_forever(
        *sprite, 8, spriteItems->tiles_item(), 0, 1, 2, 3, 4);
    actionWalk = bn::create_sprite_animate_action_forever(
        *sprite, 2, spriteItems->tiles_item(), 5, 6, 7, 8, 9, 10, 11, 12);

    actionIdle = bn::create_sprite_animate_action_forever(
        *sprite, 5, spriteItems->tiles_item(), 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55);   // idle con testa che si muove

    g_dog->sprite->set_camera(g_camera);
    g_dog->sprite->set_bg_priority(2);

    polvere_sprite = bn::sprite_items::fox1632.create_sprite(0, 0, 0);
    polvere_sprite->set_camera(g_camera);
    polvere_sprite->set_bg_priority(3);
    polvere_anim = bn::create_sprite_animate_action_once(
        *polvere_sprite, 2, bn::sprite_items::fox1632.tiles_item(), 56, 57, 58, 59);   // 4 frame di sbuffo
    ground_pound_anim = bn::create_sprite_animate_action_once(
        *polvere_sprite, 2, bn::sprite_items::fox1632.tiles_item(), 63, 64, 65, 66);

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

    spriteAusiliario = bn::sprite_items::lavabubble.create_sprite(0, 0, 0);
    spriteAusiliario->set_camera(g_camera);
    spriteAusiliario->set_bg_priority(1);
    spriteAusiliario->set_z_order(-1);          // davanti al cane
    spriteAusiliario->set_visible(false);
    carica_anim = bn::create_sprite_animate_action_forever(
        *spriteAusiliario, 1, bn::sprite_items::lavabubble.tiles_item(), 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22);
}

void dog::update_animations() {
    if (!onGround || chr_vx == 0)
        actionWalk->reset();

    if (colpito_anim_ticks > 0)
    {
        colpito_anim_ticks--;
        if (!actionColpito->done())
            actionColpito->update();
    }
    else if (dash_ticks > 0)
    {
        sprite->set_tiles(spriteItems->tiles_item(), 25);
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
        if (!actionIdle->done())
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
    else if (wall_sliding)
        sprite->set_tiles(spriteItems->tiles_item(), 27);
    else if (!onGround)
        sprite->set_tiles(spriteItems->tiles_item(), (chr_vy > 0 ? 23 : 21));
    else if (onGround && chr_vx != 0)
        actionWalk->update();
    else
        actionStand->update();

    if (invulnerability)
        sprite->set_visible(invulnerability % 2);
        
bool carica_piena = charge_ticks >= BAU_CHARGE_MAX && bn::keypad::a_held();

if (carica_piena)
{
    if (!carica_attiva)
    {
        carica_attiva = true;          // frame in cui la carica raggiunge il massimo
        carica_anim->reset();
        spriteAusiliario->set_visible(true);
    }
    carica_anim->update();
    spriteAusiliario->set_x(sprite->x());
    spriteAusiliario->set_y(sprite->y());
    spriteAusiliario->set_horizontal_flip(dir == DIR_LEFT);
}
else if (carica_attiva)
{
    carica_attiva = false;
    spriteAusiliario->set_visible(false);
}        

}

void dog::dog_damage(int amount)
{
    if (life > 0) {
        actionColpito->reset();
        colpito_anim_ticks = COLPITO_ANIM_TICKS;
        life -= amount;
        if (life <= 0) {
            life = 0;
        }
    }
}

void dog::applica_powerup(uint16_t attributo)
{
    if (attributo & POWERUP_CORSA) {
        if (bonus_corsa == bn::fixed(0))
            bonus_corsa = bn::fixed(0.5);
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
        doppio_salto_acquisito = true;
    }
    if (attributo & POWERUP_WALL_JUMP) {
        wall_jump_acquisito = true;
    }
    if (attributo & POWERUP_DASH) {
        dash_acquisito = true;
    }
    if (attributo & POWERUP_AIR_DASH) {
        air_dash_acquisito = true;
    }
    if (attributo & POWERUP_GROUND_POUND) {
        ground_pound_acquisito = true;
    }
}