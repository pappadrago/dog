#include "globals.h"
#include "healthbar.h"
#include "enemy.h"
#include "boss.h"
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

void healthbar::show(const enemy* e)
{
    target = e;
    boss_target = nullptr;
    ticks  = HEALTHBAR_DURATA;
    update();   // posiziona subito, senza aspettare il prossimo frame
}

void healthbar::show(const boss* b)
{
    target = nullptr;
    boss_target = b;
    ticks  = HEALTHBAR_DURATA;
    update();   // posiziona subito, senza aspettare il prossimo frame
}

void healthbar::update()
{
    if (ticks <= 0 || (!target && !boss_target))
        return;

    ticks--;
    if (ticks == 0)
    {
        sprite->set_visible(false);
        target = nullptr;
        return;
    }

    // frame in base alla vita: arrotonda per eccesso, così con life>0 si vede sempre almeno 1 segmento



    int current_life = target ? target->life : boss_target->life;
    int total_life = target ? START_LIFE : START_LIFE;
    if (total_life <= 0)
        total_life = 1;
    int f = (current_life * (HEALTHBAR_FRAMES - 1) + total_life - 1) / total_life;
    if (f < 0) f = 0;
    if (f > HEALTHBAR_FRAMES - 1) f = HEALTHBAR_FRAMES - 1;

    sprite->set_tiles(bn::sprite_items::healthbar.tiles_item(), f);

    // Sopra la testa dell'oggetto colpito.
    bn::fixed target_x = target ? target->chr_x : boss_target->chr_x;
    bn::fixed target_y = target ? target->chr_y : boss_target->chr_y;
    bn::fixed offset_y = boss_target ? boss_target->dimensione / 2 + 6 : bn::fixed(14);
    sprite->set_x(target_x - HALF_SCREEN_W);
    sprite->set_y(target_y - HALF_SCREEN_H - offset_y);

    sprite->set_visible(ticks > HEALTHBAR_FLICK || ticks % 2);
}