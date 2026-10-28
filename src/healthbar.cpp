#include "globals.h"
#include "healthbar.h"
#include "enemy.h"
#include "boss.h"
#include "obj.h"
#include "bn_sprite_items_healthbar.h"
#include "bn_log.h"

healthbar::healthbar()
{
    sprite = bn::sprite_items::healthbar.create_sprite(0, 0, 0);
    sprite->set_camera(g_camera);
    sprite->set_bg_priority(1);
    sprite->set_z_order(0);
    sprite->set_visible(false);
}

void healthbar::show(const live_obj* b)
{
    obj_target = b;
    ticks  = HEALTHBAR_DURATA;
    update();   // posiziona subito, senza aspettare il prossimo frame
}

void healthbar::update()
{
    if (ticks <= 0 || (!obj_target))
        return;

    ticks--;
    if (ticks == 0)
    {
        sprite->set_visible(false);
        obj_target = nullptr;
        return;
    }

    // frame in base alla vita: arrotonda per eccesso, così con life>0 si vede sempre almeno 1 segmento
    int current_life = obj_target->life;
    int total_life = START_LIFE;
    if (total_life <= 0)
        total_life = 1;
    int f = (current_life * (HEALTHBAR_FRAMES - 1) + total_life - 1) / total_life;
    if (f < 0) f = 0;
    if (f > HEALTHBAR_FRAMES - 1) f = HEALTHBAR_FRAMES - 1;

    sprite->set_tiles(bn::sprite_items::healthbar.tiles_item(), f);

    // Sopra la testa dell'oggetto colpito.
    bn::fixed target_x =  obj_target->chr_x;
    bn::fixed target_y =  obj_target->chr_y;
    bn::fixed offset_y = obj_target ? obj_target->box_dim + 6 : bn::fixed(14);
    sprite->set_x(target_x - HALF_SCREEN_W);
    sprite->set_y(target_y - HALF_SCREEN_H - offset_y);

    sprite->set_visible(ticks > HEALTHBAR_FLICK || ticks % 2);
}