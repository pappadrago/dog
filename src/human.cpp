#include "human.h"
#include "ball.h"
#include "hud.h"
#include "bn_random.h"
#include "bn_sprite_items_zombie.h"
#include "bn_sprite_items_bars.h"

extern bn::random random1Instance;

human::human(ball* ball) : _ball(ball)
{
    player = bn::sprite_items::zombie.create_sprite(chr_x - HALF_SCREEN_W, chr_y - CHR_FLOOR + CHR_FLOOR_HEIGHT, 0);
    player->set_bg_priority(2);
    actionStand = bn::create_sprite_animate_action_forever(
        *player, 6, bn::sprite_items::zombie.tiles_item(), 0, 1, 2, 3, 4, 5, 6, 7);
    actionLaunch = bn::create_sprite_animate_action_once(
        *player, 3, bn::sprite_items::zombie.tiles_item(), 8, 9, 10, 11, 12, 13, 14);
    barra_rossa = bn::sprite_items::bars.create_sprite(80, 100, 1);
    barra_verde = bn::sprite_items::bars.create_sprite(80, 100, 0);
    barra_verde->set_bg_priority(2);
    barra_rossa->set_bg_priority(2);
    barra_rossa->set_z_order(1);
    barra_verde->set_z_order(0);
}

void human::do_greet_and_launch()
{
    update_text(1 + random1Instance.get_int(24));
    _ball->pickedup = false;
    launching = false;
    _ball->chr_x = chr_x;
    _ball->chr_y = 2000;
    ticks = 120;
}

void human::do_launch()
{
    _ball->chr_x  = chr_x;
    _ball->chr_y  = chr_y;
    _ball->chr_vy = JUMP_VY - random1Instance.get_fixed(max_ball_speed);
    _ball->chr_vx = bn::fixed(2.5) + random1Instance.get_fixed(max_ball_speed);
    _ball->rolling = true;
    _ball->inHand  = false;
    _ball->valore  = 10;
    life -= _ball->valore;
    launching = true;
    update_text(25 + random1Instance.get_int(24));
}

void human::update()
{
    if (ticks > 0)
    {
        ticks--;
        if (ticks == 0) do_launch();
    }
    player->set_x(chr_x - HALF_SCREEN_W);
    player->set_y(chr_y - CHR_FLOOR + CHR_FLOOR_HEIGHT);
    player->set_horizontal_flip(dir == DIR_LEFT);
    barra_verde->set_x(chr_x - HALF_SCREEN_W - (16 - life.multiplication(16).division(MAX_LIFE)));
    barra_verde->set_y(chr_y - CHR_FLOOR + CHR_FLOOR_HEIGHT - 16);
    barra_rossa->set_x(chr_x - HALF_SCREEN_W);
    barra_rossa->set_y(chr_y - CHR_FLOOR + CHR_FLOOR_HEIGHT - 16);
    barra_verde->set_horizontal_scale(bn::fixed(0.01) + life.division(MAX_LIFE));
    if (launching)
    {
        if (!actionLaunch->done()) actionLaunch->update();
        else { launching = false; actionLaunch->reset(); }
    }
    else
        actionStand->update();
}
