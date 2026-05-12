#pragma once

#include "bn_fixed.h"

// Direzioni
#define DIR_RIGHT 1
#define DIR_LEFT -1

// Schermo
#define HALF_SCREEN_W 120

// Costanti fisiche
#define JUMP_VY       bn::fixed(-2.7)
#define DOUBLE_JUMP_VY bn::fixed(-3.0)
#define GRAVITY        bn::fixed(.2)
#define FRICTION       bn::fixed(-.2)
#define FRICTION_GROUND bn::fixed(-.1)
#define FRICTION_AIR   bn::fixed(-.02)
#define MAX_FALL       bn::fixed(5.5)

// Pavimento / layout
#define CHR_FLOOR        bn::fixed(224)
#define CHR_FLOOR_HEIGHT bn::fixed(32)

// Vita
#define MAX_LIFE 100

// Limiti pool
#define MAX_ENEMY_BULLETS 16
#define MAX_BULLETS       8
#define MAX_ENEMY_BOMBS   4
#define MAX_ENEMIES       12
#define MAX_BONUSES       3

// Cooldown
#define FRESH_COOLDOWN_COUNTER 30

// Stati bonus
#define BONUS_STATUS_HIDE  0
#define BONUS_STATUS_SHOW  1
#define BONUS_STATUS_SHINE 2
