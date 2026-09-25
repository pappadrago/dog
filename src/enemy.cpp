#include "globals.h"
#include "enemy.h"
#include "bau.h"
#include "dog.h"
#include "schemi.h"
#include "bn_random.h"
#include "bn_sprite_items_weapons.h"
#include "bn_sprite_items_enemies.h"
#include "bn_sprite_items_king.h"
#include "bn_sprite_items_enemies2.h"
#include "bn_math.h"
#include "bn_log.h"

// Controlla i 4 angoli di una hitbox di 3x3 centrata sull'arma
static bool weapon_hits_map(bn::fixed x, bn::fixed y, int schema)
{
    static constexpr int HITBOX_SIZE = 3;
    return is_solid_at(x - HITBOX_SIZE, y - HITBOX_SIZE, schema) || is_solid_at(x + HITBOX_SIZE, y - HITBOX_SIZE, schema) ||
        is_solid_at(x - HITBOX_SIZE, y + HITBOX_SIZE, schema) || is_solid_at(x + HITBOX_SIZE, y + HITBOX_SIZE, schema);
}
enemy::enemy(const enemy_def& def)
{
    tipo = def.tipo;
    attributo = def.attributo;

    switch (tipo)
    {
    case TIPO_NEMICO_DRUIDO_DINAMICO:
    case TIPO_NEMICO_DRUIDO_STATICO:
    {
        int N = 3 * 3;
        sprite = bn::sprite_items::enemies2.create_sprite(chr_x, chr_y, 0);
        actionStand = bn::create_sprite_animate_action_forever(
            *sprite, 6, bn::sprite_items::enemies2.tiles_item(), N + 1, N + 1);
        actionWalk = bn::create_sprite_animate_action_forever(
            *sprite, 8, bn::sprite_items::enemies2.tiles_item(), N + 0, N + 1, N + 2, N + 1);

        weaponSprite = bn::sprite_items::weapons.create_sprite(chr_x, chr_y, 23);
        break;
    }
    case TIPO_NEMICO_ARCIERE_STATICO:
    {
        int N = 7 * 3;
        sprite = bn::sprite_items::enemies.create_sprite(chr_x, chr_y, 0);
        actionStand = bn::create_sprite_animate_action_forever(
            *sprite, 6, bn::sprite_items::enemies.tiles_item(), N + 1, N + 1);
        actionWalk = bn::create_sprite_animate_action_forever(
            *sprite, 8, bn::sprite_items::enemies.tiles_item(), N + 0, N + 1, N + 2, N + 1);

        weaponSprite = bn::sprite_items::weapons.create_sprite(chr_x, chr_y, 3);
        break;
    }
    case TIPO_NEMICO_MORTAIO:
    {
        int N = 2 * 3;
        sprite = bn::sprite_items::enemies.create_sprite(chr_x, chr_y, 0);
        actionStand = bn::create_sprite_animate_action_forever(
            *sprite, 6, bn::sprite_items::enemies.tiles_item(), N + 1, N + 1);
        actionWalk = bn::create_sprite_animate_action_forever(
            *sprite, 6, bn::sprite_items::enemies.tiles_item(), N + 0, N + 1, N + 2, N + 1);
        // niente weaponSprite: l'arma è il colpo di mortaio (bomba)
        break;
    }
    case TIPO_NEMICO_BLOB_PATTUGLIATORE:
    {
        int N = 5 * 3;
        sprite = bn::sprite_items::enemies2.create_sprite(chr_x, chr_y, 0);
        actionStand = bn::create_sprite_animate_action_forever(
            *sprite, 6, bn::sprite_items::enemies2.tiles_item(), N + 1, N + 1);
        actionWalk = bn::create_sprite_animate_action_forever(
            *sprite, 6, bn::sprite_items::enemies2.tiles_item(), N + 0, N + 1, N + 2, N + 1);
        // no weapon
        max_vx = bn::fixed(1.0);
        break;
    }
    case TIPO_NEMICO_SPADACCINO_PATTUGLIATORE:
    {
        int N = 0;
        sprite = bn::sprite_items::enemies.create_sprite(chr_x, chr_y, 0);
        actionStand = bn::create_sprite_animate_action_forever(
            *sprite, 6, bn::sprite_items::enemies.tiles_item(), N + 1, N + 1);
        actionWalk = bn::create_sprite_animate_action_forever(
            *sprite, 8, bn::sprite_items::enemies.tiles_item(), N + 0, N + 1, N + 2, N + 1);

        weaponSprite = bn::sprite_items::weapons.create_sprite(chr_x, chr_y, 2);
        break;
    }
    case TIPO_NEMICO_BOMBAROLO:
    {
        int N = 0;
        sprite = bn::sprite_items::enemies2.create_sprite(chr_x, chr_y, 0);
        actionStand = bn::create_sprite_animate_action_forever(
            *sprite, 6, bn::sprite_items::enemies2.tiles_item(), N + 1, N + 1);
        actionWalk = bn::create_sprite_animate_action_forever(
            *sprite, 4, bn::sprite_items::enemies2.tiles_item(), N + 0, N + 1, N + 2, N + 1);
        // niente weaponSprite: l'arma è l'oggetto bomb
        break;
    }
    case TIPO_NEMICO_PATTUGLIATORE:
    {
        sprite = bn::sprite_items::king.create_sprite(chr_x, chr_y, 0);
        actionStand = bn::create_sprite_animate_action_forever(
            *sprite, 6, bn::sprite_items::king.tiles_item(), 0, 1, 2, 3);
        actionWalk = bn::create_sprite_animate_action_forever(
            *sprite, 8, bn::sprite_items::king.tiles_item(), 4, 5, 6, 7, 8, 9);
        // nessuna weaponSprite: danno solo da contatto
        max_vx = bn::fixed(0.8);
        if (attributo & ATTRIBUTO_MELEE) {

            actionMelee = bn::create_sprite_animate_action_once(
                *sprite, 2, bn::sprite_items::king.tiles_item(), 10, 11, 12, 13, 14, 15, 16, 17, 18, 19);
            contact_damage = 1;
            melee_damage = 5;
        }
        break;
    }
    default: // TIPO_NEMICO_GENERICO
    {
        int N = (def.variante % 10) * 3;
        sprite = bn::sprite_items::enemies.create_sprite(chr_x, chr_y, 0);
        actionStand = bn::create_sprite_animate_action_forever(
            *sprite, 6, bn::sprite_items::enemies.tiles_item(), N + 1, N + 1);
        actionWalk = bn::create_sprite_animate_action_forever(
            *sprite, 3, bn::sprite_items::enemies.tiles_item(), N + 0, N + 1, N + 2, N + 1);

        weaponSprite = bn::sprite_items::weapons.create_sprite(chr_x, chr_y, def.variante % 10);
        break;
    }
    }

    sprite->set_bg_priority(1);
    sprite->set_camera(g_camera);
    if (weaponSprite) {
        weaponSprite->set_bg_priority(1);
        weaponSprite->set_camera(g_camera);
    }

    init(def);
    box_dim = bn::fixed(8);
    box_halfdim = bn::fixed(4);
}

void enemy::init(const enemy_def& def)
{
    chr_y = 100;
    chr_x = bn::fixed(def.x);
    dir = def.dir;
    ticks2action = def.delay;

    if (tipo == TIPO_NEMICO_SPADACCINO_PATTUGLIATORE) {
        chr_vx = bn::fixed(1.5);
        ticks2action = 80;
    }

    currentAction = (tipo == TIPO_NEMICO_PATTUGLIATORE || tipo == TIPO_NEMICO_BLOB_PATTUGLIATORE) ? ACTION_MOVE : ACTION_STAND;
}

void enemy::update_pattugliatore()
{
    const collision_map_info& mappa = get_collision_map(g_schema);
    if (currentAction == ACTION_MOVE) {
        // Punto di controllo: un po' oltre il "muso", nella direzione di marcia
        bn::fixed x_avanti = chr_x + bn::fixed(8 * dir);
        bn::fixed y_sotto = chr_y + box_halfdim + bn::fixed(4);   // poco sotto i piedi

        bool muro_o_bordo = (x_avanti <= 0) || (x_avanti >= bn::fixed(mappa.map_w))
            || is_solid_at(x_avanti, chr_y, g_schema);

        bool gradino = !is_solid_at(x_avanti, y_sotto, g_schema);   // niente terreno sotto: è un bordo di piattaforma

        if (muro_o_bordo || gradino) {
            dir = -dir;
        }

        chr_accx = GROUND_ACCEL;
    }
    chr_vx += chr_accx.multiplication(dir);
    bool cane_a_destra = chr_x < g_dog->chr_x;

    // il cane "lo guarda" se il fantasma è dalla parte verso cui il cane è rivolto
    bool sees_dog = (cane_a_destra && dir == DIR_RIGHT) ||
        (!cane_a_destra && dir == DIR_LEFT);

    chr_vx = cap(chr_vx, (sees_dog && dog_in_melee_range() && (attributo & ATTRIBUTO_BASH)) ? (max_vx + max_vx) : max_vx);
    chr_x += chr_vx;

    if (currentAction == ACTION_MOVE && (attributo & ATTRIBUTO_MELEE)
        && bn::abs(chr_x - g_dog->chr_x) < (28)
        && bn::abs(chr_y - g_dog->chr_y) < 24) {
        dir = cane_a_destra ? DIR_RIGHT : DIR_LEFT;
        currentAction = ACTION_ATTACK;
        actionMelee->reset();
        meleeTicks = 16;
        ticks2action = 30;
        chr_accx = bn::fixed(0);
    }

    if (meleeTicks > 0)meleeTicks--;

    apply_map();
    apply_friction();
    apply_gravity();

    ticks2action--;
    if (ticks2action == 0) {
        switch (currentAction)
        {
        case ACTION_STUN:
        case ACTION_ATTACK:
            currentAction = ACTION_MOVE;
            break;

        default:
            break;
        }
    }

    sprite->set_x(chr_x - HALF_SCREEN_W);
    sprite->set_y(chr_y - HALF_SCREEN_H);
    sprite->set_horizontal_flip(dir == DIR_LEFT);

    if (currentAction == ACTION_ATTACK) {
        if (!actionMelee->done())
            actionMelee->update();
    }
    else if (onGround) {
        actionWalk->update();
    }
    else {
        actionStand->update();
    }

    if (invulnerability > 0)
    {
        invulnerability--;
        sprite->set_visible(invulnerability % 2);
    }
    else
        sprite->set_visible(true);
}

void enemy::update()
{

    if (tipo == TIPO_NEMICO_PATTUGLIATORE ||
        tipo == TIPO_NEMICO_BLOB_PATTUGLIATORE) {
        update_pattugliatore();
        return;
    }

    if (bomba) {
        bomba->update();
        if (bomba->finished)
            bomba.reset();
    }

    {
        chr_vx += chr_accx;
        chr_vx = cap(chr_vx, max_vx);
        chr_x += chr_vx;
    }
    apply_map();
    apply_gravity();
    apply_friction();

    // A: calcola le nuove posizioni di enemy e weapon (se c'è)
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
        if (tipo == TIPO_NEMICO_DRUIDO_DINAMICO || tipo == TIPO_NEMICO_DRUIDO_STATICO) {
            // dont check for map collision
        }
        else if (tipo != TIPO_NEMICO_SPADACCINO_PATTUGLIATORE && weapon_hits_map(wx_base, wy_base, g_schema)) {
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
                if (weaponSprite || tipo == TIPO_NEMICO_BOMBAROLO || tipo == TIPO_NEMICO_MORTAIO)
                    currentAction = ACTION_ATTACK;
                else
                    currentAction = ACTION_MOVE;
                break;
            case ACTION_ATTACK:
            case ACTION_STUN:
                if (tipo == TIPO_NEMICO_MORTAIO)
                    currentAction = ACTION_STAND;
                else
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
                    currentAction = ACTION_MOVE;
                    break;
                default:
                    break;
                }
            }

            if (currentAction == ACTION_MOVE) {
                switch (tipo)
                {
                case TIPO_NEMICO_DRUIDO_DINAMICO:
                    dir = chr_x > g_dog->chr_x ? DIR_LEFT : DIR_RIGHT;
                    chr_vy = bn::fixed(-4.0);
                    chr_accx = bn::fixed(0.3).multiplication(dir);
                    max_vx = g_rng.get_fixed(bn::fixed(1.0)) + bn::fixed(0.5);
                    break;
                case TIPO_NEMICO_BLOB_PATTUGLIATORE:
                    dir = -dir;
                    chr_accx = bn::fixed(0.2).multiplication(dir);
                    max_vx = bn::fixed(1.0);
                    chr_vy = bn::fixed(-1.5);
                    break;
                case TIPO_NEMICO_SPADACCINO_PATTUGLIATORE:
                    dir = -dir;
                    max_vx = bn::fixed(1.0);
                    chr_accx = bn::fixed(0.2).multiplication(dir);
                    break;
                case TIPO_NEMICO_BOMBAROLO:
                    dir = g_rng.get_bool() ? DIR_LEFT : DIR_RIGHT;
                    max_vx = bn::fixed(0.8);
                    chr_accx = bn::fixed(0.15).multiplication(dir);
                    break;
                case TIPO_NEMICO_PUGILE:
                    dir = (g_dog->chr_x > chr_x) ? DIR_RIGHT : DIR_LEFT;   // insegue il cane
                    chr_vx = bn::fixed(0);
                    chr_accx = bn::fixed(0.2).multiplication(dir);
                    max_vx = bn::fixed(1.0);
                    ticks2action = 60;
                    break;
                default:
                    break;
                }

            }

            if (currentAction == ACTION_ATTACK && tipo == TIPO_NEMICO_BOMBAROLO) {
                throw_bomb();
                ticks2action = 60;
            }
            if (currentAction == ACTION_ATTACK && tipo == TIPO_NEMICO_MORTAIO) {
                throw_shell();
                ticks2action = 180;   // ricarica più lunga: un colpo pesante, non a raffica
            }

            if (currentAction == ACTION_ATTACK && weaponSprite && weaponTicks == 0) {
                wx_base = chr_x + dir * bn::fixed(8);
                wy_base = chr_y;
                weaponSprite->set_visible(true);
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

                    if (attributo & ATTRIBUTO_AIM) {
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
        if (tipo == TIPO_NEMICO_DRUIDO_DINAMICO || tipo == TIPO_NEMICO_DRUIDO_STATICO) {
            weaponSprite->set_y(-100); // out of view
        }
    }

    if (onGround && bn::abs(chr_vx) <= bn::fixed(.1))
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

void enemy::beHitByBark()
{
    chr_vy = bn::fixed(-3.0);
    chr_vx = g_bau->chr_vx > 0 ? bn::fixed(2.0) : bn::fixed(-2.0);
    chr_accx = bn::fixed(0);

    if (tipo == TIPO_NEMICO_SPADACCINO_PATTUGLIATORE)
        weaponTicks = 0;   // la sua arma è "attaccata" al corpo: si ferma insieme a lui

    currentAction = ACTION_STUN;
    ticks2action =
        invulnerability = 60;   // per l'intera durata dello stordimento non collide col corpo del cane
}

void enemy::beHitByDog()
{
    chr_vy = bn::fixed(-3.0);
    chr_vx = g_dog->chr_vx > 0 ? bn::fixed(2.0) : bn::fixed(-2.0);
    chr_accx = bn::fixed(0);

    if (tipo == TIPO_NEMICO_SPADACCINO_PATTUGLIATORE)
        weaponTicks = 0;   // la sua arma è "attaccata" al corpo: si ferma insieme a lui

    currentAction = ACTION_STUN;
    ticks2action =
        invulnerability = 60;   // per l'intera durata dello stordimento non collide col corpo del cane
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

void enemy::throw_shell()
{
    if (bomba) return;   // un colpo alla volta

    static constexpr int SHELL_FLIGHT_TIME = 70;   // tick stimati per l'arco

    bn::fixed dx = g_dog->chr_x - chr_x;
    dir = (dx > 0) ? DIR_RIGHT : DIR_LEFT;

    bn::fixed vx = dx.division(SHELL_FLIGHT_TIME);
    bn::fixed vy = -bn::fixed(0.5).multiplication(GRAVITY).multiplication(SHELL_FLIGHT_TIME);

    bomba.emplace(chr_x, chr_y - 4, vx, vy, /*max_bounces=*/0);
}
bool enemy::dog_in_melee_range() const
{
    return bn::abs(g_dog->chr_x - chr_x) < 48;
}