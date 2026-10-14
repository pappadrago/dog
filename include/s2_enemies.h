#pragma once
#include "enemy.h"

constexpr enemy_def schema2_enemies[] = {
    { enemy_tipo::MOVE_STATIC, enemy_attr::ATTR_NONE,  400, 400,0 },
    { enemy_tipo::MOVE_CHASE,                 enemy_attr::ATTR_AIM, 200,400,   150 },
    { enemy_tipo::MOVE_CHASE_SHY,                 enemy_attr::ATTR_AIM, 600, 400, 210},
    { enemy_tipo::MOVE_PATROL,          enemy_attr::ATTR_BOMB, 900, 400,  180 },
    { enemy_tipo::MOVE_STATIC,          enemy_attr::ATTR_SHELL, 1200, 400, 240 },
    { enemy_tipo::MOVE_STATIC_JUMP,       enemy_attr::ATTR_BOMB,  1400, 400,  120 },
};
constexpr int schema2_enemies_count = sizeof(schema2_enemies) / sizeof(schema2_enemies[0]);