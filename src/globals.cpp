#include "globals.h"

#include "bau.h"
#include "enemy.h"
#include "item.h"
#include "dog.h"
#include "game_timer.h"


bn::random                             g_rng;
bn::optional<bn::camera_ptr>           g_camera;

bn::optional<bau>                      g_bau;
bn::optional<dog>                      g_dog;
bn::optional<bn::vector<enemy*, MAX_ENEMIES>> g_enemies;
bn::optional<bn::vector<item*, MAX_ENEMIES>> g_items;
bn::optional<game_timer> g_timer;

bn::fixed cap(bn::fixed val, bn::fixed max) {
    if (val > max) val = max;
    if (val < -max) val = -max;
    return val;
}