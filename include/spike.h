#pragma once
#include "bn_optional.h"
#include "bn_sprite_ptr.h"
#include <cstdint>

#define SPIKE_EXT_AVVISO    4   // quanto spunta durante l'avviso
#define SPIKE_EXT_PERICOLO  8   // sotto questa estensione non fa danno

#define SPIKE_ALTA_16 16
#define SPIKE_ALTA_32 32

struct spike_def
{
    int16_t  tile_x, tile_y;   // tile (16px) SOLIDA da cui esce lo spuntone (pavimento o soffitto)
    uint8_t  altezza;          // SPIKE_ALTA_16 o SPIKE_ALTA_32
    bool     verso_basso;      // false = esce dal pavimento verso l'alto, true = esce dal soffitto verso il basso
    uint16_t ticks_attesa;     // nascosto
    uint16_t ticks_avviso;     // vibra
    uint16_t ticks_scatto;     // salita veloce
    uint16_t ticks_estesa;     // resta fuori
    uint16_t ticks_ritiro;     // discesa lenta
    uint16_t sfasamento;       // tick di anticipo sul ciclo
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
    int _base = 0;     // y del bordo del solido da cui esce (mondo, px)
    int _segno = -1;   // -1 = verso l'alto, +1 = verso il basso
    bn::optional<bn::sprite_ptr> sprite;

    void avanza();
    void cambia(stato s) { _stato = s; _ticks = 0; }
    void aggiorna_sprite();
    void controlla_cane();
};