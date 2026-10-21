#include "globals.h"
#include "door.h"
#include "dog.h"
#include "bn_keypad.h"
#include "bn_math.h"
#include "bn_string.h"
#include "bn_sprite_items_doors.h"
#include "bn_log.h"

door::door(const door_def& info) :
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
    bool apribile = g_dog->porta_apribile(_info->id);

    // numero della chiave necessaria: visibile solo con dog davanti a una porta chiusa
    bool mostra_numero = dog_at_door && !apribile;

    if (mostra_numero)
        g_dog->show_nr(bn::to_string<4>(_info->id));

    uint_fast8_t frame;
    if (!apribile)
        frame = DOOR_FRAME_LOCKED;
    else if (dog_at_door)
        frame = DOOR_FRAME_OPEN;
    else
        frame = DOOR_FRAME_CLOSED;

    // cambia il frame solo se diverso da quello corrente, per evitare di aggiornare lo sprite ogni frame
    if (frame != current_frame)
    {
        current_frame = frame;
        sprite->set_tiles(bn::sprite_items::doors.tiles_item(), frame);
    }

    return dog_at_door && apribile && g_dog->onGround && bn::keypad::up_pressed();
}