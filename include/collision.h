#pragma once

#include "bn_sprite_ptr.h"
#include "bn_fixed_rect.h"
#include "obj.h"

bool check_collision(const bn::sprite_ptr& s1, const bn::sprite_ptr& s2);
bool check_collision_16(const obj& ball, const obj& _dog);

bool check_collision_16(const obj& object);
// collision.h
bool check_collision_boss(const obj& object);

bool check_collision_melee_boss(const obj& object);
bool check_collision_16_boss(const obj& object);