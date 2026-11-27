#include "boss.h"
#include "bomb.h"
#include "globals.h"
#include "dog.h"
#include "obj.h"
#include "schemi.h"
#include "bn_sprite_items_zombie.h"
#include "bn_sprite_items_skeleton.h"
#include "bn_sprite_items_bat.h"
#include "bn_sprite_items_weapons.h"
#include "bn_random.h"
#include "bn_math.h"
#include "bn_log.h"
#include "healthbar.h"
#include <bn_sound_items.h>
#include "collision.h"
#include "lampo.h"

namespace
{
    enum : int { CH_PATTUGLIA, CH_AVVISO, CH_CARICA, CH_STORDITO };
    enum : int { SR_PATTUGLIA, SR_EVOCA };
    enum : int { SL_ATTESA, SL_SALTO, SL_RECUPERO };
    enum : int { TP_VISIBILE, TP_NASCOSTO, TP_RAFFICA };
    enum : int { GU_GUARDIA, GU_ATTACCO };
    enum : int { SP_FERMO, SP_SPOSTA };
}

boss::boss(bn::fixed x, bn::fixed y, int num_colpi, boss_tipo _tipo, bn::fixed _dimensione)
{
    tipo = _tipo;

    chr_x = x;
    chr_y = y;
    colpi_rimasti = num_colpi;
    colpi_totali = num_colpi;
    attributo = enemy_attr::ATTR_NONE;

    dimensione = _dimensione;

    if (tipo == boss_tipo::SUMMONER)
        attributo |= enemy_attr::ATTR_SUMMON;
    if (tipo == boss_tipo::SKELETON)
        attributo |= enemy_attr::ATTR_MELEE;

    box_dim = dimensione / 2;
    box_halfdim = dimensione / 4;
    if (tipo == boss_tipo::FLYER) {
        sprite = bn::sprite_items::bat.create_sprite(x - HALF_SCREEN_W, y - HALF_SCREEN_H, 0);
        actionStand = actionWalk = actionMelee = actionSummon = bn::create_sprite_animate_action_forever(
            *sprite, 6, bn::sprite_items::bat.tiles_item(), 0, 1, 2, 1);
    }
    else if (tipo == boss_tipo::SKELETON) {
        sprite = bn::sprite_items::skeleton.create_sprite(x - HALF_SCREEN_W, y - HALF_SCREEN_H, 0);
        actionStand = bn::create_sprite_animate_action_forever(
            *sprite, 10, bn::sprite_items::skeleton.tiles_item(), 0, 1, 2, 3, 4, 5);

        actionWalk = bn::create_sprite_animate_action_forever(
            *sprite, 5, bn::sprite_items::skeleton.tiles_item(), 6, 7, 8, 9, 10, 11);

        if (attributo & enemy_attr::ATTR_MELEE)
        {
            actionMelee = bn::create_sprite_animate_action_forever(
                *sprite, 8, bn::sprite_items::skeleton.tiles_item(), 12, 13, 14, 15, 16, 17);
            melee_damage = 20;
        }

    }
    else {
        sprite = bn::sprite_items::zombie.create_sprite(x - HALF_SCREEN_W, y - HALF_SCREEN_H, 0);
        actionStand = bn::create_sprite_animate_action_forever(
            *sprite, 10, bn::sprite_items::zombie.tiles_item(), 0, 1, 2, 3, 4, 5);

        actionWalk = bn::create_sprite_animate_action_forever(
            *sprite, 5, bn::sprite_items::zombie.tiles_item(), 6, 7, 8, 9, 10);

        if (attributo & enemy_attr::ATTR_MELEE)
        {
            actionMelee = bn::create_sprite_animate_action_forever(
                *sprite, 8, bn::sprite_items::zombie.tiles_item(), 11, 12, 13, 14, 15, 16, 17);
            melee_damage = 20;
        }
        actionSummon = bn::create_sprite_animate_action_forever(
            *sprite, 4, bn::sprite_items::zombie.tiles_item(), 12, 13);

    }
    sprite->set_camera(g_camera);
    sprite->set_bg_priority(1);


    dir = DIR_RIGHT;
    max_vx = bn::fixed(.5);

    ticks_prossimo_attacco = 120;

    if (tipo == boss_tipo::BALLER)
    {
        max_vx = bn::fixed(1);
        ticks_prossimo_attacco = 120;
    }

    if (tipo == boss_tipo::COWARD)
    {
        max_vx = bn::fixed(1.25);
    }

    if (tipo == boss_tipo::CHARGER)
    {
        max_vx = bn::fixed(0.6);
        ticks_prossimo_attacco = 120;
    }
    if (tipo == boss_tipo::STREGONE)
        ticks_prossimo_attacco = 90;
    if (tipo == boss_tipo::SLAMMER)
        ticks_prossimo_attacco = 90;

    if (tipo == boss_tipo::TELEPORTER)
        ticks_prossimo_attacco = 90;

    if (tipo == boss_tipo::SPIRALE)
    {
        ticks_prossimo_attacco = 60;
    }

        inizializza_stato(x, y);
}

void boss::update_runner()
{

    static constexpr int EVADE_TICKS = 120;
    static constexpr bn::fixed CORNER_MARGIN = 32;
    const collision_map_info& map = get_collision_map(g_schema);

    // --- Direzione: opposta a dog; se è quasi sopra di lui mantiene quella attuale (niente jitter) ---
    if (runner_evade_ticks > 0)
        runner_evade_ticks--;
    else
    {
        bn::fixed dx = chr_x - g_dog->chr_x;
        if (bn::abs(dx) > 8)
            dir = dx > 0 ? DIR_RIGHT : DIR_LEFT;

        // all'angolo (bordo mappa alle spalle): inverte e corre oltre dog per un po'
        bool angolo = (dir == DIR_RIGHT && chr_x > bn::fixed(map.map_w) - CORNER_MARGIN) ||
            (dir == DIR_LEFT && chr_x < CORNER_MARGIN);
        if (angolo)
        {
            dir = -dir;
            runner_evade_ticks = EVADE_TICKS;
        }
    }

    // --- Salta se c'è un ostacolo davanti o un vuoto ---
    bn::fixed x_avanti = chr_x + bn::fixed(dir).multiplication(box_halfdim + 2);
    bool ostacolo = muro_lato == dir || is_solid_at(x_avanti, chr_y, g_schema);
    bool vuoto = !is_solid_at(x_avanti, chr_y + box_dim + 4, g_schema);
    if (onGround && (ostacolo || vuoto))
    {
        chr_vy = ENEMY_BIG_JUMP_VY;
        onGround = false;
    }

    // --- Movimento ---
    chr_accx = onGround ? GROUND_ACCEL : AIR_ACCEL;
    chr_vx += chr_accx.multiplication(dir);
    chr_vx = cap(chr_vx, max_vx);
    chr_x += chr_vx;
    apply_map();
    apply_friction();
    apply_gravity();

    // --- Lancio bombe verso dog (senza cambiare la direzione di fuga) ---
    if (runner_ticks_to_throw > 0)
        runner_ticks_to_throw--;
    if (runner_ticks_to_throw <= 0 && throw_runner_bomb())
        runner_ticks_to_throw = 90 + g_rng.get_int(90);

    // --- Grafica ---
    sprite->set_x(chr_x - HALF_SCREEN_W);
    sprite->set_y(chr_y - HALF_SCREEN_H);
    sprite->set_horizontal_flip(dir == DIR_RIGHT);
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

bool boss::throw_runner_bomb()
{
    bn::fixed dx = g_dog->chr_x - chr_x;
    if (bn::abs(dx) > SCREEN_W)
        return false;                       // dog troppo lontano: riprova al prossimo giro

    bomb_def d;
    d.tipoProjec = BOMB_MINA;
    d.x = chr_x + bn::fixed(8 * dir);
    d.y = chr_y;
    d.vx = SMALL_VX_R * dir;
    d.vy = ZERO;
    d.ignoreGravity = false;
    d.ignoreWalls = false;
    d.max_bounces = 0;
    d.angle = ZERO;   // 0 = niente rotazione, il flip segue il segno di vx

    if (!spawn_bomb(d))
        return false;
    bn::sound_items::fire_lighting.play();
    return true;
}

void boss::update()
{
    if (life <= 0)
    {
        static constexpr int DEATH_FRAMES = 90;
        static constexpr int DEATH_SHRINK_FRAMES = 60;

        invulnerability = 1;
        sprite->set_x(chr_x - HALF_SCREEN_W);
        sprite->set_y(chr_y - HALF_SCREEN_H);

        if (deathTicks > 0)
            deathTicks--;

        int elapsed = DEATH_FRAMES - deathTicks;
        sprite->set_rotation_angle_safe(bn::fixed(elapsed * (dir >= 0 ? -31 : 31)));

        if (deathTicks < DEATH_SHRINK_FRAMES)
        {
            bn::fixed scale = bn::fixed(deathTicks) / DEATH_SHRINK_FRAMES;
            if (scale < bn::fixed(0.1))
                scale = bn::fixed(0.1);
            sprite->set_scale(scale);
            sprite->set_visible(deathTicks % 2);
        }

        if (deathTicks == 0)
        {
            distrutto = true;
            sprite->set_visible(false);
            sprite->remove_affine_mat();
        }
        return;
    }

    switch (tipo)
    {
    case boss_tipo::BALLER:
        update_baller();
        break;
    case boss_tipo::BOMBER:
        update_bomber();
        break;
    case boss_tipo::SUMMONER:
    case boss_tipo::SKELETON:
        update_summoner();
        break;
    case boss_tipo::FLYER:
        update_flyer();
        break;
    case boss_tipo::COWARD:
        update_runner();
        break;
    case boss_tipo::CHARGER:
        update_charger();
        break;
    case boss_tipo::STREGONE:
        update_stregone();
        break;
    case boss_tipo::SLAMMER:
        update_slammer();
        break;
    case boss_tipo::TELEPORTER:
        update_teleporter();
        break;
    case boss_tipo::GUARDIANO:
        update_guardiano();
        break;
    case boss_tipo::SPIRALE:
        update_spirale();
        break;
    default:
        break;
    }

}

void boss::update_summoner()
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
        else if ((attributo & enemy_attr::ATTR_SUMMON) && dist_x < RANGE_SUMMON && ticks_prossimo_summon <= 0 && onGround)
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
    sprite->set_horizontal_flip(dir == DIR_RIGHT);

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

    bomb_def def = { BOMB_BOMB, chr_x + bn::fixed(8).multiplication(dir),
        chr_y - 4, vx, bn::fixed(-4.0), false, false,
        BOMB_MAX_BOUNCES, BOMB_TILE_INDEX, ZERO };
    return spawn_bomb(def) != nullptr;
}

void boss::update_baller()
{
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
    if (ticks_prossimo_attacco <= 0 && throw_baller_fireball())
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
    def.tileIndex = enemy_config::FIREBALL_TILE_INDEX;
    def.angle = -angle;

    if (!spawn_bomb(def))
        return false;
    bn::sound_items::fire_lighting.play();
    return true;

}

void boss::update_flyer()
{
    if (flyer_ticks_to_bomb > 0)
        flyer_ticks_to_bomb--;
    if (flyer_ticks_to_bomb <= 0 && throw_flyer_bomb())
        flyer_ticks_to_bomb = 120;

    if (flyer_ticks_to_fireball > 0)
        flyer_ticks_to_fireball--;
    if (flyer_ticks_to_fireball <= 0)
        flyer_fireball_pending = true;

    const collision_map_info& map = get_collision_map(g_schema);
    bn::fixed target_x = g_dog->chr_x + bn::fixed(g_dog->dir * 80);
    if (target_x < 16)
        target_x = 16;
    else if (target_x > map.map_w - 16)
        target_x = bn::fixed(map.map_w - 16);

    bn::fixed target_y = flyer_fireball_pending ? g_dog->chr_y : g_dog->chr_y - 60;

    if (!flyer_moving)
    {
        if (flyer_move_delay > 0)
            flyer_move_delay--;
        if (flyer_move_delay == 0)
        {
            flyer_start_x = chr_x.integer();
            flyer_start_y = chr_y.integer() ;
            flyer_target_x = target_x.integer();
            flyer_target_y = target_y.integer();
            flyer_move_ticks = 0;
            flyer_moving = true;
        }
    }

    if (flyer_moving)
    {
        static constexpr int MOVE_FRAMES = 90;

        if (flyer_move_ticks < MOVE_FRAMES)
            flyer_move_ticks++;

        bn::fixed progress = bn::fixed(flyer_move_ticks).division(MOVE_FRAMES);
        bn::fixed eased = progress.multiplication(progress).multiplication(
            bn::fixed(3) - bn::fixed(2).multiplication(progress));
        bn::fixed dx = bn::fixed(flyer_target_x - flyer_start_x);
        bn::fixed dy = bn::fixed(flyer_target_y - flyer_start_y);
        chr_x = bn::fixed(flyer_start_x) + dx.multiplication(eased);
        chr_y = bn::fixed(flyer_start_y) + dy.multiplication(eased);

        if (flyer_move_ticks == MOVE_FRAMES)
        {
            flyer_moving = false;
            flyer_move_delay = 30;
        }
    }

    if (flyer_fireball_pending &&
        bn::abs(target_x - chr_x) <= 2 &&
        bn::abs(target_y - chr_y) <= 2 &&
        throw_flyer_fireball())
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

    bomb_def def = { BOMB_BOMB, chr_x, chr_y + 8, vx, ZERO, false, false,
        BOMB_MAX_BOUNCES, BOMB_TILE_INDEX, ZERO };
    return spawn_bomb(def) != nullptr;
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
    def.tileIndex = enemy_config::FIREBALL_TILE_INDEX;
    def.angle = -angle;

    if (!spawn_bomb(def))
        return false;
    bn::sound_items::fire_lighting.play();
    return true;
}

bool boss::dog_in_melee_range() const
{
    return bn::abs(g_dog->chr_x - chr_x) < 70;
}

bool boss::subisci_colpo(int danno)
{

    // invulnerabile mentre è nascosto o in guardia
    if (tipo == boss_tipo::TELEPORTER && stato == TP_NASCOSTO)
        return false;
    if (tipo == boss_tipo::GUARDIANO && stato == GU_GUARDIA)
    {
        bn::sound_items::thud.play();   // feedback: il colpo è parato
        return false;
    }

    if (life <= 0 || invulnerability > 0)
        return false;

    if (danno < 1)
        danno = 1;
    if ((tipo == boss_tipo::CHARGER && stato == CH_STORDITO) ||
        (tipo == boss_tipo::SLAMMER && stato == SL_RECUPERO))
        danno *= 2;
    life -= danno;
    if (life < 0)
        life = 0;
    invulnerability = 30;
    g_healthbar->show(this);
    if (life == 0)
    {
        deathTicks = 90;
        chr_vx = ZERO;
        chr_vy = ZERO;
        chr_accx = ZERO;
        sprite->set_visible(true);
        sprite->set_scale(1);
        sprite->set_rotation_angle_safe(0);
    }
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


// ---------------------------------------------------------------
// Helper comuni
// ---------------------------------------------------------------
void boss::aggiorna_grafica(int anim, int x_offset)
{
    sprite->set_x(chr_x + x_offset - HALF_SCREEN_W);
    sprite->set_y(chr_y - HALF_SCREEN_H);
    sprite->set_horizontal_flip(dir == DIR_RIGHT);   // come update_summoner(): lo zombie guarda a sinistra

    switch (anim)
    {
    case 1:
        actionWalk->update();
        break;
    case 2:
        if (actionSummon.has_value() && !actionSummon->done())
            actionSummon->update();
        break;
    default:
        actionStand->update();
        break;
    }

    if (invulnerability > 0)
    {
        invulnerability--;
        sprite->set_visible(invulnerability % 2);
    }
    else
        sprite->set_visible(true);
}

// dog::update() non controlla il contatto col corpo del boss: lo gestiamo qui
void boss::contatto_cane(int danno)
{
    if (g_dog->invulnerability > 0)
        return;
    if (check_collision_16_boss(*this))
        g_dog->dog_damage_and_jump(danno, g_dog->chr_x < chr_x);
}

// ---------------------------------------------------------------
// CHARGER: pattuglia -> avviso (trema) -> carica -> (muro) stordito
// ---------------------------------------------------------------
void boss::update_charger()
{
    static constexpr bn::fixed CHARGE_SPEED = bn::fixed(3.5);
    static constexpr int AVVISO_TICKS = 45;
    static constexpr int CARICA_TICKS_MAX = 90;
    static constexpr int STORDITO_TICKS = 90;

    bn::fixed dx = g_dog->chr_x - chr_x;
    int x_offset = 0;
    int anim = 0;

    switch (stato)
    {
    case CH_PATTUGLIA:
    {
        if (bn::abs(dx) > 8)
            dir = dx > 0 ? DIR_RIGHT : DIR_LEFT;

        chr_accx = onGround ? GROUND_ACCEL : AIR_ACCEL;
        chr_vx += chr_accx.multiplication(dir);
        chr_vx = cap(chr_vx, max_vx);
        chr_x += chr_vx;
        apply_map();
        apply_friction();
        apply_gravity();

        if (ticks_prossimo_attacco > 0)
            ticks_prossimo_attacco--;
        if (ticks_prossimo_attacco <= 0 && onGround && bn::abs(dx) < SCREEN_W)
        {
            stato = CH_AVVISO;
            stato_ticks = AVVISO_TICKS;
            chr_vx = ZERO;
        }
        anim = (onGround && chr_vx != ZERO) ? 1 : 0;
        break;
    }
    case CH_AVVISO:
    {
        chr_vx = ZERO;
        apply_gravity();

        // si gira verso dog, ma negli ultimi frame la direzione resta bloccata
        if (stato_ticks > 12 && bn::abs(dx) > 8)
            dir = dx > 0 ? DIR_RIGHT : DIR_LEFT;

        x_offset = ((stato_ticks >> 1) & 1) ? 1 : -1;   // vibra
        if (--stato_ticks <= 0)
        {
            stato = CH_CARICA;
            stato_ticks = CARICA_TICKS_MAX;
        }
        break;
    }
    case CH_CARICA:
    {
        chr_vx = CHARGE_SPEED * dir;
        chr_x += chr_vx;
        apply_map();                    // se c'è un muro azzera chr_vx
        bool muro = (chr_vx == ZERO);
        apply_gravity();
        anim = 1;

        if (muro)
        {
            screen_shake(GROUND_POUND_SHAKE_TICKS, GROUND_POUND_SHAKE_AMP);
            bn::sound_items::bomb.play();
            stato = CH_STORDITO;
            stato_ticks = STORDITO_TICKS;
            chr_vy = ENEMY_SMALL_JUMP_VY;   // rimbalzo all'indietro
            onGround = false;
        }
        else if (--stato_ticks <= 0)
        {
            stato = CH_PATTUGLIA;
            ticks_prossimo_attacco = 90 + g_rng.get_int(60);
        }
        break;
    }
    case CH_STORDITO:
    default:
    {
        chr_vx = ZERO;
        apply_gravity();
        if (--stato_ticks <= 0)
        {
            stato = CH_PATTUGLIA;
            ticks_prossimo_attacco = 120;
        }
        break;
    }
    }

    if (stato != CH_STORDITO)
        contatto_cane(stato == CH_CARICA ? 8 : contact_damage);

    aggiorna_grafica(anim, x_offset);
}

// ---------------------------------------------------------------
// STREGONE: pattuglia + evoca fulmini vicino a dog
// ---------------------------------------------------------------
void boss::spawn_fulmini(int quanti)
{
    const collision_map_info& map = get_collision_map(g_schema);

    for (int i = 0; i < quanti; ++i)
    {
        // il primo cade vicino a dog, gli altri sono sfalsati ai lati
        int offset = g_rng.get_int(97) - 48;
        if (i > 0)
            offset += (i & 1) ? 48 : -48;

        bn::fixed x = g_dog->chr_x + offset;
        if (x < 16)
            x = 16;
        else if (x > map.map_w - 16)
            x = bn::fixed(map.map_w - 16);

        bomb_def def;
        def.tipoProjec = BOMB_FULMINE;
        def.x = x;
        def.y = g_dog->chr_y - 100;
        def.vx = ZERO;
        def.vy = ZERO;
        def.ignoreGravity = false;
        def.ignoreWalls = false;
        def.max_bounces = 0;
        def.tileIndex = 11;
        def.angle = ZERO;
        spawn_bomb(def);
    }
}

void boss::update_stregone()
{
    static constexpr bn::fixed PATROL_HALF_RANGE = 96;
    static constexpr int EVOCA_TICKS = 45;

    bn::fixed dx = g_dog->chr_x - chr_x;
    int anim = 0;

    if (stato == SR_PATTUGLIA)
    {
        bn::fixed patrol_left = bn::fixed(patrol_origin_x)  - PATROL_HALF_RANGE;
        bn::fixed patrol_right = bn::fixed(patrol_origin_x)  + PATROL_HALF_RANGE;

        if ((dir == DIR_RIGHT && chr_x >= patrol_right) ||
            (dir == DIR_LEFT && chr_x <= patrol_left) ||
            (muro_lato == dir && onGround))
        {
            dir = -dir;
            chr_vx = ZERO;
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
        if (ticks_prossimo_attacco <= 0 && onGround && bn::abs(dx) < SCREEN_W)
        {
            stato = SR_EVOCA;
            stato_ticks = EVOCA_TICKS;
            chr_vx = ZERO;
            dir = dx > 0 ? DIR_RIGHT : DIR_LEFT;
            if (actionSummon.has_value())
                actionSummon->reset();
        }
        anim = (onGround && chr_vx != ZERO) ? 1 : 0;
    }
    else
    {
        chr_vx = ZERO;
        apply_gravity();
        anim = 2;

        if (--stato_ticks <= 0)
        {
            // più è ferito, più fulmini e più spesso
            int percentuale = life * 100 / START_LIFE;
            int quanti = 1 + (percentuale < 66 ? 1 : 0) + (percentuale < 33 ? 1 : 0);
            spawn_fulmini(quanti);
            lampo_start();
            bn::sound_items::fire_lighting.play();

            chr_vy = ENEMY_SMALL_JUMP_VY;   // saltello alla evocazione
            onGround = false;

            ticks_prossimo_attacco = 60 + (80 * life) / START_LIFE;
            stato = SR_PATTUGLIA;
        }
    }

    contatto_cane(contact_damage);
    aggiorna_grafica(anim);
}

// ---------------------------------------------------------------
// SLAMMER: salto verso dog, onda d'urto radente all'atterraggio
// ---------------------------------------------------------------
void boss::update_slammer()
{
    static constexpr int SALTO_TICKS_MAX = 120;
    static constexpr int RECUPERO_TICKS = 45;
    static constexpr bn::fixed ONDA_SPEED = bn::fixed(2.0);

    bn::fixed dx = g_dog->chr_x - chr_x;
    int anim = 0;

    switch (stato)
    {
    case SL_ATTESA:
    {
        if (bn::abs(dx) > 8)
            dir = dx > 0 ? DIR_RIGHT : DIR_LEFT;

        chr_vx = ZERO;
        apply_gravity();

        if (ticks_prossimo_attacco > 0)
            ticks_prossimo_attacco--;
        if (ticks_prossimo_attacco <= 0 && onGround && bn::abs(dx) < SCREEN_W)
        {
            // arco di ~50 frame: orizzontale proporzionale alla distanza
            set_salto_vx(cap(dx.division(50), bn::fixed(3)));
            chr_vy = ENEMY_BIG_JUMP_VY;
            onGround = false;
            stato = SL_SALTO;
            stato_ticks = SALTO_TICKS_MAX;
        }
        break;
    }
    case SL_SALTO:
    {
        chr_vx = salto_vx();
        chr_x += chr_vx;
        apply_map();
        apply_gravity();
        anim = 1;
        --stato_ticks;

        if (atterrato_ora || stato_ticks <= 0)
        {
            screen_shake(GROUND_POUND_SHAKE_TICKS, GROUND_POUND_SHAKE_AMP);
            bn::sound_items::bomb.play();

            // onde radenti a destra e a sinistra: dog le schiva saltando
            for (int d = -1; d <= 1; d += 2)
            {
                bomb_def def;
                def.tipoProjec = BOMB_FIREBALL;
                def.x = chr_x + bn::fixed(d * 10);
                def.y = chr_y + box_dim - 6;
                def.vx = ONDA_SPEED * d;
                def.vy = ZERO;
                def.ignoreGravity = true;
                def.ignoreWalls = true;
                def.max_bounces = 0;
                def.tileIndex = enemy_config::FIREBALL_TILE_INDEX;
                def.angle = ZERO;
                spawn_bomb(def);
            }

            // se dog è proprio sotto l'impatto, prende danno diretto
            if (g_dog->invulnerability == 0 &&
                bn::abs(g_dog->chr_x - chr_x) < 24 &&
                bn::abs(g_dog->chr_y - chr_y) < 24)
                g_dog->dog_damage_and_jump(10, g_dog->chr_x < chr_x);

            chr_vx = ZERO;
            stato = SL_RECUPERO;
            stato_ticks = RECUPERO_TICKS;
        }
        break;
    }
    case SL_RECUPERO:
    default:
    {
        chr_vx = ZERO;
        apply_gravity();
        if (--stato_ticks <= 0)
        {
            stato = SL_ATTESA;
            ticks_prossimo_attacco = 60 + g_rng.get_int(60);
        }
        break;
    }
    }

    contatto_cane(contact_damage);
    aggiorna_grafica(anim);
}

// ---------------------------------------------------------------
// TELEPORTER: visibile -> nascosto -> ricompare dietro dog -> raffica
// ---------------------------------------------------------------
void boss::update_teleporter()
{
    static constexpr int NASCOSTO_TICKS = 40;
    static constexpr int RAFFICA_INTERVALLO = 14;
    static constexpr int RAFFICA_TICKS = 20 + 3 * RAFFICA_INTERVALLO;   // 20 di avviso + 3 colpi

    bn::fixed dx = g_dog->chr_x - chr_x;
    int anim = 0;

    switch (stato)
    {
    case TP_VISIBILE:
    {
        if (bn::abs(dx) > 8)
            dir = dx > 0 ? DIR_RIGHT : DIR_LEFT;
        chr_vx = ZERO;
        apply_gravity();

        if (ticks_prossimo_attacco > 0)
            ticks_prossimo_attacco--;
        if (ticks_prossimo_attacco <= 0 && onGround)
        {
            stato = TP_NASCOSTO;
            stato_ticks = NASCOSTO_TICKS;
        }
        break;
    }
    case TP_NASCOSTO:
    {
        chr_vx = ZERO;
        apply_gravity();

        if (--stato_ticks <= 0)
        {
            // ricompare alle spalle di dog, o davanti se dietro c'è un muro
            const collision_map_info& mappa = get_collision_map(g_schema);
            int lato = -g_dog->dir;
            for (int tentativo = 0; tentativo < 2; ++tentativo)
            {
                bn::fixed x = g_dog->chr_x + bn::fixed(lato * 48);
                if (x > 16 && x < mappa.map_w - 16 && !is_solid_at(x, g_dog->chr_y, g_schema))
                {
                    chr_x = x;
                    chr_y = g_dog->chr_y;
                    chr_vy = ZERO;
                    break;
                }
                lato = -lato;
            }
            dir = chr_x < g_dog->chr_x ? DIR_RIGHT : DIR_LEFT;
            stato = TP_RAFFICA;
            stato_ticks = RAFFICA_TICKS;
        }
        break;
    }
    default:   // TP_RAFFICA
    {
        chr_vx = ZERO;
        apply_gravity();
        anim = 2;

        dir = dx > 0 ? DIR_RIGHT : DIR_LEFT;

        // dopo 20 tick di avviso spara a 42, 28, 14
        if (stato_ticks <= 3 * RAFFICA_INTERVALLO && stato_ticks % RAFFICA_INTERVALLO == 0)
            throw_baller_fireball();

        if (--stato_ticks <= 0)
        {
            stato = TP_VISIBILE;
            ticks_prossimo_attacco = 100 + g_rng.get_int(60);
        }
        break;
    }
    }

    if (stato != TP_NASCOSTO)
        contatto_cane(contact_damage);

    aggiorna_grafica(anim);
    if (stato == TP_NASCOSTO)
        sprite->set_visible(false);
}

// ---------------------------------------------------------------
// GUARDIANO: guardia (invulnerabile, fermo) <-> attacco (insegue e lancia bombe)
// ---------------------------------------------------------------
void boss::update_guardiano()
{
    static constexpr int GUARDIA_TICKS = 120;
    static constexpr int ATTACCO_TICKS = 180;
    static constexpr int BOMBA_INTERVALLO = 60;

    bn::fixed dx = g_dog->chr_x - chr_x;
    int anim = 0;

    if (bn::abs(dx) > 8)
        dir = dx > 0 ? DIR_RIGHT : DIR_LEFT;

    if (stato == GU_GUARDIA)
    {
        chr_vx = ZERO;
        apply_gravity();

        if (--stato_ticks <= 0)
        {
            stato = GU_ATTACCO;
            stato_ticks = ATTACCO_TICKS;
            ticks_prossimo_attacco = 30;
            max_vx = bn::fixed(1.2);
        }
    }
    else
    {
        if (onGround && muro_lato == dir)
        {
            chr_vy = ENEMY_BIG_JUMP_VY;
            onGround = false;
        }

        chr_accx = onGround ? GROUND_ACCEL : AIR_ACCEL;
        chr_vx += chr_accx.multiplication(dir);
        chr_vx = cap(chr_vx, max_vx);
        chr_x += chr_vx;
        apply_map();
        apply_friction();
        apply_gravity();
        anim = (onGround && chr_vx != ZERO) ? 1 : 0;

        if (ticks_prossimo_attacco > 0)
            ticks_prossimo_attacco--;
        if (ticks_prossimo_attacco <= 0 && throw_bomber_bomb())
            ticks_prossimo_attacco = BOMBA_INTERVALLO;

        if (--stato_ticks <= 0)
        {
            stato = GU_GUARDIA;
            stato_ticks = GUARDIA_TICKS;
            max_vx = bn::fixed(0.5);
            chr_vx = ZERO;
        }
    }

    contatto_cane(stato == GU_ATTACCO ? contact_damage + 2 : contact_damage);
    aggiorna_grafica(anim);
}

// ---------------------------------------------------------------
// SPIRALE: fermo spara anelli rotanti, poi salta di lato
// ---------------------------------------------------------------
void boss::spara_anello(int n)
{
    for (int i = 0; i < n; ++i)
    {
        int a = (angolo_raggi + i * 360 / n) % 360;
        bn::fixed angolo = bn::fixed(a);

        bomb_def def;
        def.tipoProjec = BOMB_ORB;
        def.x = chr_x;
        def.y = chr_y + enemy_config::SPAWN_Y_OFFSET;
        def.vx = bn::degrees_lut_cos_safe(angolo).multiplication(enemy_config::FIREBALL_SPEED);
        def.vy = bn::degrees_lut_sin_safe(angolo).multiplication(enemy_config::FIREBALL_SPEED);
        def.ignoreGravity = true;
        def.ignoreWalls = true;
        def.max_bounces = 0;
        def.tileIndex = enemy_config::FIREBALL_TILE_INDEX;
        def.angle = ZERO;
        spawn_bomb(def);   // se il pool è pieno, il colpo salta
    }
    bn::sound_items::fire_lighting.play();
}

void boss::update_spirale()
{
    static constexpr int FERMO_TICKS = 180;
    static constexpr int SPOSTA_TICKS_MAX = 120;

    bool furioso = life * 2 < START_LIFE;   // seconda fase sotto il 50%
    bn::fixed dx = g_dog->chr_x - chr_x;
    int anim = 0;

    if (stato == SP_FERMO)
    {
        chr_vx = ZERO;
        apply_gravity();
        anim = 2;
        dir = dx > 0 ? DIR_RIGHT : DIR_LEFT;

        if (ticks_prossimo_attacco > 0)
            ticks_prossimo_attacco--;
        if (ticks_prossimo_attacco <= 0)
        {
            spara_anello(furioso ? 8 : 6);
            angolo_raggi = (angolo_raggi + 20) % 360;   // l'anello successivo è ruotato
            ticks_prossimo_attacco = furioso ? 25 : 40;
        }

        if (--stato_ticks <= 0 && onGround)
        {
            // vicino a dog si allontana, lontano gli si avvicina
            bn::fixed verso = bn::abs(dx) < 96 ? -dx : dx;
            set_salto_vx(cap(verso.division(50), bn::fixed(3)));
            chr_vy = ENEMY_BIG_JUMP_VY;
            onGround = false;
            stato = SP_SPOSTA;
            stato_ticks = SPOSTA_TICKS_MAX;
        }
    }
    else
    {
        chr_vx = salto_vx();
        chr_x += chr_vx;
        apply_map();
        apply_gravity();
        anim = 1;

        if (atterrato_ora || --stato_ticks <= 0)
        {
            chr_vx = ZERO;
            stato = SP_FERMO;
            stato_ticks = FERMO_TICKS;
            ticks_prossimo_attacco = 30;
        }
    }

    contatto_cane(contact_damage);
    aggiorna_grafica(anim);
}

void boss::inizializza_stato(bn::fixed x, bn::fixed y)
{
    switch (tipo)
    {
    case boss_tipo::BOMBER:
        bomber_ticks_to_jump = 180;
        bomber_ticks_to_throw = 60;
        bomber_shots_in_burst = 0;
        bomber_throw_animation_ticks = 0;
        break;

    case boss_tipo::FLYER:
        flyer_ticks_to_bomb = 120;
        flyer_ticks_to_fireball = 420;
        flyer_move_delay = 30;
        flyer_move_ticks = 0;
        flyer_fireball_pending = false;
        flyer_moving = false;
        flyer_start_x = flyer_target_x = x.integer();
        flyer_start_y = flyer_target_y = y.integer();
        break;

    case boss_tipo::COWARD:
        runner_ticks_to_throw = 90;
        runner_evade_ticks = 0;
        break;

    case boss_tipo::SUMMONER:
    case boss_tipo::SKELETON:
        in_summon = false;
        summon_ticks = 0;
        ticks_prossimo_summon = 150;
        ticks_prossimo_saltello = 60;
        ticks2dir = 0;
        break;

    default:   // BALLER e tutti i boss a stati/fasi
        patrol_origin_x = x.integer();
        stato = 0;                 // CH_PATTUGLIA, SR_PATTUGLIA, SL_ATTESA, TP_VISIBILE, GU_GUARDIA, SP_FERMO
        stato_ticks = (tipo == boss_tipo::GUARDIANO || tipo == boss_tipo::SPIRALE) ? 120 : 0;
        contatore = 0;
        angolo_raggi = 0;
        salto_vx_data = 0;
        fase_boss = 0;
        break;
    }
}