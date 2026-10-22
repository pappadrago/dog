#pragma once

#include "bn_optional.h"
#include "bn_sprite_ptr.h"
#include "bn_sprite_animate_actions.h"
#include "bn_fixed.h"
#include "game_constants.h"
#include "obj.h"
#include "bomb.h"

class ball;
class bau;
class dog;


// ---------- TIPO = movimento ----------
enum enemy_tipo : uint8_t
{
    MOVE_STATIC = 0,       // fermo
    MOVE_STATIC_JUMP,      // fermo, salta ogni tanto
    MOVE_PATROL,           // avanti/indietro entro patrol_range da home_x
    MOVE_CHASE,            // insegue sempre dog (entro sight_range)
    MOVE_CHASE_SHY,        // insegue solo se dog NON lo guarda
    MOVE_MELEE,            // si avvicina fino a melee_range, poi si ferma e attacca
    MOVE_GHOST,            // si avvicina passando attraverso i muri
    MOVE_GHOST_SHY         // si avvicina passando attraverso i muri solo se dog NON lo guarda
};

// ---------- ATTRIBUTO = bitmask attacco/difese ----------
enum enemy_attr : int16_t
{
    ATTR_NONE = 0,
    ATTR_CONTACT = 1 << 0,   // danno per scontro fisico (nessuna arma)
    ATTR_MELEE = 1 << 1,   // arma a mano (spada che ruota)
    ATTR_ARROW = 1 << 2,   // proiettile dritto, non rimbalza, muore sul muro
    ATTR_BOMB = 1 << 3,   // parabola + gravità + N rimbalzi + esplosione
    ATTR_SHELL = 1 << 4,   // scivola a terra, rimbalza sui muri
    ATTR_FIREBALL = 1 << 5,   // rimbalza sul pavimento, non sui muri
    ATTR_AIM = 1 << 6,   // modificatore: mira a dog (altrimenti dritto)
    ATTR_SHIELD = 1 << 7,   // para il bau se frontale
    ATTR_SUMMON = 1 << 8,
    ATTR_RESPAWN = 1 << 9,
    ATTR_PATROL = 1 << 10,
    ATTR_BASH = 1 << 11,
    ATTR_GRAVITY_ARROW = 1 << 12,
    ATTR_PROJ_MASK = ATTR_ARROW |ATTR_GRAVITY_ARROW | ATTR_BOMB | ATTR_SHELL | ATTR_FIREBALL
    // bit 15 = segno di int16_t: usa solo bit 0..14
};

#define WEAPON_FLICK_TIME 25

#define ACTION_STAND 0
#define ACTION_MOVE 1
#define ACTION_ATTACK 2
#define ACTION_STUN 3
#define ACTION_DEAD 4

#define MELEE_NONE     0
#define MELEE_WINDUP   1
#define MELEE_ACTIVE   2
#define MELEE_RECOVERY 3


#define ENEMY_DEATH_FRAMES 90
#define ENEMY_DEATH_SHRINK 60   // ultimi frame in cui si rimpicciolisce


// Definizione statica di un nemico posizionato in uno schema.
// Le tabelle per schema stanno in <schema>_enemies.h (es. s1_enemies.h)
struct enemy_def
{
    uint8_t  tipo;             // TIPO_NEMICO_*
    uint16_t  attributo;        // enemy_attr::ATTR_*
    int16_t  x;                // posizione iniziale (coordinate mondo)
    int16_t  y;                // posizione iniziale (coordinate mondo)
    uint16_t delay;            // ticks2action iniziale, prima del primo cambio di stato
};

class enemy : public live_obj
{
public:
    bn::optional<bomb> bomba;   // creata al lancio, distrutta a esplosione finita
    void throw_bomb();
    void throw_shell();   // variante mortaio: arco alto, esplode al primo impatto
    void throw_arrow();
    void throw_gravity_arrow();
    void throw_fireball();

    void melee();

    bn::optional<bn::sprite_animate_action<10>> actionMelee;

    uint8_t tipo = 0;
    uint16_t attributo = 0;

    bn::fixed sight_range = 110;
    bn::fixed melee_range = 24;
    bn::fixed chase_range = 64;
    uint16_t  jump_period = 90; // frame tra due salti (STATIC_JUMP)
    uint16_t  attack_period = 120;// frame tra due attacchi

    // --- stato ---
    uint8_t   currentAction;      // ACTION_STAND/MOVE/ATTACK/STUN
    uint16_t  attack_cd = 0;
    uint16_t  jump_cd = 0;

    explicit enemy(const enemy_def& def);

    virtual void update();
    virtual ~enemy() = default;  // distruttore virtual obbligatorio

    void beHitByBark(int _dir, bn::fixed power = 0);
    void beHitByDog();
    void beHitByShockwave(int _dir);   // _dir: direzione in cui il nemico viene spinto (opposta al cane)
    void beHitByCassa(int damage);

    void takeDamage(int damage);

    int contact_damage = 1;     // danno per semplice contatto
    int melee_damage = 0;     // 0 = non è un nemico melee
    int meleeTicks;
    bool dog_in_melee_range() const;

    int  vita_residua_ticks = -1;   // -1 = nessun limite; >=0 = nemico evocato, sparisce a 0
    bool distrutto = false;         // true quando la magia si esaurisce: va rimosso da g_enemies
    const enemy_def* def_origine = nullptr;   // != nullptr: nemico di tabella, rinasce dopo l'eliminazione


    uint8_t deathTicks = 0;

    bool is_dead() const { return currentAction == ACTION_DEAD; }
    void die(int _dir);
    void update_death();

    bool has(int16_t a) const { return (attributo & a) != 0; }
    bool blocks_bark(int from_dir) const;     // hook per bau::update()
private:
    void init(const enemy_def& def);
    void update_fantasma();
    void update_pattugliatore();

    void update_physics();
    void update_movement();
    void update_attack();
    void update_projectile();
    void update_graphics();

    // movimenti
    void move_static();
    void move_static_jump();
    void move_patrol();
    void move_chase();
    void move_chase_shy();
    void move_melee();
    void move_ghost();
    void move_ghost_shy();
    // helper
    bool  dog_in_sight() const;
    bool  dog_is_looking_at_me() const;
    bn::fixed dist_to_dog() const;
    void  face_dog();
    void  walk_towards(int d, bn::fixed accel);
    void  fire(int16_t proj_type);
    int16_t pick_projectile() const;
    void throw_aimed_projectile(uint8_t tipoProjec, uint8_t tileIndex,
                            bool ignoreGravity, bool ignoreWalls,
                            bn::fixed speed);
};

namespace enemy_config {
    constexpr int PROJECTILE_LIFETIME = 120;   // frame di vita di proiettili lanciati
    constexpr int ARROW_TILE_INDEX = 9;
    constexpr int GRAVITY_ARROW_TILE_INDEX = 24;
    constexpr int FIREBALL_TILE_INDEX = 23;
    constexpr bn::fixed ARROW_SPEED = bn::fixed(2.0);
    constexpr bn::fixed FIREBALL_SPEED = bn::fixed(1.5);
    // Offset verticale di spawn rispetto al nemico (centro del sprite).
    constexpr bn::fixed SPAWN_Y_OFFSET = bn::fixed(-4);
}
