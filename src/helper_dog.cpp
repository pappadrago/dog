#include "helper_dog.h"
#include "ball.h"
#include "human.h"
#include "enemy.h"
#include "collision.h"
#include "bn_sprite_items_fox.h"
#include "bn_math.h"
#include "maxmod.h"
#include "game_context.h"

helper_dog::helper_dog(GameContext*    _ctx)
    : ctx(_ctx)
{
    dog_item = bn::sprite_items::fox;
    player = dog_item->create_sprite(
        chr_x - HALF_SCREEN_W, chr_y - CHR_FLOOR + CHR_FLOOR_HEIGHT, 0);
    player->set_bg_priority(2);
    
    actionStand = bn::create_sprite_animate_action_forever(
        *player, 5, dog_item->tiles_item(), 0, 1, 2, 3, 4);
    actionWalk = bn::create_sprite_animate_action_forever(
        *player, 3, dog_item->tiles_item(), 6, 7, 8, 9, 10, 11);
    player->set_visible(false);
}

void helper_dog::spawn(bn::fixed start_x)
{
    chr_x = start_x; chr_y = CHR_FLOOR;
    chr_vx = 0; chr_vy = 0;
    player->set_camera(ctx->camera);
    dir = DIR_RIGHT; jumping = false; walking = false;
    active = true; state = GO_TO_BALL; invincibile = 0;
    mmSetModuleTempo(1536);
    player->set_visible(true);
}

void helper_dog::despawn()
{
    active = false; state = NONE;
    player->set_visible(false);
}

void helper_dog::move_toward(bn::fixed target_x)
{
    bn::fixed dx = target_x - chr_x;
    if (bn::abs(dx) > 8)
    {
        dir = (dx > 0) ? DIR_RIGHT : DIR_LEFT;
        walking = true;
        chr_vx += GROUND_ACCEL;
        if (chr_vx > MAX_VX) chr_vx = MAX_VX;
    }
    else
    {
        walking = false;
        chr_vx -= GROUND_FRICTION;
        if (chr_vx < 0) chr_vx = 0;
    }
}

void helper_dog::try_jump_over_enemies()
{
    if (jumping) return;
    for (const enemy& e : ctx->enemies)
    {
        bn::fixed dist = e.chr_x - chr_x;
        bool in_front = (dir == DIR_RIGHT && dist > 0 && dist < 48) ||
            (dir == DIR_LEFT && dist < 0 && dist > -48);
        if (in_front)
        {
            if (e.dir != dir) { jumping = true; chr_vy = HELP_JUMP_VY; chr_vx = MAX_VX; walking = false; break; }
            else { walking = false; chr_vx -= GROUND_FRICTION; if (chr_vx < 0) chr_vx = 0; }
        }
    }
}

void helper_dog::apply_physics()
{
    chr_x += bn::fixed(dir).multiplication(chr_vx);
    if (chr_x < 0)    chr_x = 0;
    if (chr_x > 1791) chr_x = 1791;
    if (jumping)
    {
        chr_vy += GRAVITY; if (chr_vy > MAX_FALL) chr_vy = MAX_FALL;
        chr_y += chr_vy;
        if (chr_y >= CHR_FLOOR) { chr_y = CHR_FLOOR; chr_vy = 0; jumping = false; }
    }
    player->set_x(chr_x - HALF_SCREEN_W);
    player->set_y(chr_y - CHR_FLOOR + CHR_FLOOR_HEIGHT);
    player->set_horizontal_flip(dir == DIR_LEFT);
}

bool helper_dog::check_enemy_collisions()
{
    if (invincibile > 0) { invincibile--; return false; }
    for (enemy& e : ctx->enemies)
    {
        if (e.ticks2hit > 0) continue;
        bool hit = check_collision_16(*e.player, *player);
        if (hit)
        {
            dir = -dir; chr_vy = bn::fixed(-2.0); chr_vx = bn::fixed(1.5);
            jumping = true; walking = false; invincibile = 60;
            e.dir = (e.chr_x < chr_x) ? DIR_LEFT : DIR_RIGHT;
            e.chr_vy = bn::fixed(-1.5); e.chr_vx = bn::fixed(1.5);
            e.jumping = true; e.ticks2hit = 60;
            if (ctx->the_ball.pickedup)
            {
                ctx->the_ball.pickedup = false;
                ctx->the_ball.chr_vx = bn::fixed(dir).multiplication(bn::fixed(-2.0));
                ctx->the_ball.chr_vy = JUMP_VY;
                ctx->the_ball.rolling = true;
                return true;
            }
        }
    }
    return false;
}

void helper_dog::update_animations()
{
    if (jumping);
    else if (walking) actionWalk->update();
    else              actionStand->update();
}

bool helper_dog::update()
{
    if (!active) return false;
    bool lost_ball = check_enemy_collisions();
    switch (state)
    {
    case GO_TO_BALL:
        move_toward(ctx->the_ball.chr_x);
        try_jump_over_enemies();
        if (bn::abs(chr_x - ctx->the_ball.chr_x) < 16 && !ctx->the_ball.pickedup)
        {
            ctx->the_ball.pickedup = true; 
            ctx->the_ball.pickedup_by_helper = true;
            state = CARRY_BALL;
        }
        if (lost_ball) state = GO_TO_BALL;
        break;
    case CARRY_BALL:
        if (lost_ball) { state = GO_TO_BALL; break; }
        move_toward(ctx->the_human.chr_x);
        try_jump_over_enemies();
        ctx->the_ball.chr_x = chr_x + bn::fixed(20.0).multiplication(dir);
        ctx->the_ball.chr_y = chr_y - 8;
        if (bn::abs(chr_x - ctx->the_human.chr_x) < 24)
        {
            ctx->the_ball.pickedup = false;
            ctx->the_ball.chr_x = ctx->the_human.chr_x;
            ctx->the_ball.chr_y = ctx->the_human.chr_y - 16;
            state = DONE;
            mmSetModuleTempo(1024);
            return true;
        }
        break;
    case DONE:
        dir = DIR_RIGHT; walking = true; chr_vx = MAX_VX;
        if (chr_x >= 1791) despawn();
        break;
    default: break;
    }
    apply_physics();
    update_animations();
    return false;
}
