#include "globals.h"
#include "item.h"
#include "bau.h"
#include "dog.h"
#include "bn_sprite_items_items.h"
#include "bn_math.h"
#include "bn_log.h"
#include "piattaforma.h"
#include "piattaforma_mobile.h"
#include "hud.h"
#include "bn_string.h"
#include <bn_sound_items.h>

// Indici tile per tipo di oggetto: adatta agli indici reali dello spritesheet "items"
static constexpr int TILE_CHIAVE = 31;
static constexpr int TILE_ENERGIA = 11;

static constexpr int TILE_SBLOCCO = 15;
static constexpr int TILE_SBLOCCO_ATTIVO = 17;   // adatta all'indice reale nello spritesheet "items"

static constexpr int TILE_POWERUP_BASE = 0; // + sotto_tipo (4 varianti consecutive)

static int tile_per_item(uint8_t tipo)
{
    switch (tipo)
    {
    case ITEM_TIPO_CHIAVE:  return TILE_CHIAVE;
    case ITEM_TIPO_ENERGIA: return TILE_ENERGIA;
    case ITEM_TIPO_POWERUP: return TILE_POWERUP_BASE;
    case ITEM_TIPO_INTERRUTTORE: return TILE_SBLOCCO; // Assuming a tile index for the lock item
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

    ticks2action = 0;
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

void item::update_physics() {
    if (ticks2action > 0) {
        ticks2action--;
        if (ticks2action == 0)
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
    numero_sprites.clear();
    sprite->remove_affine_mat();
    sprite->set_visible(false);
    active = false;
    ttl = 0;
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
    bn::fixed dy = 0;
    bn::fixed dx = 0;
    if (tipo != ITEM_TIPO_INTERRUTTORE) {
        dy = bn::degrees_lut_sin_safe(animAngle << 3) * 4 - 2;
        dx = bn::degrees_lut_cos_safe(animAngle << 2) * 6 - 2;
    }
    sprite->set_x(chr_x + dx - HALF_SCREEN_W);
    sprite->set_y(chr_y + dy - HALF_SCREEN_H);

    if (is_drop)
    {
        sprite->set_visible(ttl > DROP_BLINK_TIME || ttl % 4 < 2);
    }
    else {
        sprite->set_visible(ticks2action > 0 ? (ticks2action % 2) : true);
    }
    if (ticks2action > 0)
        sprite->set_scale(bn::fixed(2) - bn::fixed(ticks2action).division(30));

    // il numero sale piano; lampeggia solo negli ultimi frame, per restare leggibile
    for (bn::sprite_ptr& s : numero_sprites)
    {
        s.set_y(s.y() - bn::fixed(0.5));
        s.set_visible(ticks2action > 10 || ticks2action % 2);
    }
}

void item::show_nr(bn::string<4> txt) {
    if (bn::sprite_text_generator* gen = hud_text_generator())
    {
        numero_sprites.clear();
        gen->set_center_alignment();
        gen->generate(chr_x - HALF_SCREEN_W, chr_y - HALF_SCREEN_H - 14,
            txt, numero_sprites);
        gen->set_left_alignment();
        for (bn::sprite_ptr& s : numero_sprites){
            s.set_camera(g_camera);   // il testo segue la camera come gli altri sprite del mondo
            s.set_visible(true);
        }
    }
}

void item::raccogli()
{
    // TIPO_SBLOCCO non passa da raccogli() ma va direttamente in contatto_interruttore()
    if (!active) return;

    chr_vy = MEDIUM_JUMP_VY;
    ticks2action = 30;

    switch (tipo)
    {
    case ITEM_TIPO_CHIAVE:
        g_dog->chiavi_raccolte++;
        g_dog->segna_chiave(attributo);
        show_nr(bn::to_string<4>(attributo & 0xFF));
        bn::sound_items::creaky_door_short.play();
        break;

    case ITEM_TIPO_ENERGIA:
        g_dog->life += 30; // valore da bilanciare
        g_dog->life = capzero(g_dog->life, MAX_LIFE);
        show_nr("+" + bn::to_string<3>(30));
        bn::sound_items::heart_collect.play();
        break;

    case ITEM_TIPO_POWERUP:
        g_dog->applica_powerup(attributo);
        bn::sound_items::gem_collect.play();
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
    ticks2action = 0;
    sprite->set_tiles(bn::sprite_items::items.tiles_item(), g_rng.get_int(10));
    sprite->set_position(x - HALF_SCREEN_W, y - HALF_SCREEN_H);
    sprite->set_visible(true);
}


void item::commuta_sblocco()
{
    // --- qui va la tua logica di commutazione delle piattaforme collegate ---
    for (piattaforma* p : *g_piattaforme)
        if (p->def.attiva_con == attributo && !p->in_transizione) {
            p->commuta();
            commuta_stato_grafico();
        }

    for (piattaforma_mobile* pm : *g_piattaforme_mobili)
        if (pm->def.attiva_con == attributo && !pm->attiva) {
            pm->attiva_piattaforma();
            commuta_stato_grafico();
        }
}

void item::commuta_stato_grafico()
{
    bn::sound_items::lock_unlock.play();
    stato ^= 1;
    g_dog->show_str("CLICK");
    sprite->set_tiles(bn::sprite_items::items.tiles_item(),
        stato ? TILE_SBLOCCO_ATTIVO : TILE_SBLOCCO);
}

void item::contatto_interruttore(bool hit)
{
    if (hit && !dog_sopra)    {  // solo sul fronte di salita del contatto
        commuta_sblocco();
    }
    dog_sopra = hit;
}