#include "globals.h"
#include "door.h"
#include "dog.h"
#include "bn_keypad.h"
#include "bn_math.h"
#include "bn_sprite_items_doors.h"

door::door(const door_info& info) :
    _info(&info)
{
    chr_x = bn::fixed(info.x);
    chr_y = bn::fixed(info.y);

    sprite = bn::sprite_items::doors.create_sprite(
        chr_x - HALF_SCREEN_W, chr_y - HALF_SCREEN_H, DOOR_FRAME_CLOSED);

    // Stessa priorita' del cane (2) ma z_order maggiore: la porta sta dietro di lui.
    // Se la vedi davanti al cane, inverti il segno dello z_order.
    sprite->set_bg_priority(2);
    sprite->set_z_order(10);
    sprite->set_camera(g_camera);
}

bool door::update()
{
    bn::fixed dx = bn::abs(g_dog->chr_x - chr_x);
    bn::fixed dy = bn::abs(g_dog->chr_y - chr_y);

    bool dog_at_door = dx < DOOR_ENTER_HALF_W && dy < DOOR_ENTER_HALF_H;

    // la porta si apre quando il cane e' davanti
    if (dog_at_door != open)
    {
        open = dog_at_door;
        sprite->set_tiles(bn::sprite_items::doors.tiles_item(),
                          open ? DOOR_FRAME_OPEN : DOOR_FRAME_CLOSED);
    }

    return dog_at_door && g_dog->onGround && bn::keypad::up_pressed();
}
