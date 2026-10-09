#pragma once

#include "bn_optional.h"
#include "bn_sprite_ptr.h"
#include "bn_sprite_animate_actions.h"
#include "bn_fixed.h"
#include "game_constants.h"
#include "obj.h"

class bau;
class dog;

#define ITEM_TIPO_CHIAVE  0
#define ITEM_TIPO_POWERUP 1
#define ITEM_TIPO_ENERGIA 2
#define ITEM_TIPO_SBLOCCO 3

#define POWERUP_CORSA      (1<<0)
#define POWERUP_SALTO      (1<<1)
#define POWERUP_BAU        (1<<2)
#define POWERUP_RESISTENZA (1<<3)
#define POWERUP_DOPPIO_SALTO (1<<4)
#define POWERUP_WALL_JUMP (1<<5)
#define POWERUP_DASH (1<<6)
#define POWERUP_AIR_DASH (1<<7)
#define POWERUP_GROUND_POUND (1<<8)

// Definizione statica di un item posizionato in uno schema
struct item_def
{
    uint8_t tipo;       // ITEM_TIPO_*
    uint16_t attributo; // usato solo per ITEM_TIPO_POWERUP -> POWERUP_*
    int16_t x;
    int16_t y;
};

class item : public live_obj
{
public:

    int      action = 0;
    uint8_t  tipo;
    uint16_t attributo;
    bn::fixed spawn_x, spawn_y;
    bool     raccolto = false;

    explicit item(const item_def& def);

    virtual void update();
    virtual ~item() = default;

    // Applica l'effetto (chiave/powerup/energia) e marca l'item come raccolto.
    void raccogli();
};