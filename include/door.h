#pragma once

#include "bn_optional.h"
#include "bn_sprite_ptr.h"
#include "bn_fixed.h"
#include "game_constants.h"
#include "schemi.h"

#define DOOR_ENTER_HALF_W 12   // tolleranza orizzontale per entrare (px dal centro)
#define DOOR_ENTER_HALF_H 16   // tolleranza verticale (px dal centro)

#define DOOR_FRAME_CLOSED 0    // porta apribile, cane non ancora davanti
#define DOOR_FRAME_OPEN   1    // porta apribile, cane davanti (pronta a passare)
#define DOOR_FRAME_LOCKED 2    // mancano chiavi: adatta l'indice al tuo spritesheet "doors"

class door
{
public:
    bn::optional<bn::sprite_ptr> sprite;
    bn::fixed chr_x;
    bn::fixed chr_y;

    door(const door_info& info);

    // Da chiamare ogni frame. Ritorna true nel frame in cui il cane
    // usa la porta (up premuto, a terra, davanti alla porta, con tutte le chiavi).
    bool update();

    const door_info& info() const { return *_info; }

private:
    const door_info* _info;
    int current_frame = -1; // forza il primo aggiornamento del frame
};