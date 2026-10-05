#pragma once

#include <stdint.h>
#include "bn_regular_bg_ptr.h"
#include "piattaforma.h"
#include "piattaforma_mobile.h"
#include "lava.h"

// Numero di schemi disponibili: aumentare quando si aggiunge s2, s3...
constexpr int NUM_SCHEMI = 2;

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

// Una porta: dove sta e dove porta
struct door_info
{
    int x, y;          // centro della porta (px, coordinate mondo)
    int dest_schema;   // schema di destinazione
    int dest_door;     // indice della porta di arrivo nello schema di destinazione
};

struct door_list
{
    const door_info* data;
    int count;
};

// Porte dello schema (valori fuori range -> schema 1)
const door_list& get_schema_doors(int schema);

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
const bn::regular_bg_item& get_schema_platform_item(int schema);

const piattaforma_def* get_schema_piattaforme(int schema, int& count);
const lava_zona_def* get_schema_lava(int schema, int& count);

const piattaforma_mobile_def* get_schema_piattaforme_mobili(int schema, int& count);