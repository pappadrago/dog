#include "collision.h"
#include "dog.h"
#include "game_constants.h"
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

bool check_collision_ball_dog(const bn::sprite_ptr& ball, const dog& _dog)
{
    bn::fixed dog_x  = _dog.player->x();
    bn::fixed dog_y  = _dog.player->y() + 6;
    bn::fixed dog_hw = 8;
    bn::fixed dog_hh = 13;
    bn::fixed ball_hw = 4;
    bn::fixed ball_hh = 4;

    return bn::abs(ball.x() - dog_x) < (dog_hw + ball_hw) &&
           bn::abs(ball.y() - dog_y) < (dog_hh + ball_hh);
}

bool check_collision_16(const bn::sprite_ptr& ball, const dog& _dog)
{
    bn::fixed dog_x  = _dog.player->x();
    bn::fixed dog_y  = _dog.player->y() + 6;
    bn::fixed dog_hw = 8;
    bn::fixed dog_hh = 13;
    bn::fixed ball_hw = 6;
    bn::fixed ball_hh = 6;

    return bn::abs(ball.x() - dog_x) < (dog_hw + ball_hw) &&
           bn::abs(ball.y() - dog_y) < (dog_hh + ball_hh);
}

bool check_collision_16(const bn::sprite_ptr& ball, const bn::sprite_ptr& _dog)
{
    bn::fixed dog_hw = 8, dog_hh = 8;
    bn::fixed ball_hw = 8, ball_hh = 8;

    return bn::abs(ball.x() - _dog.x()) < (dog_hw + ball_hw) &&
           bn::abs(ball.y() - _dog.y()) < (dog_hh + ball_hh);
}
