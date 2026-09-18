#pragma once

#include "bn_optional.h"
#include "bn_sprite_ptr.h"
#include "bn_sprite_animate_actions.h"
#include "bn_sprite_item.h"
#include "bn_fixed.h"
#include "bn_vector.h"
#include "bn_camera_ptr.h"
#include "game_constants.h"

struct GameContext;  // forward declaration
class ball;
class human;
class enemy;

class helper_dog
{
public:
    static constexpr int NONE = -1;
    static constexpr int GO_TO_BALL = 0;
    static constexpr int CARRY_BALL = 1;
    static constexpr int DONE = 2;

    bn::optional<bn::sprite_ptr>               player;
    bn::optional<bn::sprite_animate_action<5>> actionStand;
    bn::optional<bn::sprite_animate_action<6>> actionWalk;
    bn::optional<bn::sprite_item>              dog_item;

    bn::fixed chr_x = 0;
    bn::fixed chr_y = CHR_FLOOR;
    bn::fixed chr_vx = 0;
    bn::fixed chr_vy = 0;

    int  dir = DIR_RIGHT;
    bool walking = false;
    bool jumping = false;
    bool active = false;
    int  state = NONE;
    int  invincibile = 0;

    static constexpr bn::fixed GROUND_ACCEL = bn::fixed(0.15);
    static constexpr bn::fixed GROUND_FRICTION = bn::fixed(0.08);
    static constexpr bn::fixed AIR_ACCEL = bn::fixed(0.06);
    static constexpr bn::fixed AIR_FRICTION = bn::fixed(0.02);
    static constexpr bn::fixed MAX_VX = bn::fixed(2.0);
    static constexpr bn::fixed AIR_MAX_VX = bn::fixed(2.0);
    static constexpr bn::fixed HELP_JUMP_VY = bn::fixed(-4.0);

    GameContext* ctx;
    helper_dog(GameContext* ctx);

    void spawn(bn::fixed start_x);
    void despawn();
    bool update();   // returns true when ball delivered

private:
    void move_toward(bn::fixed target_x);
    void try_jump_over_enemies();
    void apply_physics();
    bool check_enemy_collisions();
    void update_animations();
};
