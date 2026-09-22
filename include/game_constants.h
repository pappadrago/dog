#pragma once

#include "bn_fixed.h"

// Direzioni
#define DIR_RIGHT 1
#define DIR_LEFT -1

// Schermo
#define SCREEN_W bn::fixed(240)
#define SCREEN_H bn::fixed(160)

#define SCREEN_DG bn::fixed(288)

#define HALF_SCREEN_W bn::fixed(120)
#define HALF_SCREEN_H bn::fixed(80)

// Costanti fisiche
#define JUMP_VY       bn::fixed(-2.7)
#define DOUBLE_JUMP_VY bn::fixed(-3.0)
#define GRAVITY        bn::fixed(.2)
#define FRICTION_GROUND bn::fixed(-.1)
#define FRICTION_AIR   bn::fixed(-.02)
#define MAX_FALL       bn::fixed(5.5)

// Vita
#define MAX_LIFE 999
#define START_LIFE 100

// Limiti pool
#define MAX_ENEMY_BULLETS 16
#define MAX_BULLETS       8
#define MAX_ENEMY_BOMBS   4
#define MAX_ENEMIES       16
#define MAX_BONUSES       3


#define MAX_DOORS 4 