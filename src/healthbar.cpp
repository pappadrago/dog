#include "globals.h"
#include "healthbar.h"
#include "enemy.h"
#include "bn_sprite_items_healthbar.h"

healthbar::healthbar()
{
    sprite = bn::sprite_items::healthbar.create_sprite(0, 0, 0);
    sprite->set_camera(g_camera);
    sprite->set_bg_priority(1);
    sprite->set_z_order(0);
    sprite->set_visible(false);
}

void healthbar::show(const enemy* e)
{
    target = e;
    ticks  = HEALTHBAR_DURATA;
    update();   // posiziona subito, senza aspettare il prossimo frame
}

void healthbar::update()
{
    if (ticks <= 0 || !target)
        return;

    ticks--;
    if (ticks == 0)
    {
        sprite->set_visible(false);
        target = nullptr;
        return;
    }

    // frame in base alla vita: arrotonda per eccesso, così con life>0 si vede sempre almeno 1 segmento
    int f = (target->life * (HEALTHBAR_FRAMES - 1) + START_LIFE - 1) / START_LIFE;
    if (f < 0) f = 0;
    if (f > HEALTHBAR_FRAMES - 1) f = HEALTHBAR_FRAMES - 1;
    sprite->set_tiles(bn::sprite_items::healthbar.tiles_item(), f);

    // sopra la testa del nemico (sprite 16x16 centrato su chr_x/chr_y)
    sprite->set_x(target->chr_x - HALF_SCREEN_W);
    sprite->set_y(target->chr_y - HALF_SCREEN_H - 14);

    sprite->set_visible(ticks > HEALTHBAR_FLICK || ticks % 2);
}