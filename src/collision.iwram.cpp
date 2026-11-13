#include "collision.h"
#include "dog.h"
#include "enemy.h"
#include "cassa.h"
#include "boss.h"
#include "bn_log.h"
#include "game_constants.h"
#include "globals.h"

namespace
{
    inline bool collision_aabb(int x1, int y1, int x2, int y2, int half_width, int half_height)
    {
        int result;
        int difference;

        asm volatile(
            "subs %[difference], %[x1], %[x2]\n"
            "rsblt %[difference], %[difference], #0\n"
            "cmp %[difference], %[half_width]\n"
            "movlt %[result], #1\n"
            "movge %[result], #0\n"
            "subs %[difference], %[y1], %[y2]\n"
            "rsblt %[difference], %[difference], #0\n"
            "cmp %[difference], %[half_height]\n"
            "movge %[result], #0\n"
            : [result] "=&r" (result), [difference] "=&r" (difference)
            : [x1] "r" (x1), [y1] "r" (y1), [x2] "r" (x2), [y2] "r" (y2),
              [half_width] "r" (half_width), [half_height] "r" (half_height)
            : "cc");

        return result != 0;
    }
}

bool check_collision_16(const obj& object) {
    bn::fixed dog_hw = 12, dog_hh = 6;
    bn::fixed ball_hw = 8, ball_hh = 8;

    return collision_aabb(object.chr_x.data(), object.chr_y.data(),
        g_dog->chr_x.data(), g_dog->chr_y.data(),
        (dog_hw + ball_hw).data(), (dog_hh + ball_hh).data());
}

bool check_collision_16(const obj& ball, const obj& _dog)
{
    bn::fixed dog_hw = 8, dog_hh = 8;
    bn::fixed ball_hw = 8, ball_hh = 8;

    return collision_aabb(ball.chr_x.data(), ball.chr_y.data(),
        _dog.chr_x.data(), _dog.chr_y.data(),
        (dog_hw + ball_hw).data(), (dog_hh + ball_hh).data());
}

// collision.cpp
bool check_collision_boss(const obj& c)
{
    bn::fixed meta_boss = g_boss->box_halfdim;
    return collision_aabb(c.chr_x.data(), c.chr_y.data(),
        g_boss->chr_x.data(), g_boss->chr_y.data(),
        (meta_boss + c.box_halfdim).data(), (meta_boss + c.box_halfdim).data());
}


bool check_collision_melee_boss(const obj& _boss)
{
    bn::fixed dog_hw = 12, dog_hh = 6;
    bn::fixed boss_hw = g_boss->box_halfdim + (20 - g_boss->meleeTicks);   // il colpo "si estende"
    bn::fixed boss_hh = g_boss->box_halfdim;

    return collision_aabb(_boss.chr_x.data(), _boss.chr_y.data(),
        g_dog->chr_x.data(), g_dog->chr_y.data(),
        (dog_hw + boss_hw).data(), (dog_hh + boss_hh).data());
}

bool check_collision_16_boss(const obj& _boss)
{
    bn::fixed dog_hw = 8, dog_hh = 8;
    bn::fixed boss_hw = g_boss->box_halfdim;
    bn::fixed boss_hh = g_boss->box_halfdim;

    return collision_aabb(_boss.chr_x.data(), _boss.chr_y.data(),
        g_dog->chr_x.data(), g_dog->chr_y.data(),
        (dog_hw + boss_hw).data(), (dog_hh + boss_hh).data());
}