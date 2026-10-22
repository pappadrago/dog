#include "schemi.h"
#include "megadog_s1.h"
#include "s1.h"
#include "s2.h"

static constexpr int MAX_COLLISION_CELLE = 192 * 32;   // in tile di gioco (16px), non hardware
static uint8_t s_collision_runtime[MAX_COLLISION_CELLE];
static int s_schema_caricato = -1;
static collision_map_info s_attiva;


void imposta_collisione(int tile_x, int tile_y, uint8_t value)
{
    if (tile_x < 0 || tile_x >= s_attiva.columns) return;
    if (tile_y < 0 || tile_y >= s_attiva.rows) return;
    s_collision_runtime[tile_y * s_attiva.columns + tile_x] = value;
}

void imposta_collisione_rect(int x1, int y1, int x2, int y2, uint8_t value)
{
    for (int ty = y1; ty <= y2; ty++)
        for (int tx = x1; tx <= x2; tx++)
            imposta_collisione(tx, ty, value);
}

constexpr schema_def schemi[] = {
    megadog_s1::make_schema_A(),
make_schema_1(),
make_schema_2()
};
constexpr int NUM_SCHEMI = sizeof(schemi) / sizeof(schemi[0]);

static const schema_def& S(int schema)
{
    int i = schema - 1;
    if (i < 0 || i >= NUM_SCHEMI) i = 0;   // come il vecchio fallback
    return schemi[i];
}

const trampolino_def* get_schema_trampolini(int schema, int& count)
{
    const schema_def& s = S(schema);
    count = s.trampolini_count;
    return s.trampolini;
}

const cassa_def* get_schema_casse(int schema, int& count)
{
    const schema_def& s = S(schema);
    count = s.casse_count;
    return s.casse;
}

const enemy_def* get_schema_enemies(int schema, int& count)
{
    const schema_def& s = S(schema);
    count = s.enemies_count;
    return s.enemies;
}

const lava_zona_def* get_schema_lava(int schema, int& count)
{
    const schema_def& s = S(schema);
    count = s.lava_count;
    return s.lava;
}

const piattaforma_def* get_schema_piattaforme(int schema, int& count)
{
    const schema_def& s = S(schema);
    count = s.piattaforme_count;
    return s.piattaforme;
}

const piattaforma_mobile_def* get_schema_piattaforme_mobili(int schema, int& count)
{
    const schema_def& s = S(schema);
    count = s.piattaforme_mobili_count;
    return s.piattaforme_mobili;
}

const door_def* get_schema_doors(int schema, int& count)
{
    const schema_def& s = S(schema);
    count = s.doors_count;
    return s.doors;
}

const item_def* get_schema_items(int schema, int& count)
{
    const schema_def& s = S(schema);
    count = s.items_count;
    return s.items;
}

bn::regular_bg_ptr create_schema_platform(int schema)
{
    return S(schema).sprite_level_bg_item.create_bg(0);
}

bn::regular_bg_item create_schema_bg_item(int schema)
{
    return S(schema).sprite_level_bg_item;
}

const bn::regular_bg_item& get_schema_sprite_level_bg_item(int schema)
{
    return S(schema).sprite_level_bg_item;
}

bn::regular_bg_ptr create_schema_bg0(int schema)
{
    return S(schema).parallax_bg_item.create_bg(0);
}

bn::regular_bg_ptr create_schema_bg1(int schema)
{
    return S(schema).parallax_bg_item.create_bg(1);
}

bn::regular_bg_ptr create_schema_foreground(int schema)
{
    return S(schema).foreground_item.create_bg(0);
}

void carica_collisione_schema(int schema)
{
    const collision_map_info& originale = *S(schema).map_info;
    int celle = originale.columns * originale.rows;

    BN_ASSERT(celle <= MAX_COLLISION_CELLE, "collisione: buffer troppo piccolo");

    for (int k = 0; k < celle; k++)
        s_collision_runtime[k] = originale.data[k];

    s_schema_caricato = schema;
    s_attiva = originale;
    s_attiva.data = s_collision_runtime;
}

const collision_map_info& get_collision_map(int schema)
{
    if (schema == s_schema_caricato)
        return s_attiva;
    return *S(schema).map_info;
}
