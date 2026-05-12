#pragma once

#include "bn_sprite_ptr.h"
#include "bn_sprite_text_generator.h"
#include "bn_vector.h"
#include "bn_string_view.h"

class dog;

// Testi visualizzabili (indice 0 = schermata attesa, 1-24 = complimenti, 25-48 = lancio)
extern const bn::string_view testi[];
extern const int NUM_TESTI;

// Inizializza il generatore di testo (chiamare una volta all'avvio)
void update_text(int status, bn::sprite_text_generator* gen);

// Mostra un messaggio contestuale con il numero di schema
void update_text(int status, dog* _dog, bn::sprite_text_generator* gen = nullptr);

// Mostra un messaggio semplice (no dog)
void update_text(int status);

// Da chiamare ogni frame: gestisce la dissolvenza del testo corrente
void update_text_tick();
