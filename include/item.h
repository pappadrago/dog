#pragma once

#include "bn_optional.h"
#include "bn_sprite_ptr.h"
#include "bn_sprite_animate_actions.h"
#include "bn_fixed.h"
#include "game_constants.h"
#include "obj.h"

class bau;
class dog;

#define ACTION_MOVING 1
#define ACTION_ATTACK 2

#define ITEM_TIPO_CHIAVE  0
#define ITEM_TIPO_POWERUP 1
#define ITEM_TIPO_ENERGIA 2

#define POWERUP_CORSA      0
#define POWERUP_SALTO      1
#define POWERUP_BAU        2
#define POWERUP_RESISTENZA 3

// Definizione statica di un item posizionato in uno schema.
// Le tabelle per schema stanno in <schema>_items.h (es. s1_items.h)
struct item_def
{
    uint8_t tipo;       // ITEM_TIPO_*
    uint8_t sotto_tipo; // usato solo per ITEM_TIPO_POWERUP -> POWERUP_*
    int16_t x;
    int16_t y;
};

class item : public live_obj
{
public:

    int      action = 0;
    uint8_t  tipo;
    uint8_t  sotto_tipo;
    bn::fixed spawn_x, spawn_y;
    bool     raccolto = false;

    explicit item(const item_def& def);

    void do_spawn();
    virtual void update();
    virtual ~item() = default;

    // Applica l'effetto (chiave/powerup/energia) e marca l'item come raccolto.
    void raccogli();
};