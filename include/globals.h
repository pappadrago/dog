#pragma once

#include "bn_random.h"
#include "bn_camera_ptr.h"
#include "bn_optional.h"
#include "bn_vector.h"
#include "game_constants.h"
#include "math.h"

// Forward declaration di tutte le classi di gioco:
// i .cpp che ne hanno bisogno includeranno i rispettivi header.
class bau;
class item;
class enemy;
class dog;
class game_timer;

// --- Globali ---
// Definiti in globals.cpp, accessibili ovunque includendo questo file.

extern bn::random              g_rng;
extern bn::optional<bn::camera_ptr> g_camera;

extern bn::optional<bau>       g_bau;
extern bn::optional<dog>       g_dog;
extern bn::optional<game_timer> g_timer;

// Il vector di enemy è allocato come optional per evitare
// il costruttore globale con template non banale.
extern bn::optional<bn::vector<enemy*, MAX_ENEMIES>> g_enemies;
extern bn::optional<bn::vector<item*, MAX_ENEMIES>> g_items;

bn::fixed cap(bn::fixed val, bn::fixed max);
bn::fixed capzero(bn::fixed val, bn::fixed max);
int capzero(int val, int max);

#define MAP_HALF_W 768
#define MAP_HALF_H 256


extern int g_schema;

