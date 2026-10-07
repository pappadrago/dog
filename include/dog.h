#pragma once

#include "bn_optional.h"
#include "bn_sprite_ptr.h"
#include "bn_sprite_animate_actions.h"
#include "bn_sprite_item.h"
#include "bn_fixed.h"
#include "bn_vector.h"
#include "game_constants.h"
#include "obj.h"

#define CADUTA_DURA_SOGLIA bn::fixed(4.0)   // velocità d'impatto oltre la quale scatta lo stordimento
#define STORDIMENTO_CADUTA_TICKS 30

class ball;
class human;
class bau;
class enemy;

class dog : public live_obj
{
public:

    int score = 0;

    int chiavi_raccolte = 0;
    int chiavi_richieste = 0;

    dog();

    void update();
    void dog_damage(int amount);
    void add_score(int points) { score += points; }


    bn::fixed bonus_corsa = bn::fixed(0);
    bn::fixed bonus_salto = bn::fixed(0);
    bn::fixed bonus_bau = bn::fixed(0);
    int       bonus_resistenza = 0;

    void applica_powerup(uint8_t sotto_tipo);
    void update_animations();

    bool porta_apribile() const { return chiavi_raccolte >= chiavi_richieste; }

    int stordito_ticks = 0;
    int idle_ticks = 0;

    bn::optional<bn::sprite_ptr> polvere_sprite;
    bn::optional<bn::sprite_animate_action<4>> polvere_anim;
    bn::optional<bn::sprite_animate_action<4>> gorund_pound_anim;

#define WALL_JUMP_VX bn::fixed(2.6)
#define WALL_JUMP_VY bn::fixed(-2.6)   // meno negativa del salto normale (-4.0): più orizzontale che verticale
#define WALL_JUMP_ANIM_TICKS 14

    bn::optional<bn::sprite_animate_action<4>> actionWallJump;   // placeholder: aggiorna numero/indici frame quando pronti
    bn::optional<bn::sprite_animate_action<14>> actionIdle;   // placeholder: aggiorna numero/indici frame quando pronti
    int wall_jump_anim_ticks = 0;

    bool doppio_salto_acquisito = false;
    bool wall_jump_acquisito = false;
    bool doppio_salto_disponibile = false;   // si consuma a ogni uso, si ricarica all'atterraggio


#define COLPITO_ANIM_TICKS 18        // 6 frame, placeholder: regola in base alla velocità scelta per actionColpito
#define ATTERRAGGIO_ANIM_TICKS 12   // stessa durata dello stordimento: finiscono insieme
#define STACCO_ANIM_TICKS 12

    bn::optional<bn::sprite_animate_action<6>> actionColpito;
    bn::optional<bn::sprite_animate_action<4>> actionAtterraggio;   // placeholder: aggiorna il numero quando sai i frame reali
    bn::optional<bn::sprite_animate_action<4>> actionStacco;        // idem

    int colpito_anim_ticks = 0;
    int atterraggio_anim_ticks = 0;
    int stacco_anim_ticks = 0;

    bool grond_pound_engaged = false;


    // Getter e Setter per Power Land
    [[nodiscard]] bool is_power_landing() const noexcept { return grond_pound_engaged; }
    void set_power_landing(bool value) noexcept { grond_pound_engaged = value; }
};
