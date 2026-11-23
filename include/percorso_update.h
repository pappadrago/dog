#pragma once

#include "dog.h"
#include "globals.h"

template<typename Punto>
inline void aggiorna_percorso(obj& percorso, int danno, Punto punto)
{
    if (bn::abs(g_dog->chr_x - percorso.chr_x) > SCREEN_W ||
        bn::abs(g_dog->chr_y - percorso.chr_y) > SCREEN_H)
        return;

    punto();
    percorso.sprite->set_x(percorso.chr_x - HALF_SCREEN_W);
    percorso.sprite->set_y(percorso.chr_y - HALF_SCREEN_H);

    if (g_dog->invulnerability > 0)
        return;

    if (bn::abs(g_dog->chr_x - percorso.chr_x) < g_dog->box_halfdim + percorso.box_halfdim &&
        bn::abs(g_dog->chr_y - percorso.chr_y) < g_dog->box_dim + percorso.box_dim)
    {
        g_dog->dog_damage_and_jump(danno, g_dog->chr_x < percorso.chr_x);
    }
}
