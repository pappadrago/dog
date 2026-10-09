#pragma once


#define JUMP_VY bn::fixed(-4) // impulso verticale iniziale

#define DOUBLE_JUMP_VY bn::fixed(-3.0) // impulso verticale aggiuntivo per il doppio salto
#define GRAVITY bn::fixed(.15)          // gravita' per frame
#define FRICTION_GROUND bn::fixed(-.1)
#define FRICTION_AIR bn::fixed(-.02)
#define MAX_FALL bn::fixed(5.5) // velocita' caduta massima

#define WALL_SLIDE_VY bn::fixed(0.8)   // velocità di discesa lungo il muro (contro MAX_FALL_SPEED = 5.5), da bilanciare
