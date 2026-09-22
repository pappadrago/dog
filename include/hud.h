#pragma once

#include "bn_sprite_text_generator.h"
#include "bn_string_view.h"

// ---------------------------------------------------------------------------
// Messaggi contestuali (centro/sinistra schermo, con dissolvenza)
// ---------------------------------------------------------------------------

// Testi visualizzabili (indice 0 = schermata attesa, 1-24 = complimenti, 25-48 = lancio)
extern const bn::string_view testi[];
extern const int NUM_TESTI;

// Inizializza il generatore di testo (chiamare una volta all'avvio).
// Imposta anche priorita' e z-order del generatore, cosi' tutto il testo
// (messaggi, HUD, timer) sta davanti a sfondi e sprite di gioco.
void update_text_init(bn::sprite_text_generator* gen);

// Mostra un messaggio contestuale con numero di schema (es. schermata attesa START)
void update_text_schema(int schema);

// Mostra un messaggio semplice per indice
void update_text(int status);

// Nasconde il testo corrente
void update_text_clear();

// Da chiamare ogni frame: gestisce la dissolvenza del testo corrente
void update_text_tick();

// ---------------------------------------------------------------------------
// HUD di gioco: energia (in alto a sinistra, stile Gunstar Heroes) e punteggio
// (in alto a destra). Il chiamante passa ogni frame i valori reali; l'HUD
// li rincorre con un contatore che scorre e rigenera gli sprite solo quando
// la cifra visualizzata cambia.
// ---------------------------------------------------------------------------

// Da chiamare ogni frame dopo la logica di gioco.
//   life  = energia residua (0..max), score = punteggio (0..999999)
// Alla prima chiamata i contatori partono direttamente dal valore passato.
// Quando l'energia cala il numero lampeggia; sotto HUD_LOW_LIFE lampeggia piano.
void hud_update(int life, int score, int keys);

// Cancella l'HUD; alla prossima hud_update riparte da zero (nuova partita).
void hud_reset();
