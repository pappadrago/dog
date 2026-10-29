#include "boss.h"
#include "globals.h"
#include "dog.h"
#include "obj.h"
#include "schemi.h"
#include "bn_sprite_items_zombie.h"
#include "bn_sprite_items_weapons.h"
#include "bn_random.h"
#include "bn_math.h"
#include "bn_log.h"
#include "healthbar.h"
#include <bn_sound_items.h>

boss::boss(bn::fixed x, bn::fixed y, int num_colpi, uint8_t _attributo, bn::fixed _dimensione)
{
    chr_x = x;
    chr_y = y;
    patrol_origin_x = x;
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

    if (attributo & enemy_attr::ATTR_MELEE)
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

boss::boss(bn::fixed x, bn::fixed y, int num_colpi, boss_tipo tipo, bn::fixed _dimensione) :
    boss(x, y, num_colpi, enemy_attr::ATTR_NONE, _dimensione)
{
    bomber_mode = tipo == boss_tipo::BOMBER;
    baller_mode = tipo == boss_tipo::BALLER;
    flyer_mode = tipo == boss_tipo::FLYER;
    if (baller_mode)
    {
        max_vx = bn::fixed(1);
        ticks_prossimo_attacco = 120;
    }
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

    if (bomber_mode)
    {
        update_bomber();
        return;
    }

    if (baller_mode)
    {
        update_baller();
        return;
    }

    if (flyer_mode)
    {
        update_flyer();
        return;
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

        if ((attributo & enemy_attr::ATTR_MELEE) && dist_x < RANGE_MELEE && onGround)
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
        else if ((attributo & enemy_attr::ATTR_SUMMON) && dist_x < RANGE_SUMMON && ticks_prossimo_summon <= 0 && onGround && weaponTicks == 0)
        {
            // Priorità 2: troppo lontano per colpire, ma nel raggio del summon
            dir = cane_a_destra ? DIR_RIGHT : DIR_LEFT;
            chr_accx = bn::fixed(0);
            avvia_summon();
        }
        else if (attributo & enemy_attr::ATTR_PATROL)
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
    bool bash_attivo = sees_dog && dog_in_bash_range() && (attributo & enemy_attr::ATTR_BASH);
    chr_vx = cap(chr_vx, bash_attivo ? (max_vx + max_vx + max_vx) : max_vx);
    chr_x += chr_vx;

    if (meleeTicks > 0) meleeTicks--;
    if (ticks_prossimo_summon > 0) ticks_prossimo_summon--;   // il cooldown scorre sempre, non solo mentre pattuglia

    apply_map();
    apply_friction();
    apply_gravity();

    // --- Saltello periodico, solo mentre pattuglia liberamente ---
    if (currentAction == ACTION_MOVE && onGround && chr_vx == ZERO)
    {
        if (ticks_prossimo_saltello > 0)
            ticks_prossimo_saltello--;
        else
        {
            chr_vy = ENEMY_BIG_JUMP_VY;
            onGround = false;
            ticks_prossimo_saltello = 90 + g_rng.get_int(60);
        }
    }

    if (attributo & enemy_attr::ATTR_FIREBALL) {
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

void boss::update_bomber()
{
    for (bn::optional<bomb>& projectile : bomber_bombs)
    {
        if (projectile)
        {
            projectile->update();
            if (projectile->finished)
                projectile.reset();
        }
    }

    chr_accx = ZERO;
    chr_vx = ZERO;
    apply_map();
    apply_gravity();

    if (bomber_ticks_to_jump > 0)
        bomber_ticks_to_jump--;
    if (bomber_ticks_to_jump <= 0 && onGround)
    {
        chr_vy = ENEMY_BIG_JUMP_VY;
        onGround = false;

        int total_energy = colpi_totali > 0 ? colpi_totali : 1;
        int remaining_energy = colpi_rimasti;
        if (remaining_energy < 0)
            remaining_energy = 0;
        if (remaining_energy > total_energy)
            remaining_energy = total_energy;
        bomber_ticks_to_jump = 45 + 135 * remaining_energy / total_energy;
    }

    if (bomber_ticks_to_throw > 0)
        bomber_ticks_to_throw--;
    if (bomber_ticks_to_throw <= 0 && throw_bomber_bomb())
    {
        if (bomber_shots_in_burst == 0 && actionSummon.has_value())
            actionSummon->reset();
        bomber_shots_in_burst++;
        bomber_throw_animation_ticks = 24;

        if (bomber_shots_in_burst == MAX_ENEMY_BOMBS)
        {
            bomber_shots_in_burst = 0;
            bomber_ticks_to_throw = 180;
        }
        else
            bomber_ticks_to_throw = 60;
    }

    bool throwing = bomber_throw_animation_ticks > 0;
    if (throwing)
    {
        bomber_throw_animation_ticks--;
        if (actionSummon.has_value())
            actionSummon->update();
    }
    else
        actionStand->update();

    if (g_dog->chr_x != chr_x)
        dir = g_dog->chr_x > chr_x ? DIR_RIGHT : DIR_LEFT;
    sprite->set_x(chr_x - HALF_SCREEN_W);
    sprite->set_y(chr_y - HALF_SCREEN_H);
    sprite->set_horizontal_flip(dir == DIR_LEFT);

    if (invulnerability > 0)
    {
        invulnerability--;
        sprite->set_visible(invulnerability % 2);
    }
    else
        sprite->set_visible(true);
}

bool boss::throw_bomber_bomb()
{
    bn::fixed dx = g_dog->chr_x - chr_x;
    if (dx != ZERO)
        dir = dx > ZERO ? DIR_RIGHT : DIR_LEFT;
    bn::fixed vx = cap(dx.division(70), bn::fixed(2.5));

    for (bn::optional<bomb>& projectile : bomber_bombs)
    {
        if (! projectile)
        {
            projectile.emplace(chr_x + bn::fixed(8).multiplication(dir), chr_y - 4,
                               vx, bn::fixed(-4.0));
            return true;
        }
    }
    return false;
}

void boss::update_baller()
{
    if (baller_fireball)
    {
        baller_fireball->update();
        if (baller_fireball->finished)
            baller_fireball.reset();
    }

    static constexpr bn::fixed PATROL_HALF_RANGE = 96;
    static constexpr bn::fixed JUMP_VY = ENEMY_BIG_JUMP_VY;
    static constexpr int FIREBALL_COOLDOWN = 120;
    bn::fixed patrol_left = patrol_origin_x - PATROL_HALF_RANGE;
    bn::fixed patrol_right = patrol_origin_x + PATROL_HALF_RANGE;

    if ((dir == DIR_RIGHT && chr_x >= patrol_right) ||
        (dir == DIR_LEFT && chr_x <= patrol_left))
    {
        dir = -dir;
        chr_x = dir == DIR_LEFT ? patrol_right : patrol_left;
        chr_vx = ZERO;
    }

    bool blocked_ahead = muro_lato == dir ||
        is_solid_at(chr_x + bn::fixed(dir).multiplication(box_halfdim + 2), chr_y, g_schema);
    if (onGround && blocked_ahead)
    {
        chr_vy = JUMP_VY;
        onGround = false;
    }

    chr_accx = onGround ? GROUND_ACCEL : AIR_ACCEL;
    chr_vx += chr_accx.multiplication(dir);
    chr_vx = cap(chr_vx, max_vx);
    chr_x += chr_vx;
    apply_map();
    apply_friction();
    apply_gravity();

    if (ticks_prossimo_attacco > 0)
        ticks_prossimo_attacco--;
    if (ticks_prossimo_attacco <= 0 && ! baller_fireball && throw_baller_fireball())
        ticks_prossimo_attacco = FIREBALL_COOLDOWN;

    sprite->set_x(chr_x - HALF_SCREEN_W);
    sprite->set_y(chr_y - HALF_SCREEN_H);
    sprite->set_horizontal_flip(dir == DIR_LEFT);
    if (onGround && chr_vx != ZERO)
        actionWalk->update();
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

bool boss::throw_baller_fireball()
{
    bn::fixed dx = g_dog->chr_x - chr_x;
    if (bn::abs(dx) > SCREEN_W)
        return false;

    bn::fixed dy = g_dog->chr_y - chr_y;
    bn::fixed angle = bn::degrees_atan2(dy.integer(), dx.integer());
    bomb_def def;
    def.tipoProjec = BOMB_FIREBALL;
    def.x = chr_x;
    def.y = chr_y + enemy_config::SPAWN_Y_OFFSET;
    def.vx = bn::degrees_lut_cos_safe(angle).multiplication(enemy_config::FIREBALL_SPEED);
    def.vy = bn::degrees_lut_sin_safe(angle).multiplication(enemy_config::FIREBALL_SPEED);
    def.ignoreGravity = true;
    def.ignoreWalls = true;
    def.max_bounces = 0;
    def.ttl = enemy_config::PROJECTILE_LIFETIME;
    def.tileIndex = enemy_config::FIREBALL_TILE_INDEX;
    def.angle = -angle;

    baller_fireball.emplace(def);
    bn::sound_items::fire_lighting.play();
    return true;

}

void boss::update_flyer()
{
    for (bn::optional<bomb>& projectile : flyer_bombs)
    {
        if (projectile)
        {
            projectile->update();
            if (projectile->finished)
                projectile.reset();
        }
    }

    if (flyer_fireball)
    {
        flyer_fireball->update();
        if (flyer_fireball->finished)
            flyer_fireball.reset();
    }

    if (flyer_ticks_to_bomb > 0)
        flyer_ticks_to_bomb--;
    if (flyer_ticks_to_bomb <= 0 && throw_flyer_bomb())
        flyer_ticks_to_bomb = 120;

    if (flyer_ticks_to_fireball > 0)
        flyer_ticks_to_fireball--;
    if (flyer_ticks_to_fireball <= 0)
        flyer_fireball_pending = true;

    const collision_map_info& map = get_collision_map(g_schema);
    bn::fixed target_x = g_dog->chr_x;
    if (flyer_fireball_pending)
        target_x += bn::fixed(g_dog->dir * 48);
    if (target_x < 16)
        target_x = 16;
    else if (target_x > map.map_w - 16)
        target_x = bn::fixed(map.map_w - 16);

    bn::fixed target_y = flyer_fireball_pending ? g_dog->chr_y : g_dog->chr_y - 100;
    chr_x += cap(target_x - chr_x, bn::fixed(2));
    chr_y += cap(target_y - chr_y, bn::fixed(2));

    if (flyer_fireball_pending &&
        bn::abs(target_x - chr_x) <= 2 &&
        bn::abs(target_y - chr_y) <= 2 &&
        !flyer_fireball && throw_flyer_fireball())
    {
        flyer_fireball_pending = false;
        flyer_ticks_to_fireball = 420;
    }

    dir = g_dog->chr_x > chr_x ? DIR_RIGHT : DIR_LEFT;
    sprite->set_x(chr_x - HALF_SCREEN_W);
    sprite->set_y(chr_y - HALF_SCREEN_H);
    sprite->set_horizontal_flip(dir == DIR_LEFT);
    actionWalk->update();

    if (invulnerability > 0)
    {
        invulnerability--;
        sprite->set_visible(invulnerability % 2);
    }
    else
        sprite->set_visible(true);
}

bool boss::throw_flyer_bomb()
{
    bn::fixed dx = g_dog->chr_x - chr_x;
    bn::fixed vx = cap(dx.division(90), bn::fixed(2.5));

    for (bn::optional<bomb>& projectile : flyer_bombs)
    {
        if (!projectile)
        {
            projectile.emplace(chr_x, chr_y + 8, vx, ZERO);
            return true;
        }
    }
    return false;
}

bool boss::throw_flyer_fireball()
{
    bn::fixed dx = g_dog->chr_x - chr_x;
    bn::fixed dy = g_dog->chr_y - chr_y;
    bn::fixed angle = bn::degrees_atan2(dy.integer(), dx.integer());

    bomb_def def;
    def.tipoProjec = BOMB_FIREBALL;
    def.x = chr_x;
    def.y = chr_y + enemy_config::SPAWN_Y_OFFSET;
    def.vx = bn::degrees_lut_cos_safe(angle).multiplication(enemy_config::FIREBALL_SPEED);
    def.vy = bn::degrees_lut_sin_safe(angle).multiplication(enemy_config::FIREBALL_SPEED);
    def.ignoreGravity = true;
    def.ignoreWalls = true;
    def.max_bounces = 0;
    def.ttl = enemy_config::PROJECTILE_LIFETIME;
    def.tileIndex = enemy_config::FIREBALL_TILE_INDEX;
    def.angle = -angle;

    flyer_fireball.emplace(def);
    bn::sound_items::fire_lighting.play();
    return true;
}

bool boss::dog_in_melee_range() const
{
    return bn::abs(g_dog->chr_x - chr_x) < 70;
}

bool boss::subisci_colpo(int danno)
{
    if (life <= 0 || invulnerability > 0)
        return false;

    if (danno < 1)
        danno = 1;
    life -= danno;
    if (life < 0)
        life = 0;
    invulnerability = 30;
    g_healthbar->show(this);
    return true;
}

bool boss::dog_in_bash_range() const
{
    return bn::abs(g_dog->chr_x - chr_x) < 70;
}

void boss::avvia_summon()
{
    in_summon = true;
    summon_ticks = 120;   // ~2 secondi a 60fps
    chr_vx = chr_accx = ZERO;
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
    enemy_def def{ enemy_tipo::MOVE_CHASE, enemy_attr::ATTR_AIM,
                   int16_t(chr_x.integer() + 10 * dir), int8_t(-dir), 60 };
    enemy* nuovo = new enemy(def);

    nuovo->chr_y = chr_y - 16;
    nuovo->chr_vy = ENEMY_MEDIUM_JUMP_VY;
    nuovo->chr_vx = dir * MEDIUM_VX_R;
    nuovo->vita_residua_ticks = 600;   // 10 secondi a 60fps

    g_enemies->push_back(nuovo);

    in_summon = false;
    ticks_prossimo_summon = 200 + g_rng.get_int(100);

}