#pragma once

#define WALL_JUMP_VX bn::fixed(2.5)
#define WALL_JUMP_VY bn::fixed(-2.0) 

#define JUMP_VY bn::fixed(-3.8) // impulso verticale iniziale
#define MEDIUM_JUMP_VY bn::fixed(-2.8)
#define SMALL_JUMP_VY bn::fixed(-1.8)

#define BIG_VX_R bn::fixed(4.0)
#define MEDIUM_VX_R bn::fixed(3.0)
#define SMALL_VX_R bn::fixed(2.0)
#define SLOW_VX_R bn::fixed(1.0)

#define BIG_VX_L bn::fixed(-4.0)
#define MEDIUM_VX_L bn::fixed(-3.0)
#define SMALL_VX_L bn::fixed(-2.0)
#define SLOW_VX_L bn::fixed(-1.0)

#define ENEMY_BIG_JUMP_VY bn::fixed(-4.0) 
#define ENEMY_MEDIUM_JUMP_VY bn::fixed(-3.0) 
#define ENEMY_SMALL_JUMP_VY bn::fixed(-2.0) 

#define DOUBLE_JUMP_VY bn::fixed(-3.0) // impulso verticale aggiuntivo per il doppio salto
#define GRAVITY bn::fixed(.15)          // gravita' per frame
#define FRICTION_GROUND bn::fixed(-.1)
#define FRICTION_AIR bn::fixed(-.02)
#define MAX_FALL bn::fixed(5.5) // velocita' caduta massima

#define WALL_SLIDE_VY bn::fixed(0.8)   // velocità di discesa lungo il muro (contro MAX_FALL_SPEED = 5.5), da bilanciare

#define DOG_RIMBALZO_TESTA_NEMICI bn::fixed(-2.0)
#define DOG_RIMBALZO_LAVA bn::fixed(-3.5)