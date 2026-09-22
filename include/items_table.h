#pragma once
#include "item.h"
#include "s1_items.h"
#include "s2_items.h"

inline const item_def* get_schema_items(int schema, int& count)
{
    switch (schema)
    {
    case 1:
        count = schema1_items_count;
        return schema1_items;
    case 2:
        count = schema2_items_count;
        return schema2_items;
    default:
        count = 0;
        return nullptr;
    }
}