#pragma once

#include <stdint.h>
#include "bn_regular_bg_ptr.h"

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
bn::regular_bg_ptr create_schema_bg(int schema);
bn::regular_bg_ptr create_schema_fg(int schema);
