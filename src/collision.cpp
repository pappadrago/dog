#include "collision.h"
#include "dog.h"
#include "enemy.h"
#include "cassa.h"
#include "boss.h"
#include "bn_log.h"
#include "game_constants.h"
#include "globals.h"
#include "bn_math.h"

bool check_collision(const bn::sprite_ptr& s1, const bn::sprite_ptr& s2)
{
    bn::fixed dx = s1.x() - s2.x();
    if (dx > s1.dimensions().width() || dx < -s1.dimensions().width())
        return false;

    bn::fixed_rect rect1(s1.x(), s1.y(),
        bn::fixed(0.8).multiplication(s1.dimensions().width()),
        bn::fixed(0.8).multiplication(s1.dimensions().height()));

    bn::fixed_rect rect2(s2.x(), s2.y(),
        bn::fixed(0.8).multiplication(s2.dimensions().width()),
        bn::fixed(0.8).multiplication(s2.dimensions().height()));

    return rect1.intersects(rect2);
}



bool check_collision_melee(const enemy& _enemy) {
    bn::fixed dog_x = g_dog->sprite->x();
    bn::fixed dog_y = g_dog->sprite->y();

    bn::fixed enem_x = _enemy.sprite->x();
    bn::fixed enem_y = _enemy.sprite->y();

    bn::fixed dog_hw = 12;
    bn::fixed dog_hh = 13;
    bn::fixed enem_hw = 18-(_enemy.meleeTicks);
    bn::fixed enem_hh = 6;

    bool hit = bn::abs(enem_x - dog_x) < (dog_hw + enem_hw) &&
        bn::abs(enem_y - dog_y) < (dog_hh + enem_hh);

    return hit;
}
bool check_collision_16(const bn::sprite_ptr& sprite){
        bn::fixed dog_hw = 8, dog_hh = 8;
    bn::fixed ball_hw = 8, ball_hh = 8;

        bn::fixed dog_x = g_dog->sprite->x();
    bn::fixed dog_y = g_dog->sprite->y();

    bn::fixed enem_x = sprite.x();
    bn::fixed enem_y = sprite.y();

    return bn::abs(enem_x - dog_x) < (dog_hw + ball_hw) &&
        bn::abs(enem_y -dog_y) < (dog_hh + ball_hh);
}

bool check_collision_16(const bn::sprite_ptr& ball, const bn::sprite_ptr& _dog)
{
    bn::fixed dog_hw = 8, dog_hh = 8;
    bn::fixed ball_hw = 8, ball_hh = 8;

    return bn::abs(ball.x() - _dog.x()) < (dog_hw + ball_hw) &&
        bn::abs(ball.y() - _dog.y()) < (dog_hh + ball_hh);
}
bool check_collision_16(const enemy& _enemy)
{
    bn::fixed dog_hw = 8, dog_hh = 8;
    bn::fixed ball_hw = 8, ball_hh = 8;

        bn::fixed dog_x = g_dog->sprite->x();
    bn::fixed dog_y = g_dog->sprite->y();

    bn::fixed enem_x = _enemy.sprite->x();
    bn::fixed enem_y = _enemy.sprite->y();

    return bn::abs(enem_x - dog_x) < (dog_hw + ball_hw) &&
        bn::abs(enem_y -dog_y) < (dog_hh + ball_hh);
}

// collision.cpp
bool check_collision_boss(const cassa& c)
{
    bn::fixed meta_boss = g_boss->dimensione/2;
    return bn::abs(c.chr_x - g_boss->chr_x) < (meta_boss + c.box_halfdim) &&
           bn::abs(c.chr_y - g_boss->chr_y) < (meta_boss + c.box_halfdim);
}


bool check_collision_melee_boss(const boss& _boss)
{
    bn::fixed dog_x = g_dog->sprite->x();
    bn::fixed dog_y = g_dog->sprite->y();
    bn::fixed boss_x = _boss.sprite->x();
    bn::fixed boss_y = _boss.sprite->y();

    bn::fixed dog_hw = 12, dog_hh = 13;
    bn::fixed boss_hw = g_boss->dimensione.division(2) + (20 - _boss.meleeTicks);   // il colpo "si estende"
    bn::fixed boss_hh = g_boss->dimensione.division(2);

    return bn::abs(boss_x - dog_x) < (dog_hw + boss_hw) &&
           bn::abs(boss_y - dog_y) < (dog_hh + boss_hh);
}

bool check_collision_16_boss(const boss& _boss)
{
    bn::fixed dog_x = g_dog->sprite->x();
    bn::fixed dog_y = g_dog->sprite->y();
    bn::fixed boss_x = _boss.sprite->x();
    bn::fixed boss_y = _boss.sprite->y();

    bn::fixed dog_hw = 8, dog_hh = 8;
    bn::fixed boss_hw = g_boss->dimensione.division(2);
    bn::fixed boss_hh = g_boss->dimensione.division(2);

    return bn::abs(boss_x - dog_x) < (dog_hw + boss_hw) &&
           bn::abs(boss_y - dog_y) < (dog_hh + boss_hh);
}