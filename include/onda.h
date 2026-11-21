#pragma once

#include "oggetto_percorso_lineare.h"

constexpr int MAX_ONDA_COMPONENTI = 8;

struct onda_def
{
    int16_t centro_x, centro_y;
    uint8_t numero_oggetti;  // massimo MAX_ONDA_COMPONENTI
    int16_t distanza_oggetti;
    uint8_t direzione;
    uint8_t sprite_size;
    int16_t escursione;
    uint8_t durata_tratto;
    uint8_t pausa_tratto;
    int16_t danno;
};

class onda
{
public:
    explicit onda(const onda_def& def);
    ~onda();
    void update();
    int componenti_count() const;
    const oggetto_percorso_lineare& componente(int index) const;

private:
    onda_def _def;
    bn::vector<oggetto_percorso_lineare*, MAX_ONDA_COMPONENTI> _oggetti;
    uint32_t _tick_iniziale;
    uint32_t _durata_tratto;

    void aggiorna_centri();
};
