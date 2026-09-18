#pragma once

#include "bn_sprite_text_generator.h"
#include "bn_string_view.h"

// Testi visualizzabili (indice 0 = schermata attesa, 1-24 = complimenti, 25-48 = lancio)
extern const bn::string_view testi[];
extern const int NUM_TESTI;

// Inizializza il generatore di testo (chiamare una volta all'avvio)
void update_text_init(bn::sprite_text_generator* gen);

// Mostra un messaggio contestuale con numero di schema (es. schermata attesa START)
void update_text_schema(int schema);

// Mostra un messaggio semplice per indice
void update_text(int status);

// Nasconde il testo corrente
void update_text_clear();

// Da chiamare ogni frame: gestisce la dissolvenza del testo corrente
void update_text_tick();
