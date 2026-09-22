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

    sprite->set_bg_priority(2);
    sprite->set_z_order(10);
    sprite->set_camera(g_camera);
}

bool door::update()
{
    bn::fixed dx = bn::abs(g_dog->chr_x - chr_x);
    bn::fixed dy = bn::abs(g_dog->chr_y - chr_y);

    bool dog_at_door = dx < DOOR_ENTER_HALF_W && dy < DOOR_ENTER_HALF_H;
    bool apribile     = g_dog->porta_apribile();

    int frame;
    if (!apribile)
        frame = DOOR_FRAME_LOCKED;
    else if (dog_at_door)
        frame = DOOR_FRAME_OPEN;
    else
        frame = DOOR_FRAME_CLOSED;

    if (frame != current_frame)
    {
        current_frame = frame;
        sprite->set_tiles(bn::sprite_items::doors.tiles_item(), frame);
    }

    return dog_at_door && apribile && g_dog->onGround && bn::keypad::up_pressed();
}