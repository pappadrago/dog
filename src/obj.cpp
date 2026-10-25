#include "obj.h"
#include "globals.h"
#include "dog.h"
#include "bau.h"
#include "enemy.h"
#include "collision.h"
#include "bn_keypad.h"
#include "bn_math.h"
#include "bn_random.h"
#include "bn_sound_items.h"
#include "bn_log.h"
#include "schemi.h"


void live_obj::stacca_da_scala(bn::fixed vx, bn::fixed vy)
{
    scala_corrente = nullptr;
    chr_vx = vx;
    chr_vy = vy;
}