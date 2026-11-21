#include "onda.h"
#include "globals.h"


onda::onda(const onda_def& def) : _def(def), _tick_iniziale(g_ostacoli_ticks)
{
    if (_def.direzione != PERCORSO_LINEARE_VERTICALE)
        _def.direzione = PERCORSO_LINEARE_ORIZZONTALE;
    if (_def.sprite_size != PERCORSO_SPRITE_32)
        _def.sprite_size = PERCORSO_SPRITE_16;

    const oggetto_percorso_lineare_def componente = {
        _def.centro_x,
        _def.centro_y,
        _def.escursione,
        _def.direzione,
        _def.durata_tratto,
        _def.pausa_tratto,
        _def.sprite_size,
        _def.danno,
    };

    if (_def.numero_oggetti > 0)
    {
        const uint32_t periodo_oggetto = (_def.durata_tratto + _def.pausa_tratto) * 2;
        const uint32_t sfasamento_ticks = periodo_oggetto / _def.numero_oggetti;
        for (int i = 0; i < _def.numero_oggetti; ++i)
            _oggetti.push_back(new oggetto_percorso_lineare(
                componente, uint32_t(i) * sfasamento_ticks));
    }

     aggiorna_centri();
}

onda::~onda()
{
    for (oggetto_percorso_lineare* oggetto : _oggetti)
        delete oggetto;
}

void onda::aggiorna_centri()
{
    for (int i = 0; i < _def.numero_oggetti; ++i)
    {
        const int offset_numeratore = (2 * i - (_def.numero_oggetti - 1)) * _def.distanza_oggetti;
        const bn::fixed offset_relativo = bn::fixed(offset_numeratore).division(bn::fixed(2));
        if (_def.direzione == PERCORSO_LINEARE_VERTICALE)
            _oggetti[i]->sposta_centro( offset_relativo, ZERO);
        else
            _oggetti[i]->sposta_centro(ZERO,  offset_relativo);
    }
}

void onda::update()
{
     aggiorna_centri();
    for (int i = 0; i < _def.numero_oggetti; ++i)
        _oggetti[i]->update();
}

int onda::componenti_count() const
{
    return _oggetti.size();
}

const oggetto_percorso_lineare& onda::componente(int index) const
{
    return *_oggetti[index];
}
