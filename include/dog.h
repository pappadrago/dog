#pragma once

#include "bn_optional.h"
#include "bn_sprite_ptr.h"
#include "bn_sprite_animate_actions.h"
#include "bn_sprite_item.h"
#include "bn_fixed.h"
#include "bn_vector.h"
#include "game_constants.h"
#include "obj.h"

#define VELOCITA_VY_CADUTA_DURA bn::fixed(4.0)   // velocità d'impatto oltre la quale scatta lo stordimento
#define STORDIMENTO_CADUTA_TICKS 30
#define DASH_TICKS 15
#define DASH_SPEED bn::fixed(4.5)
class ball;
class human;
class bau;
class enemy;
#define GROUND_POUND_SHAKE_TICKS 20
#define GROUND_POUND_SHAKE_AMP 3
class dog : public live_obj
{
public:

    int score = 0;

    // Contatori dello schema corrente (usati solo dall'HUD)
    int chiavi_raccolte = 0;
    int chiavi_richieste = 0;

    // Bitmap globale: bit N = chiave N raccolta = porta N aperta (256 bit)
    uint8_t chiavi_bitmap[32] = {};

    bool ha_chiave(int id) const
    {
        id &= 0xFF;
        return (chiavi_bitmap[id >> 3] >> (id & 7)) & 1;
    }
    void segna_chiave(int id)
    {
        id &= 0xFF;
        chiavi_bitmap[id >> 3] |= uint8_t(1 << (id & 7));
    }
    bool porta_apribile(int id) const { return ha_chiave(id); }


    dog();

    void update();
    void dog_damage(int amount);
    void add_score(int points) { score += points; }


    bn::fixed bonus_corsa = ZERO;
    bn::fixed bonus_salto = ZERO;
    bn::fixed bonus_bau = ZERO;
    int       bonus_resistenza = 0;
    int charge_ticks = 0;
    void applica_powerup(uint16_t attributo);
    void update_animations();

    u_int8_t stordito_ticks = 0;
    u_int16_t idle_ticks = 0;
    u_int8_t dash_ticks = 0;

        // Tetto temporaneo a chr_vx, impostato da oggetti che dog sta spingendo (es. trampolino)
    int       spinta_ticks = 0;
    bn::fixed tetto_vx_spinta = ZERO;

    bn::optional<bn::sprite_ptr> polvere_sprite;
    bn::optional<bn::sprite_animate_action<4>> polvere_anim;
    bn::optional<bn::sprite_animate_action<4>> ground_pound_anim;

    // meno negativa del salto normale (-4.0): più orizzontale che verticale
#define WALL_JUMP_ANIM_TICKS 14

    bn::optional<bn::sprite_animate_action<4>> actionWallJump;
    bn::optional<bn::sprite_animate_action<14>> actionIdle;
    u_int8_t wall_jump_anim_ticks = 0;

    bool doppio_salto_acquisito = false;
    bool wall_jump_acquisito = false;
    bool dash_acquisito = false;
    bool air_dash_acquisito = false;
    bool ground_pound_acquisito = false;
    bool doppio_salto_disponibile = false;   // si consuma a ogni uso, si ricarica all'atterraggio
    bool ground_pound_engaged = false;
    bool wall_sliding = false;

#define COLPITO_ANIM_TICKS 18        // 6 frame, placeholder: regola in base alla velocità scelta per actionColpito
#define ATTERRAGGIO_ANIM_TICKS 12   // stessa durata dello stordimento: finiscono insieme
#define STACCO_ANIM_TICKS 12

    bn::optional<bn::sprite_animate_action<6>> actionColpito;
    bn::optional<bn::sprite_animate_action<4>> actionAtterraggio;   // placeholder: aggiorna il numero quando sai i frame reali
    bn::optional<bn::sprite_animate_action<4>> actionStacco;        // idem

    u_int8_t colpito_anim_ticks = 0;
    u_int8_t atterraggio_anim_ticks = 0;
    u_int8_t stacco_anim_ticks = 0;



    // Getter e Setter per Power Land
    [[nodiscard]] bool is_power_landing() const noexcept { return ground_pound_engaged; }
    void set_power_landing(bool value) noexcept { ground_pound_engaged = value; }




#define COYOTE_TICKS 6                       // ~100 ms
#define JUMP_BUFFER_TICKS 6
#define SALTO_TAGLIO_VY bn::fixed(-2.0)      // velocità a cui viene tagliato il salto al rilascio (~13px di salto minimo)

    int  coyote_ticks = 0;
    int  jump_buffer_ticks = 0;
    bool salto_tenuto = false;
    bool salto_pieno = false;   // true se il salto non è stato accorciato rilasciando il tasto in salita
    void start_polvere_anim(bn::fixed x, bn::fixed y, bool groundPound, bool vertical_flip, int rotation_angle);

#define GROUND_POUND_ONDA_RAGGIO_X bn::fixed(240)
#define GROUND_POUND_ONDA_RAGGIO_Y bn::fixed(160)
#define ONDA_URTO_STUN_TICKS 30

bn::optional<bn::sprite_ptr> spriteAusiliario;
bn::optional<bn::sprite_animate_action<24>> carica_anim;
bool carica_attiva = false;

u_int8_t text_ticks = 0;
bn::vector<bn::sprite_ptr, 4> numero_sprites;
void show_nr(bn::string<4> txt);
};
