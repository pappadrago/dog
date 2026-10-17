#include "globals.h"
#include "item.h"
#include "bau.h"
#include "dog.h"
#include "bn_sprite_items_items.h"
#include "bn_math.h"
#include "bn_log.h"
#include "piattaforma.h"
#include "piattaforma_mobile.h"

// Indici tile per tipo di oggetto: adatta agli indici reali dello spritesheet "items"
static constexpr int TILE_CHIAVE = 31;
static constexpr int TILE_ENERGIA = 11;
static constexpr int TILE_SBLOCCO = 30;
static constexpr int TILE_POWERUP_BASE = 0; // + sotto_tipo (4 varianti consecutive)

static int tile_per_item(uint8_t tipo)
{
    switch (tipo)
    {
    case ITEM_TIPO_CHIAVE:  return TILE_CHIAVE;
    case ITEM_TIPO_ENERGIA: return TILE_ENERGIA;
    case ITEM_TIPO_POWERUP: return TILE_POWERUP_BASE;
    case ITEM_TIPO_SBLOCCO: return TILE_SBLOCCO; // Assuming a tile index for the lock item
    default:                return 0;
    }
}

item::item(const item_def& def)
{
    tipo = def.tipo;
    attributo = def.attributo;

    chr_x = bn::fixed(def.x);
    chr_y = bn::fixed(def.y);

    sprite = bn::sprite_items::items.create_sprite(chr_x - HALF_SCREEN_W, chr_y - HALF_SCREEN_H, tile_per_item(tipo));
    sprite->set_bg_priority(1);
    sprite->set_camera(g_camera);

    box_dim = bn::fixed(8);
    box_halfdim = bn::fixed(4);

    ticks2animation = 0;
    active = true;
    animAngle = g_rng.get_int(100);

}

void item::update()
{
    if (!active)
        return;
    update_physics();
    update_graphics();
}

<<<<<<< HEAD
void item::update_physics(){
 if (ticks2animation > 0) {
        ticks2animation--;
        if (ticks2animation == 0)
=======
void item::update_physics() {
    if (ticks2action > 0) {
        ticks2action--;
        if (ticks2action == 0)
>>>>>>> 8541bfc (bug fix)
        {
            hide_and_finish();
            return;
        }
    }
    if (is_drop)
    {
        if (--ttl <= 0)
        {
            hide_and_finish();
            return;
        }
    }
    apply_gravity();
}

void item::hide_and_finish()
{
    sprite->remove_affine_mat();
    sprite->set_visible(false);
    active = false;
<<<<<<< HEAD
    ttl=0;
=======
    ttl = 0;
>>>>>>> 8541bfc (bug fix)
}

void item::update_graphics() {
    if (!active) return;
    static constexpr bn::fixed ITEM_ACTIVATION_RANGE_X = 240;
    if (bn::abs(chr_x - g_dog->chr_x) > ITEM_ACTIVATION_RANGE_X)
    {
        sprite->set_visible(false);   // fuori portata: non lasciarlo fermo a schermo
        return;
    }
    
    animAngle++;
    bn::fixed dy = bn::degrees_lut_sin_safe(animAngle << 3) * 4 - 2;
    bn::fixed dx = bn::degrees_lut_cos_safe(animAngle << 2) * 6 - 2;
    sprite->set_x(chr_x + dx - HALF_SCREEN_W);
    sprite->set_y(chr_y + dy - HALF_SCREEN_H);

    if (is_drop)
    {
        sprite->set_visible(ttl > DROP_BLINK_TIME || ttl % 4 < 2);
    }
    else {
<<<<<<< HEAD
        sprite->set_visible(ticks2animation > 0 ? (ticks2animation % 2) : true);
=======
        sprite->set_visible(ticks2action > 0 ? (ticks2action % 2) : true);
>>>>>>> 8541bfc (bug fix)
    }
    if (ticks2animation > 0)
        sprite->set_scale(bn::fixed(2) - bn::fixed(ticks2animation).division(30));
}

void item::raccogli()
{
    if (!active) return;
    g_dog->add_score(10);
    chr_vy = MEDIUM_JUMP_VY;
    ticks2animation = 30;
    switch (tipo)
    {
    case ITEM_TIPO_CHIAVE:
        g_dog->chiavi_raccolte++;
        break;

    case ITEM_TIPO_ENERGIA:
        g_dog->life += 30; // valore da bilanciare
        g_dog->life = capzero(g_dog->life, MAX_LIFE);
        break;

    case ITEM_TIPO_POWERUP:
        g_dog->applica_powerup(attributo);
        break;
    case ITEM_TIPO_SBLOCCO:
        for (piattaforma* p : *g_piattaforme)
            if (p->def.attiva_con == attributo)
                p->commuta();

        for (piattaforma_mobile* pm : *g_piattaforme_mobili)
            if (pm->def.attiva_con == attributo)
                pm->attiva_piattaforma();
        break;
    default:
        break;
    }
}


void item::drop_at(bn::fixed x, bn::fixed y)
{
    is_drop = true;
    active = true;
    ttl = DROP_LIFETIME;
    chr_x = x;
    chr_y = y;
    chr_vx = ZERO;
    chr_vy = BIG_JUMP_VY;
    ticks2animation = 0;
    sprite->set_tiles(bn::sprite_items::items.tiles_item(), g_rng.get_int(10));
    sprite->set_position(x - HALF_SCREEN_W, y - HALF_SCREEN_H);
    sprite->set_visible(true);
}