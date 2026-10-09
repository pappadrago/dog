#pragma once

#include "bn_random.h"
#include "bn_camera_ptr.h"
#include "bn_optional.h"
#include "bn_vector.h"
#include "game_constants.h"
#include "math.h"
#include "bau.h"
#include "cassa.h"
#include "boss.h"
#include "dog.h"

// Forward declaration di tutte le classi di gioco:
// i .cpp che ne hanno bisogno includeranno i rispettivi header.
class bau;
class item;
class enemy;
class dog;
class game_timer;
class cassa;
class boss;

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
extern bn::optional<boss> g_boss;
extern bn::optional<bn::vector<cassa*, MAX_CASSE>> g_casse;


class piattaforma;
#define MAX_PIATTAFORME 16
extern bn::optional<bn::vector<piattaforma*, MAX_PIATTAFORME>> g_piattaforme;

class lava_zona;
#define MAX_LAVA 8
extern bn::optional<bn::vector<lava_zona*, MAX_LAVA>> g_lava;

class piattaforma_mobile;
#define MAX_PIATTAFORME_MOBILI 8
extern bn::optional<bn::vector<piattaforma_mobile*, MAX_PIATTAFORME_MOBILI>> g_piattaforme_mobili;

bn::fixed cap(bn::fixed val, bn::fixed max);
bn::fixed capzero(bn::fixed val, bn::fixed max);
int capzero(int val, int max);

#define MAP_HALF_W 768
#define MAP_HALF_H 256


extern int g_schema;


extern int g_shake_ticks;
extern int g_shake_durata;
extern int g_shake_ampiezza;
void screen_shake(int ticks, int ampiezza);