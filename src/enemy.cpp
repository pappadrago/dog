#include "globals.h"
#include "enemy.h"
#include "bau.h"
#include "dog.h"
#include "schemi.h"
#include "bn_random.h"
#include "bn_sprite_items_weapons.h"
#include "bn_sprite_items_enemies.h"
#include "bn_sprite_items_enemies2.h"
#include "bn_math.h"
#include "bn_log.h"

// Controlla i 4 angoli di una hitbox di 3x3 centrata sull'arma
static bool weapon_hits_map(bn::fixed x, bn::fixed y, int schema)
{
    static constexpr int R = 3;
    return is_solid_at(x - R, y - R, schema) || is_solid_at(x + R, y - R, schema) ||
        is_solid_at(x - R, y + R, schema) || is_solid_at(x + R, y + R, schema);
}

enemy::enemy(u_int8_t _tipo, u_int8_t attr)
{
    this->tipo = _tipo;
    this->attributo = attr;

    int N = g_rng.get_int(10) * 3;
    switch (tipo)
    {
    case TIPO_NEMICO_DRUIDO_DINAMICO:
    case TIPO_NEMICO_DRUIDO_STATICO:

        N = 3 * 3;
        sprite = bn::sprite_items::enemies2.create_sprite(chr_x, chr_y, 0);
        actionStand = bn::create_sprite_animate_action_forever(
            *sprite, 6, bn::sprite_items::enemies2.tiles_item(), N + 1, N + 1);
        actionWalk = bn::create_sprite_animate_action_forever(
            *sprite, 8, bn::sprite_items::enemies2.tiles_item(), N + 0, N + 1, N + 2, N + 1);

        weaponSprite = bn::sprite_items::weapons.create_sprite(chr_x, chr_y, 23);

        break;

    case TIPO_NEMICO_ARCIERE_STATICO:

        N = 7 * 3;
        sprite = bn::sprite_items::enemies.create_sprite(chr_x, chr_y, 0);
        actionStand = bn::create_sprite_animate_action_forever(
            *sprite, 6, bn::sprite_items::enemies.tiles_item(), N + 1, N + 1);
        actionWalk = bn::create_sprite_animate_action_forever(
            *sprite, 8, bn::sprite_items::enemies.tiles_item(), N + 0, N + 1, N + 2, N + 1);

        weaponSprite = bn::sprite_items::weapons.create_sprite(chr_x, chr_y, 3);

        break;
    case TIPO_NEMICO_BLOB_PATTUGLIATORE:

        N = 5 * 3;
        sprite = bn::sprite_items::enemies2.create_sprite(chr_x, chr_y, 0);
        actionStand = bn::create_sprite_animate_action_forever(
            *sprite, 6, bn::sprite_items::enemies2.tiles_item(), N + 1, N + 1);
        actionWalk = bn::create_sprite_animate_action_forever(
            *sprite, 6, bn::sprite_items::enemies2.tiles_item(), N + 0, N + 1, N + 2, N + 1);

        // no weapon

        break;
    case TIPO_NEMICO_SPADACCINO_PATTUGLIATORE:

        N = 0 * 3;
        sprite = bn::sprite_items::enemies.create_sprite(chr_x, chr_y, 0);
        actionStand = bn::create_sprite_animate_action_forever(
            *sprite, 6, bn::sprite_items::enemies.tiles_item(), N + 1, N + 1);
        actionWalk = bn::create_sprite_animate_action_forever(
            *sprite, 8, bn::sprite_items::enemies.tiles_item(), N + 0, N + 1, N + 2, N + 1);

        weaponSprite = bn::sprite_items::weapons.create_sprite(chr_x, chr_y, 2);

        break;

    case TIPO_NEMICO_BOMBAROLO:

        N = 0 * 3;
        sprite = bn::sprite_items::enemies2.create_sprite(chr_x, chr_y, 0);
        actionStand = bn::create_sprite_animate_action_forever(
            *sprite, 6, bn::sprite_items::enemies2.tiles_item(), N + 1, N + 1);
        actionWalk = bn::create_sprite_animate_action_forever(
            *sprite, 4, bn::sprite_items::enemies2.tiles_item(), N + 0, N + 1, N + 2, N + 1);

        // niente weaponSprite: l'arma è l'oggetto bomb
        break;

    default:
        if (g_rng.get_bool() || true)
        {
            sprite = bn::sprite_items::enemies.create_sprite(chr_x, chr_y, 0);
            actionStand = bn::create_sprite_animate_action_forever(
                *sprite, 6, bn::sprite_items::enemies.tiles_item(), N + 1, N + 1);
            actionWalk = bn::create_sprite_animate_action_forever(
                *sprite, 3, bn::sprite_items::enemies.tiles_item(), N + 0, N + 1, N + 2, N + 1);

            weaponSprite = bn::sprite_items::weapons.create_sprite(chr_x, chr_y, g_rng.get_int(10));

        }
        else {
            {

                N = g_rng.get_int(10) * 3;

                sprite = bn::sprite_items::enemies2.create_sprite(chr_x, chr_y, 0);
                actionStand = bn::create_sprite_animate_action_forever(
                    *sprite, 6, bn::sprite_items::enemies2.tiles_item(), N + 1, N + 1);
                actionWalk = bn::create_sprite_animate_action_forever(
                    *sprite, 3, bn::sprite_items::enemies2.tiles_item(), N + 0, N + 1, N + 2, N + 1);
            }
        }
        break;
    }

    sprite->set_bg_priority(1);
    sprite->set_camera(g_camera);
    if (weaponSprite) {
        weaponSprite->set_bg_priority(1);
        weaponSprite->set_camera(g_camera);
    }

    init();
    box_dim = bn::fixed(8);
    box_halfdim = bn::fixed(4);


}

void enemy::init()
{
    chr_y = 0;
    const collision_map_info& map = get_collision_map(g_schema);
    chr_x = g_rng.get_int(map.map_w);

    dir = g_rng.get_bool() ? DIR_LEFT : DIR_RIGHT;
    ticks2action = 120 + 60 * g_rng.get_int(5);

    if (tipo == TIPO_NEMICO_BLOB_PATTUGLIATORE) {
        chr_vx = bn::fixed(1.0);
        ticks2action = 120;
    }
    if (tipo == TIPO_NEMICO_SPADACCINO_PATTUGLIATORE) {
        chr_vx = bn::fixed(1.5);
        ticks2action = 80;
    }

    currentAction = ACTION_STAND;
}



void enemy::update()
{

    if (bomba) {
        bomba->update();
        if (bomba->finished)
            bomba.reset();
    }

    apply_friction();
    // A: calcola le nuove posizioni di enemy e weapon (se c'è)
    {
        chr_vx += chr_accx;
        chr_vx = cap(chr_vx, max_vx);
        chr_x += chr_vx;
    }

    apply_map(g_schema);
    apply_gravity(g_schema);

    if (weaponSprite && weaponTicks > 0) {
        // weapon attiva, devo muoverla

        // posizione dell'arma in coordinate mondo
        wx_base += wpn_vx;
        wy_base += wpn_vy;

        if (tipo == TIPO_NEMICO_SPADACCINO_PATTUGLIATORE) {
            bn::fixed angle = bn::fixed(weaponTicks * weaponDir * 7);
            weaponSprite->set_rotation_angle_safe(angle);
        }

        weaponSprite->set_x(wx_base - HALF_SCREEN_W);
        weaponSprite->set_y(wy_base - HALF_SCREEN_H);
        weaponSprite->set_horizontal_flip(weaponDir == DIR_LEFT);

        // collisione con la mappa: solo armi da lancio
        if (tipo != TIPO_NEMICO_SPADACCINO_PATTUGLIATORE && weapon_hits_map(wx_base, wy_base, g_schema)) {
            //weaponTicks = 0;
            wpn_vx = 0;
            wpn_vy = 0;
        }
    }

    if (invulnerability > 0)
        invulnerability--;
    if (weaponTicks > 0)
        weaponTicks--;

    if (ticks2action > 0)
    {
        ticks2action--;

        // l'azione precedente è conclusa, occorre calcolare la prossima
        if (ticks2action == 0)
        {

            ticks2action = 120;

            switch (currentAction)
            {
            case ACTION_STAND:
                if (weaponSprite || tipo == TIPO_NEMICO_BOMBAROLO)
                    currentAction = ACTION_ATTACK;
                else
                    currentAction = ACTION_MOVE;
                break;
            case ACTION_ATTACK:
                currentAction = ACTION_MOVE;
                break;
            case ACTION_STUN:
                currentAction = ACTION_MOVE;
                break;
            case ACTION_MOVE:
                currentAction = ACTION_STAND;
                break;

            default:
                break;
            }

            if (currentAction == ACTION_STAND) {
                chr_accx = bn::fixed(0);
                ticks2action = 30;
                switch (tipo)
                {
                case TIPO_NEMICO_BLOB_PATTUGLIATORE:
                    ticks2action = 1;
                    chr_vy = bn::fixed(-2);
                    break;
                default:
                    break;
                }
            }

            if (currentAction == ACTION_MOVE) {
                switch (tipo)
                {
                case TIPO_NEMICO_DRUIDO_DINAMICO:
                    dir = g_rng.get_bool() ? DIR_LEFT : DIR_RIGHT;
                    chr_vy = bn::fixed(-4.0);
                    chr_vx = bn::fixed(0);
                    chr_accx = bn::fixed(0.3).multiplication(dir);
                    max_vx = g_rng.get_fixed(bn::fixed(1.0)) + bn::fixed(0.5);
                    break;
                case TIPO_NEMICO_BLOB_PATTUGLIATORE:
                    dir = -dir;
                    chr_vx = bn::fixed(0);
                    chr_accx = bn::fixed(0.2).multiplication(dir);
                    max_vx = bn::fixed(1.0);
                    chr_vy = bn::fixed(-1.5);
                    break;
                case TIPO_NEMICO_SPADACCINO_PATTUGLIATORE:
                    dir = -dir;
                    chr_vx = bn::fixed(0);
                    max_vx = bn::fixed(1.0);
                    chr_accx = bn::fixed(0.2).multiplication(dir);
                    break;
                case TIPO_NEMICO_BOMBAROLO:
                    dir = g_rng.get_bool() ? DIR_LEFT : DIR_RIGHT;
                    chr_vx = bn::fixed(0);
                    max_vx = bn::fixed(0.8);
                    chr_accx = bn::fixed(0.15).multiplication(dir);
                    break;
                default:
                    break;
                }

            }

            if (currentAction == ACTION_ATTACK && tipo == TIPO_NEMICO_BOMBAROLO) {
                throw_bomb();
                ticks2action = 60;
            }
            if (currentAction == ACTION_ATTACK && weaponSprite && weaponTicks == 0) {
                wx_base = chr_x + dir * bn::fixed(8);
                wy_base = chr_y;

                if (tipo == TIPO_NEMICO_SPADACCINO_PATTUGLIATORE) {
                    weaponTicks = ticks2action;
                    weaponDir = dir;
                }
                else {
                    // ARMA DA LANCIO
                    weaponDir = (g_dog->chr_x > wx_base ? DIR_RIGHT : DIR_LEFT);
                    bn::fixed dx = g_dog->chr_x - wx_base;
                    bn::fixed dy = g_dog->chr_y - wy_base;

                    bn::fixed gittata = SCREEN_DG;


                    if (tipo == TIPO_NEMICO_ARCIERE_STATICO)
                        gittata = gittata.multiplication(1.5); // stessa durata maggior gittata -> più velocità

                    if (attributo == ATTRIBUTO_AIM) {
                        bn::fixed angle = bn::degrees_atan2(dy.integer(), dx.integer());

                        wpn_vx = bn::degrees_lut_cos_safe(angle).multiplication(gittata).division(weapon_time + weapon_time);
                        wpn_vy = bn::degrees_lut_sin_safe(angle).multiplication(gittata).division(weapon_time + weapon_time);
                    }
                    else {
                        wpn_vx = gittata.multiplication(weaponDir).division(weapon_time + weapon_time);
                    }
                    weaponTicks = weapon_time.integer();

                    dir = weaponDir;
                }
            }
        }
    }

    // B: aggiorna gli elementi grafici su schermo
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
    else if (currentAction == ACTION_STUN)
        sprite->set_visible(ticks2action % 2);
    else
        sprite->set_visible(true);

    if (weaponSprite) {
        if (weaponTicks > 0 && weaponTicks < WEAPON_FLICK_TIME) {
            weaponSprite->set_visible(weaponTicks % 2);
        }
        else {
            weaponSprite->set_visible(true);
        }
    }
}

void enemy::beHitByBark(int _dir)
{
    chr_vy = bn::fixed(-4.0);
    chr_vx = bn::fixed(2.0).multiplication(_dir);

    if (tipo == TIPO_NEMICO_SPADACCINO_PATTUGLIATORE)
        weaponTicks = 0;
    currentAction = ACTION_STUN;
    ticks2action = 120;
}

void enemy::throw_bomb()
{
    if (bomba) return;                       // una bomba alla volta per nemico

    bn::fixed dx = g_dog->chr_x - chr_x;
    if (bn::abs(dx) > SCREEN_W) return;      // cane troppo lontano

    dir = (dx > 0) ? DIR_RIGHT : DIR_LEFT;
    bn::fixed vx = cap(dx.division(70), bn::fixed(2.5));

    bomba.emplace(chr_x + bn::fixed(8).multiplication(dir), chr_y - 4, vx, bn::fixed(-4.0));
}

