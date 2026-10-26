#pragma once
#include "bn_sprite_text_generator.h"

// Congela il gioco e mostra l'animazione di power up.
// Bloccante: ritorna a fine animazione, con le palette ripristinate.
void powerup_freeze(bn::sprite_text_generator& text_generator);