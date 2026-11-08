#pragma once

#include "bn_optional.h"
#include "bn_vector.h"
#include "bn_array.h"
#include "bn_sprite_ptr.h"
#include "bn_sprite_animate_actions.h"
#include "bn_fixed.h"
#include "obj.h"
#include <cstdint>

#define SNAKE_SEGMENTS       16   // testa, 6 segmenti del corpo, coda
#define SNAKE_SPACING_PX     9    // distanza lungo il percorso tra due segmenti (sprite 16x16 sovrapposti)

enum snake_percorso : uint8_t
{
    SNAKE_ELLISSE = 0,     // cerchio se semiasse_x == semiasse_y
    SNAKE_RETTANGOLO,      // giro dei 4 lati
    SNAKE_OTTO             // otto (Lissajous 1:2), si incrocia al centro
};

// Definizione statica, da mettere in <schema>_snakes.h
struct snake_def
{
    int16_t  centro_x, centro_y;
    int16_t  semiasse_x, semiasse_y;
    uint8_t  percorso;          // snake_percorso
    uint8_t  velocita_x10;      // px/frame x10 (12 = 1.2 px/frame), costante lungo tutto il percorso
    bool     oraria;
    int16_t  fase_iniziale;     // gradi 0..359: punto di partenza della testa sul percorso
    uint16_t fireball_period;   // frame tra due fireball
    int16_t  life;              // <= START_LIFE, altrimenti la healthbar non si riempie
    uint16_t delay;             // frame prima della prima fireball
};

class snake : public live_obj
{
public:
    // chr_x/chr_y = posizione della testa. life/invulnerability/dir vengono da live_obj.
    int  contact_damage = 5;
    bool distrutto = false;     // true a morte finita: va rimosso da g_snakes e fatto delete

    explicit snake(const snake_def& d);

    void update();

    bool is_dead() const { return _dying; }
    void be_hit_by_bark(bn::fixed power, int segment_index);

    // Indice del segmento colpibile sovrapposto, oppure -1 se nessuno.
    int overlapping_segment(bn::fixed x, bn::fixed y, bn::fixed reach) const;

private:
    snake_def def;

    bn::vector<bn::sprite_ptr, SNAKE_SEGMENTS> _segs;
    bn::array<bn::fixed, SNAKE_SEGMENTS> _x;
    bn::array<bn::fixed, SNAKE_SEGMENTS> _y;
    bn::array<int, SNAKE_SEGMENTS> _segment_life;
    bn::array<bn::optional<bn::sprite_ptr>, SNAKE_SEGMENTS> _death_sprites;
    bn::array<bn::optional<bn::sprite_animate_action<9>>, SNAKE_SEGMENTS> _death_animations;
    int _segment_count = SNAKE_SEGMENTS;
    bool _dying = false;

    bn::fixed _perimetro;
    bn::fixed _fase;     // fase della testa, gradi 0..360
    bn::fixed _step;     // sfasamento in gradi tra due segmenti consecutivi
    bn::fixed _delta;    // gradi per frame
    int       _sgn;      // +1 oraria, -1 antioraria
    int       _dir_coda = 1;

    int _anim = 0;
    int _fire_cd = 0;
    int _death_ticks = 0;

    void take_damage(int damage, int segment_index);
    void punto(bn::fixed fase, bn::fixed& x, bn::fixed& y) const;
    void calcola_posizioni();
    void aggiorna_sprite();
    bool spara();
    void check_dog();
    void update_death();
    void update_segment_death_animations();
};
