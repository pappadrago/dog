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
#define ITEM_TIPO_INTERRUTTORE 3

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

    uint8_t  tipo;
    uint16_t attributo;

    bool is_drop = false;   // item lasciato da un nemico
    bool active = true;    // false = slot libero (solo per i drop)
    uint16_t  ttl = 0;
    uint16_t animAngle = 0;
    bn::vector<bn::sprite_ptr, 4> numero_sprites;

    bn::fixed spawn_x, spawn_y;
    explicit item(const item_def& def);

    virtual void update();
    virtual ~item() = default;

    // Applica l'effetto (chiave/powerup/energia) e marca l'item come raccolto.
    void raccogli();
    void drop_at(bn::fixed x, bn::fixed y);
    bool colpito_da_bau(bn::fixed power);
    uint8_t stato = 0;          // solo SBLOCCO: 0 = spento, 1 = attivo
    bool    dog_sopra = false;  // solo SBLOCCO: dog era in contatto nel frame precedente
    void contatto_interruttore(bool hit);   // da chiamare ogni frame con l'esito della collisione
    void commuta_sblocco();
private:
    void update_physics();
    void update_graphics();
    void hide_and_finish();
    void show_nr(bn::string<4> txt);

    void commuta_stato_grafico();
};
