#pragma once
#include "bn_optional.h"
#include "bn_sprite_ptr.h"
#include <cstdint>

#define SPIKE_MAX_EXT      16   // estensione massima in px
#define SPIKE_EXT_AVVISO    4   // quanto spunta durante l'avviso
#define SPIKE_EXT_PERICOLO  6   // sotto questa estensione non fa danno

struct spike_def
{
    int16_t  tile_x, tile_y;   // tile (16px) del PAVIMENTO su cui poggia lo spuntone
    uint16_t ticks_attesa;     // nascosto
    uint16_t ticks_avviso;     // vibra
    uint16_t ticks_scatto;     // salita veloce
    uint16_t ticks_estesa;     // resta fuori
    uint16_t ticks_ritiro;     // discesa lenta
    uint16_t sfasamento;       // tick di anticipo sul ciclo: desincronizza più spikes
    int16_t  danno;
};

class spike
{
public:
    explicit spike(const spike_def& d);
    void update();

private:
    enum stato : uint8_t { ATTESA, AVVISO, SCATTO, ESTESA, RITIRO };

    spike_def def;
    stato _stato = ATTESA;
    int _ticks = 0;
    int _ext = 0;
    bn::optional<bn::sprite_ptr> sprite;

    void avanza();           // solo logica del ciclo
    void cambia(stato s) { _stato = s; _ticks = 0; }
    void aggiorna_sprite();
    void controlla_cane();
};