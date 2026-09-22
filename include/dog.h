#pragma once

#include "bn_optional.h"
#include "bn_sprite_ptr.h"
#include "bn_sprite_animate_actions.h"
#include "bn_sprite_item.h"
#include "bn_fixed.h"
#include "bn_vector.h"
#include "game_constants.h"
#include "obj.h"

class ball;
class human;
class bau;
class enemy;

class dog : public live_obj
{
public:

    int palline_riportate = 0;
    int score = 0;

    int chiavi_raccolte = 0;
    int chiavi_richieste = 0;

    dog(int skin_index);

    void update();
    void dog_damage(int amount);
    void add_score(int points) { score += points; }


    bn::fixed bonus_corsa = bn::fixed(0);
    bn::fixed bonus_salto = bn::fixed(0);
    bn::fixed bonus_bau = bn::fixed(0);
    int       bonus_resistenza = 0;

    void applica_powerup(uint8_t sotto_tipo);

    bool porta_apribile() const { return chiavi_raccolte >= chiavi_richieste; }
};
