// src/cassa.cpp
#include "globals.h"
#include "dog.h"
#include "boss.h"
#include "collision.h"
#include "bn_sprite_items_items.h"
#include "cassa.h"

static constexpr bn::fixed VELOCITA_SPINTA = bn::fixed(1.2);
static constexpr int TICKS_PRIMA_DI_SPARIRE = 180;   // ~3 secondi a 60fps

cassa::cassa(uint8_t _tipo, bn::fixed x, bn::fixed y)
{
    tipo = _tipo;
    chr_x = x;
    chr_y = y;
    box_dim = bn::fixed(12);
    box_halfdim = bn::fixed(6);

    sprite = bn::sprite_items::items.create_sprite(x - HALF_SCREEN_W, y - HALF_SCREEN_H, tipo);
    sprite->set_camera(g_camera);
    sprite->set_bg_priority(1);
}

void cassa::update()
{
    if (distrutta) return;

    if (a_terra)
    {
        ticks_a_terra--;
        if (ticks_a_terra < 60)
            sprite->set_visible(ticks_a_terra % 2);   // lampeggia poco prima di sparire
        if (ticks_a_terra <= 0)
        {
            distrutta = true;
            sprite->set_visible(false);
        }
        return;
    }

    // spinta per contatto: mentre il cane la tocca, prende velocità nella direzione opposta a lui
    if (check_collision_16(*sprite, *g_dog->sprite))
        chr_vx = (g_dog->chr_x < chr_x) ? VELOCITA_SPINTA : -VELOCITA_SPINTA;

    chr_x += chr_vx;
    apply_friction();
    apply_map();
    apply_gravity();

    // colpisce il boss solo mentre sta cadendo attivamente, non se ferma/appoggiata
    if (!onGround && chr_vy > bn::fixed(1.0) && check_collision_boss(*this))
    {
        g_boss->subisci_colpo();   // ogni cassa = un colpo, indipendentemente dal tipo
        distrutta = true;
        sprite->set_visible(false);
        return;
    }

    if (onGround)
    {
        a_terra = true;
        ticks_a_terra = TICKS_PRIMA_DI_SPARIRE;
    }

    sprite->set_x(chr_x - HALF_SCREEN_W);
    sprite->set_y(chr_y - HALF_SCREEN_H);
}