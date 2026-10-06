#include "schemi.h"

#include "s1.h"
#include "s2.h"
#include "bn_regular_bg_items_bg_s1.h"
#include "bn_regular_bg_items_bg_s2.h"
#include "bn_regular_bg_items_s1.h"
#include "bn_regular_bg_items_s1fg.h"
#include "bn_regular_bg_items_s2.h"
#include "bn_regular_bg_items_s2fg.h"

#include "s1_piattaforme.h"
#include "s1_piattaforme_mobili.h"
#include "s2_piattaforme.h"
#include "s2_piattaforme_mobili.h"

#include "s1_lava.h"

// Per aggiungere uno schema: includere s2.h e gli sfondi s2/s2fg,
// aggiungere una riga qui sotto, un case nelle due create_* e alzare NUM_SCHEMI.
static constexpr collision_map_info s_maps[NUM_SCHEMI] =
{
    { collision_map_s1, collision_map_columns_s1, collision_map_rows_s1, collision_map_columns_s1 * 16,collision_map_rows_s1 * 16 },
    { collision_map_s2, collision_map_columns_s2, collision_map_rows_s2, collision_map_columns_s2 * 16,collision_map_rows_s2 * 16 },
};

// --- Porte ---
// x, y = centro della porta in px (mondo). Per una porta appoggiata sul pavimento:
// y = (riga_del_pavimento * 16) - 16.
// dest_door = indice della porta di arrivo nello schema di destinazione:
// il cane compare sopra quella porta.
static constexpr door_info s1_doors[] =
{
    //   x    y  dest_schema  dest_door
    { 16, 448,           2,         1 },   // porta 0 -> porta 1 (stesso schema, per provare)
    { 1526, 432,           2,         0 },   // porta 1 -> porta 0
};
static constexpr door_info s2_doors[] =
{
    //   x    y  dest_schema  dest_door
    { 16, 432,           1,         1 },   // porta 0 -> porta 1 (stesso schema, per provare)
    { 1510, 464,           1,         0 },   // porta 1 -> porta 0
};

static constexpr door_list s_doors[NUM_SCHEMI] =
{
    { s1_doors, int(sizeof(s1_doors) / sizeof(s1_doors[0])) },
    { s2_doors, int(sizeof(s2_doors) / sizeof(s2_doors[0])) },
};

const door_list& get_schema_doors(int schema)
{
    int i = schema - 1;
    if (i < 0 || i >= NUM_SCHEMI)
        i = 0;
    return s_doors[i];
}

bn::regular_bg_ptr create_schema_bg0(int schema)
{
    switch (schema)
    {
    case 2: return bn::regular_bg_items::bg_s2.create_bg(0);
    case 1:
    default:
        return bn::regular_bg_items::bg_s1.create_bg(0);
    }
}

bn::regular_bg_ptr create_schema_bg1(int schema)
{
    switch (schema)
    {
    case 2: return bn::regular_bg_items::bg_s2.create_bg(1);
    case 1:
    default:
        return bn::regular_bg_items::bg_s1.create_bg(1);
    }
}

bn::regular_bg_ptr create_schema_platform(int schema)
{
    switch (schema)
    {
    case 2: return bn::regular_bg_items::s2.create_bg(0);
    case 1:
    default:
        return bn::regular_bg_items::s1.create_bg(0);
    }
}

bn::regular_bg_item create_schema_bg_item(int schema)
{
    switch (schema)
    {
    case 2:
        return bn::regular_bg_items::s2;
    case 1:
    default:
        return bn::regular_bg_items::s1;
    }
}


bn::regular_bg_ptr create_schema_foreground(int schema)
{
    switch (schema)
    {
    case 2: 
    return bn::regular_bg_items::s2fg.create_bg(0);
    case 1:
    default:
        return bn::regular_bg_items::s1fg.create_bg(0);
    }
}

static constexpr int MAX_COLLISION_CELLE = 96 * 32;   // in tile di gioco (16px), non hardware
static uint8_t s_collision_runtime[MAX_COLLISION_CELLE];
static int s_schema_caricato = -1;
static collision_map_info s_attiva;

void carica_collisione_schema(int schema)
{
    int i = schema - 1;
    if (i < 0 || i >= NUM_SCHEMI) i = 0;

    const collision_map_info& originale = s_maps[i];
    int celle = originale.columns * originale.rows;

    BN_ASSERT(celle <= MAX_COLLISION_CELLE, "collisione: buffer troppo piccolo");

    for (int k = 0; k < celle; k++)
        s_collision_runtime[k] = originale.data[k];

    s_schema_caricato = schema;
    s_attiva = originale;
    s_attiva.data = s_collision_runtime;
}

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

const collision_map_info& get_collision_map(int schema)
{
    if (schema == s_schema_caricato)
        return s_attiva;

    int i = schema - 1;
    if (i < 0 || i >= NUM_SCHEMI) i = 0;
    return s_maps[i];
}

const bn::regular_bg_item& get_schema_platform_item(int schema)
{
    switch (schema)
    {
    case 2: return bn::regular_bg_items::s2;
    case 1:
    default: return bn::regular_bg_items::s1;
    }
}


const piattaforma_def* get_schema_piattaforme(int schema, int& count)
{
    switch (schema)
    {
    case 1:
        count = schema1_piattaforme_count;
        return schema1_piattaforme;
    case 2:
        // count = schema2_piattaforme_count;
        // return schema2_piattaforme;
        count = 0;
        return nullptr;   // finché s2 non ha piattaforme proprie
    default:
        count = 0;
        return nullptr;
    }
}

const lava_zona_def* get_schema_lava(int schema, int& count)
{
    switch (schema)
    {
    case 1:
        count = schema1_lava_count;
        return schema1_lava;
    case 2:
        // count = schema2_lava_count;
        // return schema2_lava;
        count = 0;
        return nullptr;   // finché s2 non ha lava propria
    default:
        count = 0;
        return nullptr;
    }
}


const piattaforma_mobile_def* get_schema_piattaforme_mobili(int schema, int& count)
{
    switch (schema)
    {
    case 1:
        count = schema1_piattaforme_mobili_count;
        return schema1_piattaforme_mobili;
    case 2:
        count = schema2_piattaforme_mobili_count;
        return schema2_piattaforme_mobili;
    default:
        count = 0;
        return nullptr;
    }
}