#pragma once
#include "bn_fixed.h"
#include "obj.h"
#include <cstdint>

#define SCALA_NORMALE   0   // la cima non regge: si esce in aria
#define SCALA_CON_PIANO 1   // la cima è un piano "cloud": ci si sta sopra e si scende premendo giù

#define SCALA_VELOCITA         bn::fixed(1.0)
#define SCALA_ACCELERAZIONE    bn::fixed(0.18)
#define SCALA_FRENATA         bn::fixed(0.22)
#define SCALA_MEZZA_LARGHEZZA 6   // tolleranza orizzontale per agganciarsi (px dal centro)

struct scala_def
{
    int16_t tile_x;             // colonna (tile 16px)
    int16_t tile_y1, tile_y2;   // riga più alta e più bassa (inclusive)
    uint8_t tipo;               // SCALA_*
};

class scala
{
public:
    scala_def def;

    explicit scala(const scala_def& d) : def(d) {}

    // Geometria in pixel (coordinate mondo)
    bn::fixed x_centro() const { return bn::fixed(def.tile_x * 16 + 8); }
    bn::fixed y_cima()   const { return bn::fixed(def.tile_y1 * 16); }
    bn::fixed y_fondo()  const { return bn::fixed((def.tile_y2 + 1) * 16); }

    // Da chiamare ogni frame per OGNI live_obj che puo_scalare (dog e nemici ATTR_CLIMB),
    // dopo update() e prima/insieme a verifica_nuovo_aggancio() delle piattaforme mobili.
    void aggiorna(live_obj& o);

private:
    bool in_colonna(const live_obj& o) const;
    bool prova_aggancio(live_obj& o);   // o.intent_scala != 0, nei pressi della scala
    void muovi(live_obj& o);            // o è agganciato: sale/scende, gestisce cima e fondo
    bool appoggia_in_cima(live_obj& o); // piano one-way per SCALA_CON_PIANO
    void rilascia(live_obj& o, int dir_uscita = 0);
};
