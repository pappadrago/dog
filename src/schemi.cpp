#include "schemi.h"

#include "s1.h"
#include "s2.h"
#include "bn_regular_bg_items_bg_s1.h"
#include "bn_regular_bg_items_bg_s2.h"
#include "bn_regular_bg_items_s1.h"
#include "bn_regular_bg_items_s1fg.h"
#include "bn_regular_bg_items_s2.h"
#include "bn_regular_bg_items_s2fg.h"
// Per aggiungere uno schema: includere s2.h e gli sfondi s2/s2fg,
// aggiungere una riga qui sotto, un case nelle due create_* e alzare NUM_SCHEMI.
static constexpr collision_map_info s_maps[NUM_SCHEMI] =
{
    { collision_map_s1, collision_map_columns_s1, collision_map_rows_s1 },
    { collision_map_s2, collision_map_columns_s2, collision_map_rows_s2 },
};

// --- Porte ---
// x, y = centro della porta in px (mondo). Per una porta appoggiata sul pavimento:
// y = (riga_del_pavimento * 16) - 16.
// dest_door = indice della porta di arrivo nello schema di destinazione:
// il cane compare sopra quella porta.
static constexpr door_info s1_doors[] =
{
    //   x    y  dest_schema  dest_door
    {   16, 448,           2,         1 },   // porta 0 -> porta 1 (stesso schema, per provare)
    {  1526, 432,           2,         0 },   // porta 1 -> porta 0
};
static constexpr door_info s2_doors[] =
{
    //   x    y  dest_schema  dest_door
    {   16, 432,           1,         1 },   // porta 0 -> porta 1 (stesso schema, per provare)
    {  1526, 464,           1,         0 },   // porta 1 -> porta 0
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

const collision_map_info& get_collision_map(int schema)
{
    int i = schema - 1;
    if (i < 0 || i >= NUM_SCHEMI)
        i = 0;
    return s_maps[i];
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

bn::regular_bg_ptr create_schema_bg(int schema)
{
    switch (schema)
    {
     case 2: return bn::regular_bg_items::s2.create_bg(0);
    case 1:
    default:
        return bn::regular_bg_items::s1.create_bg(0);
    }
}

bn::regular_bg_ptr create_schema_fg(int schema)
{
    switch (schema)
    {
     case 2: return bn::regular_bg_items::s2fg.create_bg(0);
    case 1:
    default:
        return bn::regular_bg_items::s1fg.create_bg(0);
    }
}
