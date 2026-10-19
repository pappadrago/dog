#pragma once

#include "bn_optional.h"
#include "bn_sprite_ptr.h"
#include "bn_fixed.h"
#include "game_constants.h"
#include "bn_vector.h"

#define DOOR_ENTER_HALF_W 12   // tolleranza orizzontale per entrare (px dal centro)
#define DOOR_ENTER_HALF_H 16   // tolleranza verticale (px dal centro)

#define DOOR_FRAME_CLOSED 0    // porta apribile, cane non ancora davanti
#define DOOR_FRAME_OPEN   1    // porta apribile, cane davanti (pronta a passare)
#define DOOR_FRAME_LOCKED 2    // mancano chiavi: adatta l'indice al tuo spritesheet "doors"

// Una porta: dove sta, dove porta, ID di apertura
struct door_def
{
    int x, y;          // centro della porta (px, coordinate mondo)
    int dest_schema;   // schema di destinazione
    int dest_door;     // indice della porta di arrivo nello schema di destinazione
    uint8_t id;        // ID della porta (0..255): si apre con la chiave che ha lo stesso ID
};

class door
{
public:
    bn::optional<bn::sprite_ptr> sprite;
    bn::fixed chr_x;
    bn::fixed chr_y;

    door(const door_def& info);

    // Da chiamare ogni frame. Ritorna true nel frame in cui il cane
    // usa la porta (up premuto, a terra, davanti alla porta, con tutte le chiavi).
    bool update();

    const door_def& info() const { return *_info; }
    bn::vector<bn::sprite_ptr, 4> testo_sprites;   // numero della chiave richiesta, mostrato se la porta è chiusa e dog è davanti
    void libera_testo() { testo_sprites.clear(); }   // rilascia subito gli sprite del numero
private:
    const door_def* _info;
    int current_frame = -1; // forza il primo aggiornamento del frame
};

