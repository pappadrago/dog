#pragma once
#include "enemy.h"

constexpr enemy_def schema1_enemies[] = {

    { enemy_tipo::MOVE_PATROL, enemy_attr::ATTR_FIREBALL|ATTR_AIM|ATTR_RESPAWN,  60,400, 80  },

    { enemy_tipo::MOVE_STATIC_JUMP, enemy_attr::ATTR_ARROW|ATTR_AIM|ATTR_RESPAWN,  260, 400,80  },
    { enemy_tipo::MOVE_STATIC, enemy_attr::ATTR_BOMB|ATTR_RESPAWN,  460, 400,80  },

};
constexpr int schema1_enemies_count = sizeof(schema1_enemies) / sizeof(schema1_enemies[0]);