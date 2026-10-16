#pragma once
#include "enemy.h"

constexpr enemy_def schema1_enemies[] = {

// { enemy_tipo::MOVE_MELEE, enemy_attr::ATTR_MELEE | ATTR_RESPAWN,  660,400, 80 },
// { enemy_tipo::MOVE_MELEE, enemy_attr::ATTR_BASH | ATTR_RESPAWN,  360,400, 80 },
// { enemy_tipo::MOVE_CHASE_SHY, enemy_attr::ATTR_FIREBALL | ATTR_AIM | ATTR_RESPAWN,  360,400, 80 },
{ enemy_tipo::MOVE_GHOST, enemy_attr::ATTR_NONE | ATTR_RESPAWN,  260, 400, 80 },
// { enemy_tipo::MOVE_STATIC, enemy_attr::ATTR_BOMB | ATTR_RESPAWN,  460, 400, 80 },
// { enemy_tipo::MOVE_CHASE, enemy_attr::ATTR_BOMB | ATTR_RESPAWN,  960, 400, 80 },
// { enemy_tipo::MOVE_CHASE, enemy_attr::ATTR_BOMB | ATTR_RESPAWN,  860, 400, 80 },
// { enemy_tipo::MOVE_CHASE_SHY, enemy_attr::ATTR_FIREBALL | ATTR_AIM | ATTR_RESPAWN,  660,400, 80 },
// { enemy_tipo::MOVE_STATIC_JUMP, enemy_attr::ATTR_ARROW | ATTR_AIM | ATTR_RESPAWN,  1260, 400, 80 },
// { enemy_tipo::MOVE_STATIC, enemy_attr::ATTR_BOMB | ATTR_RESPAWN,  1460, 400, 80 },

};
constexpr int schema1_enemies_count = sizeof(schema1_enemies) / sizeof(schema1_enemies[0]);