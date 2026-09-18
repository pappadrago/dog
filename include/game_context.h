#pragma once

#include "bn_random.h"
#include "bn_camera_ptr.h"
#include "bn_regular_bg_ptr.h"
#include "bn_sprite_text_generator.h"
#include "bn_vector.h"

#include "game_constants.h"
#include "ball.h"
#include "bau.h"
#include "enemy.h"
#include "human.h"
#include "dog.h"
#include "helper_dog.h"

// Raccoglie tutte le istanze degli oggetti di gioco.
// Viene costruito in main() e passato ai costruttori delle classi che ne hanno bisogno.
struct GameContext
{
    // --- Infrastruttura ---
    bn::random                 rng;
    bn::camera_ptr             camera;
    bn::sprite_text_generator& text_generator;  // owned by main, passato per ref

    // --- Sfondi (owned by main, passati per ref) ---
    bn::regular_bg_ptr& bg0;
    bn::regular_bg_ptr& bg1;
    bn::regular_bg_ptr& bg3;

    // --- Oggetti di gioco ---
    bau                           the_bau;
    ball                          the_ball;
    human                         the_human;
    bn::vector<enemy, MAX_ENEMIES> enemies;
    dog                           the_dog;
    helper_dog                    the_helper;

    // --- Skin scelta nella schermata di selezione ---
    int skin_index;

    // Costruttore: richiede i riferimenti agli oggetti bn già creati in main()
    // e la skin selezionata; inizializza tutti gli oggetti di gioco nell'ordine corretto.
    GameContext(bn::sprite_text_generator& text_gen,
                bn::regular_bg_ptr&        background0,
                bn::regular_bg_ptr&        background1,
                bn::regular_bg_ptr&        background3,
                int                        selected_skin);
};
