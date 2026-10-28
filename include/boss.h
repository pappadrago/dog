#pragma once
#include "bn_optional.h"
#include "bn_sprite_ptr.h"
#include "bn_sprite_animate_actions.h"
#include "obj.h"
#include "dog.h"
#include "globals.h"
#include "enemy.h"   // per enemy_attr::ATTR_*/ACTION_*, condivisi con enemy
#include "bomb.h"

enum class boss_tipo : uint8_t
{
    BOMBER,
    BALLER
};

class boss : public live_obj
{
public:
    int colpi_rimasti;
    int colpi_totali;
    bn::fixed dimensione;   // lato dello sprite (o del bounding box), non più una macro fissa

    uint8_t attributo = enemy_attr::ATTR_NONE;    // enemy_attr::ATTR_BASH, enemy_attr::ATTR_MELEE (bitmask)
    uint8_t currentAction = ACTION_MOVE;

    bn::optional<bn::sprite_ptr> weaponSprite;
    bn::optional<bn::sprite_animate_action<10>> actionMelee;

    bn::fixed wx_base, wy_base;
    bn::fixed wpn_vx, wpn_vy;
    int weaponTicks = 0;
    int meleeTicks = 0;
    int ticks_attacco_melee = 0;
    int ticks_prossimo_attacco = 90;
    int ticks_prossimo_saltello = 60;
    int ticks2dir = 0;

    int contact_damage = 2;
    int melee_damage = 0;   // 0 = niente attacco melee configurato

    boss(bn::fixed x, bn::fixed y, int num_colpi, uint8_t attributo = enemy_attr::ATTR_NONE, bn::fixed dimensione = 32);
    boss(bn::fixed x, bn::fixed y, int num_colpi, boss_tipo tipo, bn::fixed dimensione = 32);
    void update();

    bool subisci_colpo(int danno = 1);
    bool sconfitto() const { return colpi_rimasti <= 0; }
    bool dog_in_melee_range() const;
    bool dog_in_bash_range() const;

    bool in_summon = false;
    int  summon_ticks = 0;
    int  ticks_prossimo_summon = 150;
    bn::optional<bn::sprite_animate_action<12>> actionSummon;

    void avvia_summon();
    void interrompi_summon();
    void completa_summon();

private:
    bool bomber_mode = false;
    bool baller_mode = false;
    bn::fixed patrol_origin_x = ZERO;
    int bomber_ticks_to_jump = 180;
    int bomber_ticks_to_throw = 60;
    int bomber_shots_in_burst = 0;
    int bomber_throw_animation_ticks = 0;
    bn::optional<bomb> bomber_bombs[MAX_ENEMY_BOMBS];
    bn::optional<bomb> baller_fireball;

    void update_bomber();
    bool throw_bomber_bomb();
    void update_baller();
    bool throw_baller_fireball();
};