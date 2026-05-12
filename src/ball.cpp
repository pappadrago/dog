#include "ball.h"
#include "bn_sprite_items_ball.h"
#include "bn_math.h"

ball::ball()
{
    player    = bn::sprite_items::ball.create_sprite(chr_x - HALF_SCREEN_W, chr_y - CHR_FLOOR + CHR_FLOOR_HEIGHT, 0);
    shadowRed = bn::sprite_items::ball.create_sprite(chr_x - HALF_SCREEN_W, chr_y - CHR_FLOOR + CHR_FLOOR_HEIGHT, 2);
    shadow    = bn::sprite_items::ball.create_sprite(chr_x - HALF_SCREEN_W, chr_y - CHR_FLOOR + CHR_FLOOR_HEIGHT, 1);
    player->set_bg_priority(2);
}

void ball::update()
{
    if (rolling)
    {
        chr_vx -= bn::fixed(0.01);
        if (chr_vx < bn::fixed(.0))
            chr_vx = bn::fixed(.0);
    }

    chr_x = chr_x + bn::fixed(dir).multiplication(chr_vx);
    if (chr_x < 0)    chr_x = 0;
    if (chr_x > 1791) chr_x = 1791;

    if (rolling)
    {
        chr_vy += GRAVITY;
        if (chr_vy > MAX_FALL)
            chr_vy = MAX_FALL;

        if (chr_y > CHR_FLOOR)
        {
            chr_vy = chr_vy.multiplication(bn::fixed(-0.7));
            chr_vx = chr_vx.multiplication(bn::fixed(0.9));
            chr_y  = CHR_FLOOR;
            if (bn::abs(chr_vy) < bn::fixed(1.0))
                chr_vy = 0;
        }
        chr_y += chr_vy;
    }

    if (chr_vx == 0 && chr_vy == 0)
        rolling = false;

    player->set_x(chr_x - HALF_SCREEN_W);
    player->set_y(chr_y - CHR_FLOOR + CHR_FLOOR_HEIGHT + 12);

    shadow->set_x(chr_x - HALF_SCREEN_W - (4 - valore.multiplication(4).division(10)));
    shadow->set_y(48 + 13);
    shadowRed->set_x(chr_x - HALF_SCREEN_W);
    shadowRed->set_y(48 + 13);

    shadow->set_horizontal_scale(bn::fixed(0.01) + valore.division(10));

    player->set_horizontal_flip(dir == DIR_LEFT);

    if (rolling)
    {
        angle += bn::fixed(chr_vx);
        player->set_rotation_angle_safe(angle);
    }
}
