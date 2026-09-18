#pragma once

#include "bn_fixed.h"
#include "bn_vector.h"
#include "bn_sprite_ptr.h"
#include "bn_sprite_text_generator.h"
#include "bn_string.h"

class game_timer
{
public:
    static constexpr int SECONDS        = 10;
    static constexpr int FRAMES_PER_SEC = 60;
    static constexpr int TOTAL_FRAMES   = SECONDS * FRAMES_PER_SEC;

    // Punteggio base per secondo rimasto
    static constexpr int POINTS_PER_SEC = 10;

    // --- Stato ---
    bool active     = false;
    bool expired    = false;
    int  frames_left = 0;
    int  score      = 0;

    // --- Testo ---
    bn::vector<bn::sprite_ptr, 8> text_sprites;

    // -------------------------------------------------------
    // Avvia il countdown (chiamato quando human lancia ball)
    // -------------------------------------------------------
    void start()
    {
        active      = true;
        expired     = false;
        frames_left = TOTAL_FRAMES;
    }

    // -------------------------------------------------------
    // Ferma il countdown senza far scadere
    // (chiamato quando dog restituisce la palla)
    // -------------------------------------------------------
    int stop_and_score()
    {
        if(!active) return 0;

        active = false;
        expired = false;

        // Punteggio = secondi rimasti * punti per secondo
        int seconds_left = frames_left / FRAMES_PER_SEC;
        int earned = seconds_left * POINTS_PER_SEC;
        score += earned;
        return earned; // restituisce i punti guadagnati in questo lancio
    }

    // -------------------------------------------------------
    // Secondi rimanenti (per il testo)
    // -------------------------------------------------------
    int seconds_left() const
    {
        return (frames_left + FRAMES_PER_SEC - 1) / FRAMES_PER_SEC;
    }

    // -------------------------------------------------------
    // Update — chiamato ogni frame
    // Restituisce true se il timer è appena scaduto
    // -------------------------------------------------------
    bool update(bn::sprite_text_generator& gen)
    {
        if(!active) return false;

        frames_left--;

        if(frames_left <= 0)
        {
            frames_left = 0;
            active      = false;
            expired     = true;
            text_sprites.clear();
            return true; // SCADUTO
        }

        // Aggiorna il testo ogni secondo (non ogni frame, per performance)
        if(frames_left % FRAMES_PER_SEC == 0)
            _update_text(gen);

        return false;
    }

    // -------------------------------------------------------
    // Reset completo tra una sessione e l'altra
    // -------------------------------------------------------
    void reset()
    {
        active       = false;
        expired      = false;
        frames_left  = 0;
        text_sprites.clear();
    }

private:
    void _update_text(bn::sprite_text_generator& gen)
    {
        text_sprites.clear();

        int secs = seconds_left();

        // Colore di avviso: testo lampeggia sotto i 10 secondi
        // (gestito nel chiamante tramite set_visible)

        bn::string<4> str = bn::to_string<4>(secs);

        // Posizione: angolo in alto a destra
        // x = 100 (quasi bordo destro), y = -70 (quasi bordo superiore)
        gen.set_right_alignment();
        gen.generate(112, -72, str, text_sprites);
        gen.set_left_alignment(); // ripristina default
    }
};