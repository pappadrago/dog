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

#define WEAPON_FLICK_TIME 25

#define TIPO_NEMICO_GENERICO 0
#define TIPO_NEMICO_DRUIDO_DINAMICO 1
#define TIPO_NEMICO_DRUIDO_STATICO 2
#define TIPO_NEMICO_ARCIERE_STATICO 3
#define TIPO_NEMICO_BLOB_PATTUGLIATORE 4
#define TIPO_NEMICO_SPADACCINO_PATTUGLIATORE 5
#define TIPO_NEMICO_BOMBAROLO 6
#define TIPO_NEMICO_MORTAIO 10
#define TIPO_NEMICO_PATTUGLIATORE 11

#define TIPO_NEMICO_PUGILE 12

#define ATTRIBUTO_NO 0
#define ATTRIBUTO_AIM (1<<0)
#define ATTRIBUTO_MELEE (1<<1)
#define ATTRIBUTO_BASH (1<<2)

#define ACTION_STAND 0
#define ACTION_MOVE 1
#define ACTION_ATTACK 2
#define ACTION_STUN 3

#define MELEE_NONE     0
#define MELEE_WINDUP   1
#define MELEE_ACTIVE   2
#define MELEE_RECOVERY 3

// Definizione statica di un nemico posizionato in uno schema.
// Le tabelle per schema stanno in <schema>_enemies.h (es. s1_enemies.h)
struct enemy_def
{
    uint8_t  tipo;             // TIPO_NEMICO_*
    uint8_t  attributo;        // ATTRIBUTO_*
    int16_t  x;                // posizione iniziale (coordinate mondo)
    int8_t   dir;              // DIR_LEFT / DIR_RIGHT iniziale
    uint16_t delay;            // ticks2action iniziale, prima del primo cambio di stato
    uint8_t  variante = 0;     // usato solo da TIPO_NEMICO_GENERICO: skin (0-9) e tipo di arma
};

class enemy : public live_obj
{
public:
    bn::optional<bomb> bomba;   // creata al lancio, distrutta a esplosione finita
    void throw_bomb();
    void throw_shell();   // variante mortaio: arco alto, esplode al primo impatto

    bn::optional<bn::sprite_ptr>               weaponSprite;
    bn::optional<bn::sprite_animate_action<10>> actionMelee;

    bn::fixed wx_base = 0, wy_base = 0;
    bn::fixed wpn_vx = 0, wpn_vy = 0;
    bn::fixed weapon_time = bn::fixed(120.0);
    int weaponDir = DIR_RIGHT;
    uint8_t tipo = 0;
    uint8_t attributo = 0;
    uint8_t currentAction = ACTION_STAND;
    uint16_t weaponTicks = 0;

    explicit enemy(const enemy_def& def);

    virtual void update();
    virtual ~enemy() = default;  // distruttore virtual obbligatorio

    void beHitByBark();
    void beHitByDog();

    int     contact_damage = 1;     // danno per semplice contatto
    int     melee_damage = 0;     // 0 = non è un nemico melee
    int meleeTicks;
    bool dog_in_melee_range() const;

private:
    void init(const enemy_def& def);
    void update_fantasma();
    void update_pattugliatore();
};
