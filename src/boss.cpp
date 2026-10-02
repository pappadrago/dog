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

    actionWalk = bn::create_sprite_animate_action_forever(
        *sprite, 5, bn::sprite_items::zombie.tiles_item(), 4, 5, 6, 7, 8, 9, 10);

    if (attributo & ATTRIBUTO_MELEE)
    {
        actionMelee = bn::create_sprite_animate_action_forever(
            *sprite, 8, bn::sprite_items::zombie.tiles_item(), 11, 12, 13, 14);   // placeholder: frame dedicati
        melee_damage = 20;
    }

    weaponSprite = bn::sprite_items::weapons.create_sprite(x, y, 29);
    weaponSprite->set_visible(false);
    weaponSprite->set_camera(g_camera);
    dir = DIR_RIGHT;
    max_vx = bn::fixed(.5);

    actionSummon = bn::create_sprite_animate_action_forever(
        *sprite, 4, bn::sprite_items::zombie.tiles_item(), 15, 16);

    ticks_prossimo_attacco = 120;
}

void boss::update()
{
    // --- Lancio arma a distanza: indipendente dalla scelta sopra, il boss può farlo comunque durante ACTION_MOVE ---
    if (weaponTicks > 0)
    {
        wx_base += wpn_vx;
        wy_base += wpn_vy;
        weaponSprite->set_x(wx_base - HALF_SCREEN_W);
        weaponSprite->set_y(wy_base - HALF_SCREEN_H);
        weaponTicks--;
        if (weaponTicks < 30)
            weaponSprite->set_visible(weaponTicks % 2);
        if (weaponTicks == 0)
            weaponSprite->set_visible(false);
    }
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

    static constexpr bn::fixed RANGE_MELEE = 20;    // abbastanza vicino: tenta il corpo a corpo
    static constexpr bn::fixed RANGE_SUMMON = 120;   // troppo lontano per colpire, ma abbastanza per evocare

    bool cane_a_destra = chr_x < g_dog->chr_x;
    bn::fixed dist_x = bn::abs(chr_x - g_dog->chr_x);

    if (currentAction == ACTION_MOVE)
    {
        // lento a cambiare direzione
        if (ticks2dir > 0)
            ticks2dir--;
        if (ticks2dir == 0) {
            ticks2dir = 60;
            dir = cane_a_destra ? DIR_RIGHT : DIR_LEFT;
        }

        chr_accx = GROUND_ACCEL;

        if ((attributo & ATTRIBUTO_MELEE) && dist_x < RANGE_MELEE && onGround)
        {
            // Priorità 1: abbastanza vicino, avanza dritto per colpire
            if (bn::abs(chr_y - g_dog->chr_y) < 32)
            {
                currentAction = ACTION_ATTACK;
                actionMelee->reset();
                meleeTicks = 20;
                ticks_attacco_melee = 32; // multiplo di durata animazione melee
                chr_accx = bn::fixed(0);
            }
        }
        else if ((attributo & ATTRIBUTO_SUMMON) && dist_x < RANGE_SUMMON && ticks_prossimo_summon <= 0 && onGround && weaponTicks == 0)
        {
            // Priorità 2: troppo lontano per colpire, ma nel raggio del summon
            dir = cane_a_destra ? DIR_RIGHT : DIR_LEFT;
            chr_accx = bn::fixed(0);
            avvia_summon();
        }
        else if (attributo & ATTRIBUTO_PATROL)
        {
            // Priorità 3: troppo lontano da entrambi, pattuglia avanti e indietro
            if (onGround)
                chr_accx = GROUND_ACCEL;
            else
                chr_accx = AIR_ACCEL;
        }
    }
    else
        chr_accx = bn::fixed(0);

    chr_vx += chr_accx.multiplication(dir);

    bool sees_dog = (cane_a_destra && dir == DIR_RIGHT) || (!cane_a_destra && dir == DIR_LEFT);
    bool bash_attivo = sees_dog && dog_in_bash_range() && (attributo & ATTRIBUTO_BASH);
    chr_vx = cap(chr_vx, bash_attivo ? (max_vx + max_vx + max_vx) : max_vx);
    chr_x += chr_vx;

    if (meleeTicks > 0) meleeTicks--;
    if (ticks_prossimo_summon > 0) ticks_prossimo_summon--;   // il cooldown scorre sempre, non solo mentre pattuglia

    apply_map();
    apply_friction();
    apply_gravity();

    // --- Saltello periodico, solo mentre pattuglia liberamente ---
    if (currentAction == ACTION_MOVE && onGround && chr_vx == bn::fixed(0))
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

    if (attributo & ATTRIBUTO_FIREBALL) {
        if (currentAction == ACTION_MOVE || currentAction == ACTION_STAND)
        {
            if (ticks_prossimo_attacco > 0)
                ticks_prossimo_attacco--;
            else
            {
                wx_base = chr_x;
                wy_base = chr_y;

                bn::fixed dx = g_dog->chr_x + 16 - wx_base;
                bn::fixed dy = g_dog->chr_y + 8 - wy_base;
                bn::fixed angle = bn::degrees_atan2(dy.integer(), dx.integer());
                bn::fixed gittata = SCREEN_DG;
                bn::fixed weapon_time = bn::fixed(180.0);

                wpn_vx = bn::degrees_lut_cos_safe(angle).multiplication(gittata).division(weapon_time);
                wpn_vy = bn::degrees_lut_sin_safe(angle).multiplication(gittata).division(weapon_time);
                weaponTicks = weapon_time.integer();
                weaponSprite->set_rotation_angle_safe(-90 - angle);
                weaponSprite->set_visible(true);
                ticks_prossimo_attacco = 120 + g_rng.get_int(120);
            }
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

        actionMelee->update();
    }
    else if (currentAction == ACTION_MOVE && onGround && chr_vx != 0)
    {
        actionWalk->update();
    }
    else
    {
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

bool boss::dog_in_melee_range() const
{
    return bn::abs(g_dog->chr_x - chr_x) < 70;
}

bool boss::dog_in_bash_range() const
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
    enemy_def def{ TIPO_NEMICO_ZOMBIE, ATTRIBUTO_AIM,
                   int16_t(chr_x.integer() + 10 * dir), int8_t(-dir), 60 };
    enemy* nuovo = new enemy(def);

    nuovo->chr_y = chr_y - 16;
    nuovo->chr_vy = bn::fixed(-3);
    nuovo->chr_vx = bn::fixed(3 * dir);
    nuovo->vita_residua_ticks = 600;   // 10 secondi a 60fps

    g_enemies->push_back(nuovo);

    in_summon = false;
    ticks_prossimo_summon = 200 + g_rng.get_int(100);

}