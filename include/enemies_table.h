#pragma once
#include "enemy.h"
#include "s1_enemies.h"
#include "s2_enemies.h"

inline const enemy_def* get_schema_enemies(int schema, int& count)
{
    switch (schema)
    {
    case 1:
        count = schema1_enemies_count;
        return schema1_enemies;
    case 2:
        count = schema2_enemies_count;
        return schema2_enemies;
    default:
        count = 0;
        return nullptr;
    }
}