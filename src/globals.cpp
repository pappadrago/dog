#include "globals.h"

#include "bau.h"
#include "enemy.h"
#include "item.h"
#include "dog.h"
#include "boss.h"
#include "piattaforma.h"
#include "game_timer.h"


bn::random                             g_rng;
bn::optional<bn::camera_ptr>           g_camera;

bn::optional<bau>                      g_bau;
bn::optional<dog>                      g_dog;
bn::optional<boss>                      g_boss;
bn::optional<bn::vector<enemy*, MAX_ENEMIES>> g_enemies;
bn::optional<bn::vector<item*, MAX_ENEMIES>> g_items;
bn::optional<game_timer> g_timer;
bn::optional<bn::vector<piattaforma*, MAX_PIATTAFORME>> g_piattaforme;
bn::optional<bn::vector<lava_zona*, MAX_LAVA>> g_lava;
bn::optional<bn::vector<piattaforma_mobile*, MAX_PIATTAFORME_MOBILI>> g_piattaforme_mobili;

int g_schema = 1;

bn::fixed cap(bn::fixed val, bn::fixed max) {
    if (val > max) val = max;
    if (val < -max) val = -max;
    return val;
}
bn::fixed capzero(bn::fixed val, bn::fixed max) {
    if (val > max) val = max;
    if (val < 0) val = bn::fixed(0);
    return val;
}
int capzero(int val, int max) {
    if (val > max) val = max;
    if (val < 0) val = 0;
    return val;
}