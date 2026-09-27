#include "boss.h"
#include "globals.h"
#include "dog.h"
#include "schemi.h"
#include "bn_sprite_items_zombie.h"
#include "bn_sprite_items_weapons.h"
#include "bn_random.h"
#include "bn_math.h"
#include "bn_log.h"

boss::boss(bn::fixed x, bn::fixed y, int num_colpi, uint8_t _attributo, bn::fixed _dimensione)
{
    chr_x = x;
    chr_y = y;
    colpi_rimasti = num_colpi;
    colpi_totali = num_colpi;
    attributo = _attributo;

    dimensione = _dimensione;

    box_dim = dimensione / 2;
    box_halfdim = dimensione / 4;

    sprite = bn::sprite_items::zombie.create_sprite(x - HALF_SCREEN_W, y - HALF_SCREEN_H, 0);
    sprite->set_camera(g_camera);
    sprite->set_bg_priority(1);
    actionStand = bn::create_sprite_animate_action_forever(
        *sprite, 10, bn::sprite_items::zombie.tiles_item(), 0, 1, 2, 3);

    if (attributo & ATTRIBUTO_MELEE)
    {
        actionMelee = bn::create_sprite_animate_action_once(
            *sprite, 5, bn::sprite_items::zombie.tiles_item(), 4, 5, 6, 7, 8, 9, 10, 11);   // placeholder: frame dedicati
        melee_damage = 20;
    }

    weaponSprite = bn::sprite_items::weapons.create_sprite(x, y, 23);
    weaponSprite->set_visible(false);

    dir = DIR_RIGHT;
    max_vx = bn::fixed(1.0);

    actionSummon = bn::create_sprite_animate_action_once(
        *sprite, 12, bn::sprite_items::zombie.tiles_item(),  12, 13, 14);
}

void boss::update()
{

    if (in_summon)
    {
        summon_ticks--;
        if (summon_ticks <= 0)
            completa_summon();

        sprite->set_x(chr_x - HALF_SCREEN_W);
        sprite->set_y(chr_y - HALF_SCREEN_H);
        if (actionSummon.has_value() && !actionSummon->done())
            actionSummon->update();

        if (invulnerability > 0) { invulnerability--; sprite->set_visible(invulnerability % 2); }
        else sprite->set_visible(true);

        return;
    }


    const collision_map_info& mappa = get_collision_map(g_schema);

    bool cane_a_destra = chr_x < g_dog->chr_x;
    bool sees_dog = (cane_a_destra && dir == DIR_RIGHT) || (!cane_a_destra && dir == DIR_LEFT);

    // --- Movimento a muro, sospeso durante l'attacco melee ---
    if (currentAction == ACTION_MOVE)
    {
        bn::fixed meta = bn::fixed(dimensione / 2);
        bn::fixed x_avanti = chr_x + meta.multiplication(dir) + bn::fixed(2 * dir);
        if (x_avanti <= 0 || x_avanti >= bn::fixed(mappa.map_w) || is_solid_at(x_avanti, chr_y, g_schema))
            dir = -dir;

        chr_accx = GROUND_ACCEL;
    }
    else
        chr_accx = bn::fixed(0);

    chr_vx += chr_accx.multiplication(dir);

    bool bash_attivo = sees_dog && dog_in_melee_range() && (attributo & ATTRIBUTO_BASH);
    chr_vx = cap(chr_vx, bash_attivo ? (max_vx + max_vx) : max_vx);
    chr_x += chr_vx;

    // --- Trigger dell'attacco melee ---
    if (currentAction == ACTION_MOVE && (attributo & ATTRIBUTO_MELEE)
        && bn::abs(chr_x - g_dog->chr_x) < 40 && bn::abs(chr_y - g_dog->chr_y) < 32)
    {
        dir = cane_a_destra ? DIR_RIGHT : DIR_LEFT;
        currentAction = ACTION_ATTACK;
        actionMelee->reset();
        meleeTicks = 20;
        ticks_attacco_melee = 40;
        chr_accx = bn::fixed(0);
    }

    if (meleeTicks > 0) meleeTicks--;

    apply_map();
    apply_friction();
    apply_gravity();

    if (currentAction == ACTION_MOVE)
    {

    }

    // --- Saltello periodico, solo mentre pattuglia ---
    if (currentAction == ACTION_MOVE && onGround)
    {
        if (ticks_prossimo_saltello > 0)
            ticks_prossimo_saltello--;
        else
        {
            chr_vy = bn::fixed(-4.0);
            onGround = false;
            ticks_prossimo_saltello = 90 + g_rng.get_int(60);
        }
    }

    // --- Lancio arma a distanza, sospeso durante l'attacco melee ---
    if (weaponTicks > 0)
    {
        wx_base += wpn_vx;
        wy_base += wpn_vy;
        weaponSprite->set_x(wx_base - HALF_SCREEN_W);
        weaponSprite->set_y(wy_base - HALF_SCREEN_H);
        weaponTicks--;
        if (weaponTicks == 0)
            weaponSprite->set_visible(false);
    }
    else if (currentAction == ACTION_MOVE)
    {

        if (ticks_prossimo_summon > 0)
            ticks_prossimo_summon--;
        else
            avvia_summon();

        if (ticks_prossimo_attacco > 0)
            ticks_prossimo_attacco--;
        else
        {
            wx_base = chr_x;
            wy_base = chr_y;

            bn::fixed dx = g_dog->chr_x - wx_base;
            bn::fixed dy = g_dog->chr_y - wy_base;
            bn::fixed angle = bn::degrees_atan2(dy.integer(), dx.integer());
            bn::fixed gittata = SCREEN_DG;
            bn::fixed weapon_time = bn::fixed(120.0);

            wpn_vx = bn::degrees_lut_cos_safe(angle).multiplication(gittata).division(weapon_time + weapon_time);
            wpn_vy = bn::degrees_lut_sin_safe(angle).multiplication(gittata).division(weapon_time + weapon_time);
            weaponTicks = weapon_time.integer();

            weaponSprite->set_visible(true);
            ticks_prossimo_attacco = 100 + g_rng.get_int(60);
        }
    }

    // --- Fine fase di attacco melee ---
    if (currentAction == ACTION_ATTACK)
    {
        ticks_attacco_melee--;
        if (ticks_attacco_melee <= 0)
            currentAction = ACTION_MOVE;
    }



    sprite->set_x(chr_x - HALF_SCREEN_W);
    sprite->set_y(chr_y - HALF_SCREEN_H);
    sprite->set_horizontal_flip(dir == DIR_LEFT);

    if (currentAction == ACTION_ATTACK && actionMelee.has_value())
    {
        if (!actionMelee->done())
            actionMelee->update();
    }
    else
        actionStand->update();

    if (invulnerability > 0)
    {
        invulnerability--;
        sprite->set_visible(invulnerability % 2);
    }
    else
        sprite->set_visible(true);
}

bool boss::dog_in_melee_range() const
{
    return bn::abs(g_dog->chr_x - chr_x) < 70;
}

void boss::avvia_summon()
{
    in_summon = true;
    summon_ticks = 120;   // ~2 secondi a 60fps
    chr_vx = bn::fixed(0);
    chr_accx = bn::fixed(0);
    if (actionSummon.has_value())
        actionSummon->reset();
}

void boss::interrompi_summon()
{
    in_summon = false;
    summon_ticks = 0;
    ticks_prossimo_summon = 150;   // ci riprova dopo un po', come se avesse perso il turno
}

void boss::completa_summon()
{
    enemy_def def{ TIPO_NEMICO_GENERICO, ATTRIBUTO_AIM,
                   int16_t(chr_x.integer() + 40 * dir), int8_t(-dir), 30 };
    enemy* nuovo = new enemy(def);

    nuovo->chr_y = chr_y -16;
    nuovo->chr_vy = bn::fixed(-2);
    nuovo->vita_residua_ticks = 180;   // 10 secondi a 60fps

    g_enemies->push_back(nuovo);

    in_summon = false;
    ticks_prossimo_summon = 200 + g_rng.get_int(100);

    BN_LOG("summoned one enemy");
}