#include "globals.h"
#include "enemy.h"
#include "bau.h"
#include "dog.h"
#include "item.h"
#include "schemi.h"
#include "healthbar.h"
#include "bn_random.h"
#include "bn_sprite_items_weapons.h"
#include "bn_sprite_items_enemies.h"
#include "bn_sprite_items_king.h"
#include "bn_sprite_items_enemies2.h"
#include "bn_math.h"
#include "bn_log.h"


static void spawn_drop(bn::fixed x, bn::fixed y)
{
    item* it = nullptr;

    // riusa un drop inattivo, altrimenti ne crea uno nuovo se c'è posto
    for (item* i : *g_items)
        if (i->is_drop && !i->active) { it = i; break; }

    if (!it)
    {
        if (g_items->full()) return;
        item_def def = { ITEM_TIPO_ENERGIA, 0, 0, 0 };
        it = new item(def);
        g_items->push_back(it);
    }
    it->drop_at(x, y);
}

enemy::enemy(const enemy_def& def)
{
    tipo = def.tipo;
    attributo = def.attributo;

    switch (tipo)
    {
    case enemy_tipo::MOVE_CHASE:
    {
        int N = 3 * 3;
        sprite = bn::sprite_items::enemies2.create_sprite(chr_x, chr_y, 0);
        actionStand = bn::create_sprite_animate_action_forever(
            *sprite, 6, bn::sprite_items::enemies2.tiles_item(), N + 1, N + 1);
        actionWalk = bn::create_sprite_animate_action_forever(
            *sprite, 8, bn::sprite_items::enemies2.tiles_item(), N + 0, N + 1, N + 2, N + 1);


        break;
    }
    case enemy_tipo::MOVE_CHASE_SHY:
    {
        int N = 7 * 3;
        sprite = bn::sprite_items::enemies.create_sprite(chr_x, chr_y, 0);
        actionStand = bn::create_sprite_animate_action_forever(
            *sprite, 6, bn::sprite_items::enemies.tiles_item(), N + 1, N + 1);
        actionWalk = bn::create_sprite_animate_action_forever(
            *sprite, 8, bn::sprite_items::enemies.tiles_item(), N + 0, N + 1, N + 2, N + 1);
        break;
    }
    case enemy_tipo::MOVE_PATROL:
    {
        int N = 2 * 3;
        sprite = bn::sprite_items::enemies.create_sprite(chr_x, chr_y, 0);
        actionStand = bn::create_sprite_animate_action_forever(
            *sprite, 6, bn::sprite_items::enemies.tiles_item(), N + 1, N + 1);
        actionWalk = bn::create_sprite_animate_action_forever(
            *sprite, 6, bn::sprite_items::enemies.tiles_item(), N + 0, N + 1, N + 2, N + 1);
        break;
    }
    case enemy_tipo::MOVE_STATIC:
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
    case enemy_tipo::MOVE_STATIC_JUMP:
    {
        int N = 0;
        sprite = bn::sprite_items::enemies.create_sprite(chr_x, chr_y, 0);
        actionStand = bn::create_sprite_animate_action_forever(
            *sprite, 6, bn::sprite_items::enemies.tiles_item(), N + 1, N + 1);
        actionWalk = bn::create_sprite_animate_action_forever(
            *sprite, 8, bn::sprite_items::enemies.tiles_item(), N + 0, N + 1, N + 2, N + 1);

        break;
    }


    default: // TIPO_NEMICO_GENERICO
    {
        int N = 3;
        sprite = bn::sprite_items::enemies.create_sprite(chr_x, chr_y, 0);
        actionStand = bn::create_sprite_animate_action_forever(
            *sprite, 6, bn::sprite_items::enemies.tiles_item(), N + 1, N + 1);
        actionWalk = bn::create_sprite_animate_action_forever(
            *sprite, 3, bn::sprite_items::enemies.tiles_item(), N + 0, N + 1, N + 2, N + 1);
        break;
    }
    }

    sprite->set_bg_priority(1);
    sprite->set_camera(g_camera);

    init(def);
    box_dim = bn::fixed(8);
    box_halfdim = bn::fixed(4);
}

void enemy::init(const enemy_def& def)
{
    chr_y = bn::fixed(def.y);
    chr_x = bn::fixed(def.x);
    ticks2action = def.delay;
    currentAction = ACTION_MOVE;
}

void enemy::update()
{

    if (is_dead())
    {
        if (deathTicks > 0 || sprite->visible())
            update_death();
        return;
    }

    if (vita_residua_ticks >= 0)
    {
        vita_residua_ticks--;
        if (vita_residua_ticks <= 0)
        {
            die(dir);
            return;
        }
    }

    // il dog stordito/invulnerabile non cambia nulla qui; lo STUN invece blocca IA e attacco
    update_physics();

    if (currentAction != ACTION_STUN)
    {
        update_movement();
        update_attack();
    }
    else if (--ticks2action <= 0)
        currentAction = ACTION_STAND;

    update_projectile();
    update_graphics();
}

void enemy::update_physics()
{
    apply_friction();
    chr_vx += chr_accx;
    chr_vx = cap(chr_vx, max_vx);
    chr_x += chr_vx;
    apply_map();
    apply_gravity();

    if (invulnerability > 0) invulnerability--;
}

// ---------------------------------------------------------------
void enemy::update_graphics()
{
    sprite->set_x(chr_x - HALF_SCREEN_W);
    sprite->set_y(chr_y - HALF_SCREEN_H);
    sprite->set_horizontal_flip(dir == DIR_LEFT);

    if (onGround && chr_vx.integer() == 0) actionStand->update();
    else                                   actionWalk->update();

    // lampeggi (invulnerabilità, stun, proiettile in scadenza)
    if (invulnerability > 0)               sprite->set_visible(invulnerability % 2);
    else if (currentAction == ACTION_STUN) sprite->set_visible(ticks2action % 2);
    else                                   sprite->set_visible(true);

}
// ---------------------------------------------------------------
void enemy::update_projectile()
{

    if (bomba) {
        bomba->update();
        if (bomba->finished)
            bomba.reset();
    }



}


void enemy::update_movement()
{
    switch (tipo)
    {
    case MOVE_STATIC:       move_static();      break;
    case MOVE_STATIC_JUMP:  move_static_jump(); break;
    case MOVE_PATROL:       move_patrol();      break;
    case MOVE_CHASE:        move_chase();       break;
    case MOVE_CHASE_SHY:    move_chase_shy();   break;
    case MOVE_MELEE:        move_melee();       break;
    default: break;
    }
}

void enemy::move_static()
{
    chr_accx = 0;
    if (dog_in_sight()) face_dog();
}

void enemy::move_static_jump()
{
    chr_accx = 0;
    if (dog_in_sight()) face_dog();
    if (jump_cd > 0) jump_cd--;
    else if (onGround) { chr_vy = bn::fixed(-3.5); jump_cd = jump_period; }
}

void enemy::move_patrol()
{
    // inverti se sbatte contro un muro o se gradino

    const collision_map_info& mappa = get_collision_map(g_schema);
    // Punto di controllo: un po' oltre il "muso", nella direzione di marcia
    bn::fixed x_avanti = chr_x + bn::fixed(8 * dir);
    bn::fixed y_sotto = chr_y + box_halfdim + bn::fixed(4);   // poco sotto i piedi

    bool muro_o_bordo = (x_avanti <= 0) || (x_avanti >= bn::fixed(mappa.map_w))
        || is_solid_at(x_avanti, chr_y, g_schema);

    bool gradino = !is_solid_at(x_avanti, y_sotto, g_schema);   // niente terreno sotto: è un bordo di piattaforma

    if (muro_o_bordo || gradino) {
        dir = -dir;
    }

    walk_towards(dir, bn::fixed(0.2), bn::fixed(1.0));
}

void enemy::move_chase()
{
    if (!dog_in_sight()) { chr_accx = 0; return; }
    face_dog();
    walk_towards(dir, bn::fixed(0.15), bn::fixed(1.2));
}

void enemy::move_chase_shy()
{
    // Boo: si ferma se dog lo sta guardando, avanza solo se gli dà le spalle
    if (!dog_in_sight() || dog_is_looking_at_me()) { chr_accx = 0; chr_vx = 0; return; }
    face_dog();
    walk_towards(dir, bn::fixed(0.15), bn::fixed(0.8));
}

void enemy::move_melee()
{
    if (!dog_in_sight()) { chr_accx = 0; return; }
    face_dog();
    if (dist_to_dog() > melee_range)
        walk_towards(dir, bn::fixed(0.2), bn::fixed(1.3));
    else
        chr_accx = 0;          // in range: si pianta e lascia fare ad update_attack()
}

void enemy::update_attack()
{
    // ATTR_CONTACT non fa nulla qui: il danno da contatto lo gestisce dog::update()
    // con check_collision_16(*enem->sprite, *this) quando weaponSprite è assente/inattivo.

    if (attack_cd > 0) { attack_cd--; return; }
    if (!(attributo & (ATTR_PROJ_MASK | ATTR_MELEE))) return;
    if (!dog_in_sight()) return;

    // per MOVE_MELEE attacca solo quando è in range
    if (tipo == MOVE_MELEE && dist_to_dog() > melee_range) return;

    currentAction = ACTION_ATTACK;
    face_dog();

    if (has(ATTR_MELEE)) {
        
    }
    else {
        fire(pick_projectile());
    }
    attack_cd = attack_period;
}

/*
void update_old()
{




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
        if (tipo == TIPO_NEMICO_DRUIDO_DINAMICO || tipo == TIPO_NEMICO_DRUIDO_STATICO || tipo == TIPO_NEMICO_SPADACCINO_PATTUGLIATORE) {
            // dont check for map collision
        }
        else if (weapon_hits_map(wx_base, wy_base, g_schema)) {
            wpn_vx = 0;
            wpn_vy = 0;
        }
    }
    static constexpr bn::fixed ENEMY_ACTIVATION_RANGE_X = 240;
    // Da qui in poi: corpo del nemico e decisioni AI, congelabili se lontano dal cane
    if (bn::abs(chr_x - g_dog->chr_x) > ENEMY_ACTIVATION_RANGE_X)
        return;

    if (tipo == TIPO_NEMICO_PATTUGLIATORE ||
        tipo == TIPO_NEMICO_BLOB_PATTUGLIATORE) {
        update_pattugliatore();
        return;
    }
    {
        chr_vx += chr_accx;
        chr_vx = cap(chr_vx, max_vx);
        chr_x += chr_vx;
    }
    apply_map();
    apply_gravity();
    apply_friction();

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
                    chr_vy = ENEMY_BIG_JUMP_VY;
                    chr_accx = bn::fixed(0.3).multiplication(dir);
                    max_vx = g_rng.get_fixed(bn::fixed(1.0)) + bn::fixed(0.5);
                    break;
                case TIPO_NEMICO_BLOB_PATTUGLIATORE:
                    dir = -dir;
                    chr_accx = bn::fixed(0.2).multiplication(dir);
                    max_vx = bn::fixed(1.0);
                    chr_vy = ENEMY_SMALL_JUMP_VY;
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
                case TIPO_NEMICO_ZOMBIE:
                case TIPO_NEMICO_PUGILE:
                    dir = (g_dog->chr_x > chr_x) ? DIR_RIGHT : DIR_LEFT;   // insegue il cane
                    chr_vx = ZERO;
                    chr_accx = bn::fixed(0.2).multiplication(dir);
                    max_vx = bn::fixed(1.0);
                    ticks2action = 60;
                    break;
                case TIPO_NEMICO_DRUIDO_STATICO:
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

                    if (attributo & enemy_attr::ATTR_AIM) {
                        bn::fixed angle = bn::degrees_atan2(dy.integer(), dx.integer());

                        wpn_vx = bn::degrees_lut_cos_safe(angle).multiplication(gittata).division(weapon_time + weapon_time);
                        wpn_vy = bn::degrees_lut_sin_safe(angle).multiplication(gittata).division(weapon_time + weapon_time);

                        weaponSprite->set_rotation_angle_safe(-90 - angle);
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
    else if (vita_residua_ticks > 0 && vita_residua_ticks < 60)   // ultimo secondo: lampeggia, avvisa che sta per sparire
        sprite->set_visible(vita_residua_ticks % 2);
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
*/

void enemy::takeDamage(int damage) {
    life -= damage;
    if (life <= 0) {
        //if (vita_residua_ticks)
        vita_residua_ticks = invulnerability;
        die(-dir);
    }
    g_healthbar->show(this);
}

void enemy::beHitByBark(int _dir, bn::fixed power)
{
    chr_vy = ENEMY_MEDIUM_JUMP_VY - power.multiplication(bn::fixed(1.5));
    chr_vx = (SMALL_VX_R + power.multiplication(SMALL_VX_R)).multiplication(_dir);

    chr_accx = bn::fixed(0);

    currentAction = ACTION_STUN;
    ticks2action =
        invulnerability = 60;   // per l'intera durata dello stordimento non collide col corpo del cane


    int damage = (BAU_DAMAGE + power.multiplication(BAU_DAMAGE)).integer();
    takeDamage(damage);
}

void enemy::beHitByDog()
{
    // questo è il contatto fisico con il cane, non un colpo di arma: il nemico viene respinto e stordito
    chr_vy = ENEMY_MEDIUM_JUMP_VY;
    chr_vx = g_dog->chr_vx > 0 ? SMALL_VX_R : SMALL_VX_L;
    chr_accx = bn::fixed(0);

    currentAction = ACTION_STUN;
    ticks2action = invulnerability = 60;   // per l'intera durata dello stordimento non collide col corpo del cane

    takeDamage(10);
    g_healthbar->show(this);
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

void enemy::throw_arrow()
{
    if (bomba) return;   // un colpo alla volta

    bn::fixed dx = g_dog->chr_x - chr_x;
    dir = (dx > 0) ? DIR_RIGHT : DIR_LEFT;
    bn::fixed  speed = bn::fixed(2.0);
    bn::fixed vx;
    bn::fixed vy = ZERO;
    bn::fixed angle = 0;
    bool aim = has(ATTR_AIM);
    if (aim) {
        bn::fixed dy = g_dog->chr_y - chr_y;
        angle = bn::degrees_atan2(dy.integer(), dx.integer());

        vx = bn::degrees_lut_cos_safe(angle).multiplication(speed);
        vy = bn::degrees_lut_sin_safe(angle).multiplication(speed);
    }
    else {
        vx = speed.multiplication(dir);
    }
    bomb_def def = { chr_x, chr_y - 4, vx, vy, true, false, 0, 120, 4, -angle - 90 };

    bomba.emplace(def);
}

void enemy::throw_fireball()
{
    if (bomba) return;   // un colpo alla volta

    bn::fixed dx = g_dog->chr_x - chr_x;
    dir = (dx > 0) ? DIR_RIGHT : DIR_LEFT;

    bn::fixed speed = bn::fixed(1.5);
    bn::fixed vx;
    bn::fixed vy = ZERO;
    bn::fixed angle = ZERO;
    bool aim = has(ATTR_AIM);
    if (aim) {
        bn::fixed dy = g_dog->chr_y - chr_y;
        angle = bn::degrees_atan2(dy.integer(), dx.integer());

        vx = bn::degrees_lut_cos_safe(angle).multiplication(speed);
        vy = bn::degrees_lut_sin_safe(angle).multiplication(speed);
    }
    else {
        vx = speed.multiplication(dir);
    }
    bomb_def def = { chr_x, chr_y - 4, vx, vy, true, true, 0, 120, 23, -angle - 90 };

    bomba.emplace(def);
}


bool enemy::dog_in_melee_range() const
{
    return bn::abs(g_dog->chr_x - chr_x) < 48;
}

void enemy::beHitByShockwave(int _dir)
{
    chr_vy = ENEMY_MEDIUM_JUMP_VY;                          // saltello
    chr_vx = SMALL_VX_R.multiplication(bn::fixed(_dir));    // lontano dal cane
    chr_accx = bn::fixed(0);

    currentAction = ACTION_STUN;
    ticks2action = invulnerability = ONDA_URTO_STUN_TICKS;  // 30 frame, niente danno al nemico
    takeDamage(5);
}


void enemy::die(int _dir)
{
    vita_residua_ticks = 0;
    currentAction = ACTION_DEAD;
    deathTicks = ENEMY_DEATH_FRAMES;
    invulnerability = 0;
    chr_accx = ZERO;
    chr_vx = ENEMY_SMALL_JUMP_VY.multiplication(_dir);
    chr_vy = ENEMY_MEDIUM_JUMP_VY;

    sprite->set_visible(true);

        // if (g_rng.get_int(100) < ENEMY_DROP_CHANCE)
        spawn_drop(chr_x, chr_y - 8);
}

void enemy::update_death()
{
    // l'enemy continua a seguire la fisica (parabola + rimbalzo sul terreno)
    chr_x += chr_vx;
    apply_gravity();
    apply_map();
    if (onGround)
        chr_vx = chr_vx.multiplication(bn::fixed(0.9));

    sprite->set_x(chr_x - HALF_SCREEN_W);
    sprite->set_y(chr_y - HALF_SCREEN_H);

    if (deathTicks > 0)
        deathTicks--;

    // rotazione: gira nel verso in cui è stato colpito
    int elapsed = ENEMY_DEATH_FRAMES - deathTicks;
    sprite->set_rotation_angle_safe(bn::fixed(elapsed * (chr_vx >= 0 ? -31 : 31)));

    // ultimi frame: si rimpicciolisce e lampeggia
    if (deathTicks < ENEMY_DEATH_SHRINK)
    {
        bn::fixed s = bn::fixed(deathTicks) / ENEMY_DEATH_SHRINK;
        if (s < bn::fixed(0.1)) s = bn::fixed(0.1);   // scala 0 non è valida
        sprite->set_scale(s);
        sprite->set_visible(deathTicks % 2);
    }

    if (deathTicks == 0)
    {
        distrutto = true;
        sprite->set_visible(false);
        sprite->remove_affine_mat();   // libera l'affine mat (sono solo 32 sulla GBA)
    }
}


// Helper
bool enemy::dog_in_sight() const { return dist_to_dog() <= sight_range; }
bn::fixed enemy::dist_to_dog() const { return bn::abs(g_dog->chr_x - chr_x); }
void enemy::face_dog() { dir = (g_dog->chr_x > chr_x) ? DIR_RIGHT : DIR_LEFT; }

bool enemy::dog_is_looking_at_me() const
{
    // dog guarda a destra e io sono a destra, o viceversa
    return (g_dog->dir == DIR_RIGHT && chr_x > g_dog->chr_x) ||
        (g_dog->dir == DIR_LEFT && chr_x < g_dog->chr_x);
}

void enemy::walk_towards(int d, bn::fixed accel, bn::fixed vmax)
{
    dir = d;
    chr_accx = accel.multiplication(d);
    max_vx = vmax;
}

// Scudo: chiamato da bau::update() prima di beHitByBark()
bool enemy::blocks_bark(int from_dir) const
{
    // il bau arriva da from_dir (lato dog); para se il nemico guarda verso di lui
    return has(ATTR_SHIELD) && dir == -from_dir && currentAction != ACTION_STUN;
}


// ---------------------------------------------------------------
int16_t enemy::pick_projectile() const
{
    // se più bit sono accesi, scegli a caso tra quelli disponibili
    int16_t avail[4]; int n = 0;
    if (has(ATTR_ARROW))    avail[n++] = ATTR_ARROW;
    if (has(ATTR_BOMB))     avail[n++] = ATTR_BOMB;
    if (has(ATTR_SHELL))    avail[n++] = ATTR_SHELL;
    if (has(ATTR_FIREBALL)) avail[n++] = ATTR_FIREBALL;
    return n ? avail[g_rng.get_int(n)] : 0;
}

void enemy::fire(int16_t type)
{
    switch (type) {
    case ATTR_BOMB:
        throw_bomb();
        return;
    case ATTR_SHELL:
        throw_shell();
        return;
    case ATTR_ARROW:
        throw_arrow();
        return;
    case ATTR_FIREBALL:
        throw_fireball();
        return;
    default:
        break;
    }

}
