#include "bau.h"
#include "game_context.h"
#include "bn_sprite_items_bau.h"

bau::bau(GameContext* _ctx) : ctx(_ctx)
{
    sprite = bn::sprite_items::bau.create_sprite(0, 0, 0);
    sprite->set_visible(false);
}

void bau::update()
{
    if (ticks > 0) ticks--;

    if (dim < bn::fixed(1.0))
        dim += bn::fixed(0.1);
    else
        dim = bn::fixed(1.0);

    sprite->set_x(chr_x - HALF_SCREEN_W);
    sprite->set_y(chr_y - CHR_FLOOR + CHR_FLOOR_HEIGHT);
    sprite->set_scale(dim);
    sprite->set_visible(ticks > 60 || ticks % 2);
}
