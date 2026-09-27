#pragma once

#include "bn_sprite_ptr.h"
#include "bn_fixed_rect.h"
#include "enemy.h"
#include "cassa.h"

class dog;

bool check_collision(const bn::sprite_ptr& s1, const bn::sprite_ptr& s2);
bool check_collision_16(const bn::sprite_ptr& ball, const bn::sprite_ptr& _dog);

bool check_collision_melee(const enemy& _enemy);
bool check_collision_16(const enemy& _enemy);
bool check_collision_16(const bn::sprite_ptr& sprite);
// collision.h
bool check_collision_boss(const cassa& c);

class boss;
bool check_collision_melee_boss(const boss& _boss);
bool check_collision_16_boss(const boss& _boss);