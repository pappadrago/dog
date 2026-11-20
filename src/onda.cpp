#include "onda.h"
#include "globals.h"

namespace
{
    constexpr uint32_t PAUSA_ESTREMO = 30;
}

onda::onda(const onda_def& def) : _def(def), _tick_iniziale(g_ostacoli_ticks)
{
    if (_def.direzione != PERCORSO_LINEARE_VERTICALE)
        _def.direzione = PERCORSO_LINEARE_ORIZZONTALE;
    if (_def.sprite_size != PERCORSO_SPRITE_32)
        _def.sprite_size = PERCORSO_SPRITE_16;
    if (_def.escursione < 1)
        _def.escursione = 1;
    if (_def.velocita_oggetti_x10 < 1)
        _def.velocita_oggetti_x10 = 1;
    if (_def.corsa_centro < 1)
        _def.corsa_centro = 1;
    if (_def.velocita_centro_x10 < 1)
        _def.velocita_centro_x10 = 1;

    const uint32_t durata_numeratore = uint32_t(_def.corsa_centro) * 30;
    _durata_tratto = (durata_numeratore + _def.velocita_centro_x10 - 1) /
                     _def.velocita_centro_x10;
    if (_durata_tratto < 1)
        _durata_tratto = 1;

    const oggetto_percorso_lineare_def componente = {
        _def.centro_x,
        _def.centro_y,
        _def.escursione,
        _def.direzione,
        _def.velocita_oggetti_x10,
        _def.sprite_size,
        _def.danno,
    };
    for (int i = 0; i < _def.numero_oggetti; ++i)
        _oggetti.push_back(new oggetto_percorso_lineare(componente));

    aggiorna_centri(spostamento_centro());
}

onda::~onda()
{
    for (oggetto_percorso_lineare* oggetto : _oggetti)
        delete oggetto;
}

void onda::aggiorna_centri(bn::fixed spostamento)
{
    for (int i = 0; i < _def.numero_oggetti; ++i)
    {
        const int offset_numeratore = (2 * i - (_def.numero_oggetti - 1)) * _def.distanza_oggetti;
        const bn::fixed offset_relativo = bn::fixed(offset_numeratore).division(bn::fixed(2));
        if (_def.direzione == PERCORSO_LINEARE_VERTICALE)
            _oggetti[i]->sposta_centro(spostamento + offset_relativo, ZERO);
        else
            _oggetti[i]->sposta_centro(ZERO, spostamento + offset_relativo);
    }
}

bn::fixed onda::spostamento_centro() const
{
    const uint32_t durata_ciclo = (_durata_tratto + PAUSA_ESTREMO) * 2;
    const uint32_t tempo = (g_ostacoli_ticks - _tick_iniziale) % durata_ciclo;
    const uint32_t durata_fase = _durata_tratto + PAUSA_ESTREMO;
    const bool ritorno = tempo >= durata_fase;
    const uint32_t tempo_tratto = tempo % durata_fase;
    bn::fixed t = bn::fixed::from_data(int(
        (uint64_t(tempo_tratto) * bn::fixed::scale()) / _durata_tratto));
    if (t > 1)
        t = 1;

    const bn::fixed progressione = t * t * (bn::fixed(3) - bn::fixed(2) * t);
    const bn::fixed posizione = ritorno ? bn::fixed(1) - progressione : progressione;
    return (posizione * bn::fixed(2) - bn::fixed(1)) * bn::fixed(_def.corsa_centro);
}

void onda::update()
{
    aggiorna_centri(spostamento_centro());
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
