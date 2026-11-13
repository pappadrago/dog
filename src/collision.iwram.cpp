#include "collision.h"
#include "dog.h"
#include "enemy.h"
#include "cassa.h"
#include "boss.h"
#include "bn_log.h"
#include "game_constants.h"
#include "globals.h"
#include "bn_math.h"


bool check_collision_16(const obj& object) {
    bn::fixed dog_hw = 12, dog_hh = 6;
    bn::fixed ball_hw = 8, ball_hh = 8;

    bn::fixed dog_x = g_dog->chr_x;
    bn::fixed dog_y = g_dog->chr_y;

    return bn::abs(object.chr_x - dog_x) < (dog_hw + ball_hw) &&
        bn::abs(object.chr_y - dog_y) < (dog_hh + ball_hh);
}

bool check_collision_16(const obj& ball, const obj& _dog)
{
    bn::fixed dog_hw = 8, dog_hh = 8;
    bn::fixed ball_hw = 8, ball_hh = 8;

    return bn::abs(ball.chr_x - _dog.chr_x) < (dog_hw + ball_hw) &&
        bn::abs(ball.chr_y - _dog.chr_y) < (dog_hh + ball_hh);
}

// collision.cpp
bool check_collision_boss(const obj& c)
{
    bn::fixed meta_boss = g_boss->dimensione.division(2);
    return bn::abs(c.chr_x - g_boss->chr_x) < (meta_boss + c.box_halfdim) &&
        bn::abs(c.chr_y - g_boss->chr_y) < (meta_boss + c.box_halfdim);
}


bool check_collision_melee_boss(const obj& _boss)
{
    bn::fixed dog_hw = 12, dog_hh = 6;
    bn::fixed boss_hw = g_boss->dimensione.division(2) + (20 - g_boss->meleeTicks);   // il colpo "si estende"
    bn::fixed boss_hh = g_boss->dimensione.division(2);

    return bn::abs(_boss.chr_x - g_dog->chr_x) < (dog_hw + boss_hw) &&
        bn::abs(_boss.chr_y - g_dog->chr_y) < (dog_hh + boss_hh);
}

bool check_collision_16_boss(const obj& _boss)
{
    bn::fixed dog_hw = 8, dog_hh = 8;
    bn::fixed boss_hw = g_boss->dimensione.division(2);
    bn::fixed boss_hh = g_boss->dimensione.division(2);

    return bn::abs(_boss.chr_x - g_dog->chr_x) < (dog_hw + boss_hw) &&
        bn::abs(_boss.chr_y - g_dog->chr_y) < (dog_hh + boss_hh);
}