#pragma once

#include <stdint.h>
#include "bn_regular_bg_ptr.h"
#include "bn_regular_bg_item.h"
#include "piattaforma.h"
#include "piattaforma_mobile.h"
#include "lava.h"
#include "door.h"
#include "enemy.h"
#include "item.h"
#include "trampolino.h"



struct collision_map_info
{
    const uint8_t* data;
    int columns;
    int rows;
    int map_w;
    int map_h;
};

// schema: 1..NUM_SCHEMI (valori fuori range -> schema 1)
const collision_map_info& get_collision_map(int schema);


// Porte dello schema (valori fuori range -> schema 1)
const door_def* get_schema_doors(int schema, int& count);

// Sfondi di parallasse (lontano / medio) dello schema
bn::regular_bg_ptr create_schema_bg0(int schema);
bn::regular_bg_ptr create_schema_bg1(int schema);

// Livello tile (dietro) e primo piano (davanti) dello schema
bn::regular_bg_ptr create_schema_platform(int schema);
bn::regular_bg_ptr create_schema_foreground(int schema);
bn::regular_bg_item create_schema_bg_item(int schema);

void carica_collisione_schema(int schema);
void imposta_collisione(int tile_x, int tile_y, uint8_t value);
void imposta_collisione_rect(int tile_x1, int tile_y1, int tile_x2, int tile_y2, uint8_t value);

// Riferimento all'asset regular_bg del livello (s1/s2/...) dello schema, per costruirne una copia scrivibile
const bn::regular_bg_item& get_schema_sprite_level_bg_item(int schema);
const piattaforma_def* get_schema_piattaforme(int schema, int& count);
const lava_zona_def* get_schema_lava(int schema, int& count);
const piattaforma_mobile_def* get_schema_piattaforme_mobili(int schema, int& count);
const item_def* get_schema_items(int schema, int& count);
const enemy_def* get_schema_enemies(int schema, int& count);
const trampolino_def* get_schema_trampolini(int schema, int& count);
const cassa_def* get_schema_casse(int schema, int& count);

struct schema_def
{
    const cassa_def* casse; int casse_count;
    const trampolino_def* trampolini; int trampolini_count;
    const lava_zona_def* lava; int lava_count;
    const item_def* items; int items_count;
    const piattaforma_def* piattaforme; int piattaforme_count;
    const piattaforma_mobile_def* piattaforme_mobili; int piattaforme_mobili_count;
    const enemy_def* enemies; int enemies_count;
    const door_def* doors; int doors_count;
    const bn::regular_bg_item sprite_level_bg_item;  // livello sprite
    const bn::regular_bg_item parallax_bg_item;  // background in parallasse 0 e 1
    const bn::regular_bg_item foreground_item;  // foreground, davanti agli sprite
    const collision_map_info* map_info;
};

