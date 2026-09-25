#include "globals.h"
#include "item.h"
#include "bau.h"
#include "dog.h"
#include "bn_sprite_items_items.h"
#include "bn_math.h"
#include "bn_log.h"

// Indici tile per tipo di oggetto: adatta agli indici reali dello spritesheet "items"
static constexpr int TILE_CHIAVE = 31;
static constexpr int TILE_ENERGIA = 11;
static constexpr int TILE_POWERUP_BASE = 0; // + sotto_tipo (4 varianti consecutive)

static int tile_per_item(uint8_t tipo, uint8_t sotto_tipo)
{
    switch (tipo)
    {
    case ITEM_TIPO_CHIAVE:  return TILE_CHIAVE;
    case ITEM_TIPO_ENERGIA: return TILE_ENERGIA;
    case ITEM_TIPO_POWERUP: return TILE_POWERUP_BASE + sotto_tipo;
    default:                return 0;
    }
}

item::item(const item_def& def)
{
    tipo = def.tipo;
    sotto_tipo = def.sotto_tipo;
    spawn_x = bn::fixed(def.x);
    spawn_y = bn::fixed(def.y);

    chr_x = spawn_x;
    chr_y = spawn_y;

    sprite = bn::sprite_items::items.create_sprite(chr_x, chr_y, tile_per_item(tipo, sotto_tipo));
    sprite->set_bg_priority(1);
    sprite->set_camera(g_camera);

    box_dim = bn::fixed(8);
    box_halfdim = bn::fixed(4);

    ticks2action = -g_rng.get_int(100);
}

void item::update()
{
    if (raccolto && ticks2action == 0)
    {
        sprite->set_visible(false);
        return;
    }
    apply_gravity();

    ticks2action--;

    bn::fixed dy = bn::degrees_lut_sin_safe(ticks2action << 3) * 4 - 2;
    bn::fixed dx = bn::degrees_lut_cos_safe(ticks2action << 2) * 6 - 2;
    sprite->set_x(chr_x + dx - HALF_SCREEN_W);
    sprite->set_y(chr_y + dy - HALF_SCREEN_H);
    sprite->set_visible(ticks2action > 0 ? (ticks2action % 2) : true);
    if (ticks2action > 0)
        sprite->set_scale(bn::fixed(2) - bn::fixed(ticks2action).division(30));
}

void item::raccogli()
{
    if (raccolto) return;
    raccolto = true;
    g_dog->add_score(10);
    chr_vy = bn::fixed(-4.0);
    ticks2action = 30;
    switch (tipo)
    {
    case ITEM_TIPO_CHIAVE:
        g_dog->chiavi_raccolte++;
        break;

    case ITEM_TIPO_ENERGIA:
        g_dog->life += 30; // valore da bilanciare
        g_dog->life = capzero(g_dog->life, MAX_LIFE);
        break;

    case ITEM_TIPO_POWERUP:
        g_dog->applica_powerup(sotto_tipo);
        break;
    default:
        break;
    }
}