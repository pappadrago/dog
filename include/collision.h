#pragma once

#include "bn_sprite_ptr.h"
#include "bn_fixed_rect.h"
#include "enemy.h"

class dog;

bool check_collision(const bn::sprite_ptr& s1, const bn::sprite_ptr& s2);
bool check_collision_16(const bn::sprite_ptr& ball, const bn::sprite_ptr& _dog);

bool check_collision_melee(const enemy& _enemy);
bool check_collision_16(const enemy& _enemy);
bool check_collision_16(const bn::sprite_ptr& sprite);