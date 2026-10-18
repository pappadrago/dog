#pragma once

#include "bn_fixed.h"
#include "obj.h"
#include <cstdint>

#define TRAMPOLINO_RIMBALZO_VY   bn::fixed(-5.5)   // contro BIG_JUMP_VY = -3.8: ~100px di salto
#define TRAMPOLINO_RIMBALZO_VY_B bn::fixed(-7.0)   // tenendo premuto B al rilascio: ~160px
#define TRAMPOLINO_VELOCITA_SPINTA bn::fixed(0.2) // tetto di chr_vx di dog mentre spinge
#define TRAMPOLINO_RITARDO_TICKS 12                // frame tra atterraggio e rimbalzo (= ATTERRAGGIO_ANIM_TICKS)
#define TRAMPOLINO_AFFONDAMENTO 7   // px di cui dog scende a fine compressione

struct trampolino_def
{
    int16_t x, y;   // posizione iniziale del centro (px, mondo): poi cade fino a terra
};

class trampolino : public live_obj
{
public:
    explicit trampolino(const trampolino_def& d);

    // Da chiamare ogni frame, DOPO g_dog->update()
    void update();

private:
    int _ticks_carica = 0;   // > 0: dog è agganciato e sta aspettando il rimbalzo

    void avvia_carica(bn::fixed top);
    void aggiorna_carica(bn::fixed top);
    void rimbalza(bn::fixed top);
    void spingi(bn::fixed dx);
    void imposta_frame(int tile);
    void tieni_dog(bn::fixed top);
};