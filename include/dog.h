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

    bool porta_apribile() const { return chiavi_raccolte >= chiavi_richieste; }

    int stordito_ticks = 0;

    bn::optional<bn::sprite_ptr> polvere_sprite;
    bn::optional<bn::sprite_animate_action<4>> polvere_anim;
};
