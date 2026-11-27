#pragma once
#include "bn_optional.h"
#include "bn_sprite_ptr.h"
#include "bn_sprite_animate_actions.h"
#include "obj.h"
#include "dog.h"
#include "globals.h"
#include "enemy.h"   // per enemy_attr::ATTR_*/ACTION_*, condivisi con enemy

enum class boss_tipo : uint8_t
{
    BOMBER,
    BALLER,
    FLYER,
    SUMMONER,
    SKELETON,
    COWARD,
    CHARGER,
    STREGONE,
    SLAMMER,
    TELEPORTER,   // nuovo
    GUARDIANO,    // nuovo
    SPIRALE       // nuovo
};


class boss : public live_obj
{
public:
    int16_t colpi_rimasti;
    int16_t colpi_totali;
    bn::fixed dimensione;   // lato dello sprite (o del bounding box)

    uint16_t attributo = enemy_attr::ATTR_NONE;   // era uint8_t: vedi nota in fondo
    uint8_t currentAction = ACTION_MOVE;

    bn::optional<bn::sprite_animate_action<10>> actionMelee;

    // usati anche da dog.cpp / collision.iwram.cpp: nomi e visibilità invariati
    int16_t meleeTicks = 0;
    int16_t ticks_attacco_melee = 0;
    int16_t melee_damage = 0;       // 0 = niente attacco melee configurato
    int16_t contact_damage = 2;

    int16_t ticks_prossimo_attacco = 90;   // condiviso da quasi tutti i pattern
    int16_t deathTicks = 0;
    bool distrutto = false;

    boss(bn::fixed x, bn::fixed y, int num_colpi, uint8_t attributo = enemy_attr::ATTR_NONE, bn::fixed dimensione = 32);
    boss(bn::fixed x, bn::fixed y, int num_colpi, boss_tipo tipo, bn::fixed dimensione = 32);
    void update();

    bool subisci_colpo(int danno = 1);
    bool sconfitto() const { return colpi_rimasti <= 0; }
    bool dog_in_melee_range() const;
    bool dog_in_bash_range() const;

    bn::optional<bn::sprite_animate_action<12>> actionSummon;

    void avvia_summon();
    void interrompi_summon();
    void completa_summon();

private:
    boss_tipo tipo;

    // Stato specifico del pattern: un boss usa UN solo blocco, quindi condividono la memoria.
    // Le struct anonime sono un'estensione GNU (con -Wpedantic dà un warning, non un errore).
    // Ogni blocco va inizializzato per intero in inizializza_stato().
    __extension__ union
    {
        struct   // BOMBER
        {
            int16_t bomber_ticks_to_jump;
            int16_t bomber_ticks_to_throw;
            int16_t bomber_shots_in_burst;
            int16_t bomber_throw_animation_ticks;
        };
        struct   // FLYER (il più grande: dimensiona la union)
        {
            int16_t flyer_ticks_to_bomb;
            int16_t flyer_ticks_to_fireball;
            int16_t flyer_move_delay;
            int16_t flyer_move_ticks;
            int16_t flyer_start_x, flyer_start_y;     // pixel interi
            int16_t flyer_target_x, flyer_target_y;
            bool flyer_fireball_pending;
            bool flyer_moving;
        };
        struct   // COWARD
        {
            int16_t runner_ticks_to_throw;
            int16_t runner_evade_ticks;
        };
        struct   // SUMMONER / SKELETON
        {
            int16_t summon_ticks;
            int16_t ticks_prossimo_summon;
            int16_t ticks_prossimo_saltello;
            int16_t ticks2dir;
            bool in_summon;
        };
        struct   // BALLER e tutti i boss a stati/fasi
        {
            int16_t stato;
            int16_t stato_ticks;
            int16_t contatore;        // carica doppia, eco, doppio salto
            int16_t angolo_raggi;     // SPIRALE
            int16_t patrol_origin_x;  // pixel interi
            int16_t salto_vx_data;    // bn::fixed::data(): vedi salto_vx()
            uint8_t fase_boss;
        };
    };

    bn::fixed salto_vx() const { return bn::fixed::from_data(salto_vx_data); }
    void set_salto_vx(bn::fixed v) { salto_vx_data = int16_t(v.data()); }   // |v| < 8

    void inizializza_stato(bn::fixed x, bn::fixed y);

    void update_bomber();
    bool throw_bomber_bomb();
    void update_baller();
    bool throw_baller_fireball();
    void update_flyer();
    void update_summoner();
    bool throw_flyer_bomb();
    bool throw_flyer_fireball();
    void update_runner();
    bool throw_runner_bomb();

    void update_charger();
    void update_stregone();
    void update_slammer();
    void update_teleporter();
    void update_guardiano();
    void update_spirale();
    void spawn_fulmini(int quanti);
    void spara_anello(int n);
    void aggiorna_grafica(int anim, int x_offset = 0);
    void contatto_cane(int danno);

    // solo se hai applicato la versione a tre fasi:
    int calcola_fase() const;
    bool entra_in_nuova_fase(int& fase);
    void onde_radenti(bn::fixed velocita = bn::fixed(2));
};