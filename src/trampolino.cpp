#include "globals.h"
#include "trampolino.h"
#include "dog.h"
#include "constanti_fisica.h"
#include "bn_keypad.h"
#include "bn_math.h"
#include "bn_sprite_items_items.h"

static constexpr int TILE_TRAMPOLINO_RIPOSO = 13;
static constexpr int TILE_TRAMPOLINO_COMPRESSO = 14;

static constexpr bn::fixed META_LARGHEZZA = 8;   // sprite 16x16

trampolino::trampolino(const trampolino_def& d)
{
    chr_x = bn::fixed(d.x);
    chr_y = bn::fixed(d.y);

    box_dim = bn::fixed(8);   // mezza altezza: i piedi/la testa stanno a chr_y +/- 8
    box_halfdim = bn::fixed(7);   // < 8: apply_map() lo usa anche in verticale e non deve leggere la riga del pavimento

    sprite = bn::sprite_items::items.create_sprite(
        chr_x - HALF_SCREEN_W, chr_y - HALF_SCREEN_H, TILE_TRAMPOLINO_RIPOSO);
    sprite->set_camera(g_camera);
    sprite->set_bg_priority(1);
}

void trampolino::imposta_frame(int tile)
{
    sprite->set_tiles(bn::sprite_items::items.tiles_item(), tile);
}

void trampolino::update()
{
    // Solo dog: i nemici non vengono mai controllati.
    // Le posizioni sono quelle viste da dog::update() in questo frame, prima del movimento del tappeto.
    bn::fixed top = chr_y - box_dim;

    if (_ticks_carica > 0)
    {
        aggiorna_carica(top);
    }
    else
    {
        bn::fixed piedi = g_dog->chr_y + g_dog->box_dim;
        bn::fixed piedi_prima = piedi - g_dog->chr_vy;
        bn::fixed testa = g_dog->chr_y - g_dog->box_dim;
        bn::fixed dx = g_dog->chr_x - chr_x;

        bool sopra_in_x = bn::abs(dx) < META_LARGHEZZA + g_dog->box_halfdim;

        if (g_dog->chr_vy > 0 && sopra_in_x && piedi_prima <= top + 1 && piedi >= top)
            avvia_carica(top);                                                    // atterraggio dall'alto
        else if (sopra_in_x && piedi > top + 1 && testa < chr_y + box_dim)
            spingi(dx);                                                           // contatto laterale
    }

    chr_vx = ZERO;
    apply_gravity();

    sprite->set_x(chr_x - HALF_SCREEN_W);
    sprite->set_y(chr_y - HALF_SCREEN_H);
}

void trampolino::avvia_carica(bn::fixed top)
{
    dog& d = *g_dog;

    _ticks_carica = TRAMPOLINO_RITARDO_TICKS;
    imposta_frame(TILE_TRAMPOLINO_COMPRESSO);

    d.ground_pound_engaged = false;   // il ground pound finisce qui
    tieni_dog(top);                   // profondità 0: dog parte dal bordo

    d.actionAtterraggio->reset();
    d.atterraggio_anim_ticks = ATTERRAGGIO_ANIM_TICKS;
}

void trampolino::aggiorna_carica(bn::fixed top)
{
    dog& d = *g_dog;

    bool anticipa = bn::keypad::b_pressed();
    // dog scagliato via da altro (danno, lava, bomba) o partito in dash: niente rimbalzo
    bool interrotto = (d.chr_vy < 0 && !anticipa) || d.dash_ticks > 0;

    if (interrotto)
    {
        _ticks_carica = 0;
        imposta_frame(TILE_TRAMPOLINO_RIPOSO);

        // dog era affondato: lo riporto sul bordo, altrimenti il contatto laterale
        // lo scambierebbe per una spinta e sposterebbe il tappeto
        d.chr_y = top - d.box_dim;
        d.sprite->set_y(d.chr_y - HALF_SCREEN_H);
        return;
    }

    if (anticipa || --_ticks_carica == 0)
    {
        rimbalza(top);
        return;
    }

    tieni_dog(top);
}
void trampolino::rimbalza(bn::fixed top)
{
    dog& d = *g_dog;

    _ticks_carica = 0;
    imposta_frame(TILE_TRAMPOLINO_RIPOSO);

    d.chr_y = top - d.box_dim;
    d.sprite->set_y(d.chr_y - HALF_SCREEN_H);   // nuova
    d.chr_vy = (bn::keypad::b_held() ? TRAMPOLINO_RIMBALZO_VY_B : TRAMPOLINO_RIMBALZO_VY)
        - d.bonus_salto;

    d.onGround = false;
    d.salto_in_corso = true;       // il ground pound è di nuovo possibile dopo il rimbalzo
    d.salto_pieno = true;
    d.salto_tenuto = false;        // niente taglio del salto al rilascio di B
    d.ground_pound_engaged = false;
    d.coyote_ticks = 0;
    d.jump_buffer_ticks = 0;
    d.doppio_salto_disponibile = d.doppio_salto_acquisito;

    d.atterraggio_anim_ticks = 0;  // se il rimbalzo è anticipato, l'animazione di atterraggio non deve coprire quella di stacco
    d.actionStacco->reset();
    d.stacco_anim_ticks = STACCO_ANIM_TICKS;
}

void trampolino::spingi(bn::fixed dx)
{
    bn::fixed contatto_x = META_LARGHEZZA + g_dog->box_halfdim;
    bn::fixed penetrazione = contatto_x - bn::abs(dx);
    int verso = (dx > 0) ? DIR_LEFT : DIR_RIGHT;   // il tappeto si sposta via da dog

    // apply_map() guarda il lato in base al segno di chr_vx e, se trova un muro,
    // annulla esattamente questo spostamento e azzera chr_vx
    chr_vx = penetrazione * verso;
    chr_x += chr_vx;
    apply_map();
    bool bloccato = (chr_vx == ZERO);
    chr_vx = ZERO;

    if (bloccato)
    {
        // il tappeto è contro un muro: è dog a non poter avanzare
        g_dog->chr_x = chr_x - contatto_x * verso;
        g_dog->chr_vx = ZERO;
    }
    else if (g_dog->chr_vx * verso > ZERO)   // dog sta davvero spingendo, non sta solo sfiorando il tappeto
    {
        // rallenta dog: tetto alla velocità nel verso della spinta
        g_dog->tetto_vx_spinta = TRAMPOLINO_VELOCITA_SPINTA;
        g_dog->spinta_ticks = 1;
    }
}

// Tiene dog fermo sul tappeto, sempre più in basso man mano che l'attesa avanza
void trampolino::tieni_dog(bn::fixed top)
{
    dog& d = *g_dog;

    int trascorsi = TRAMPOLINO_RITARDO_TICKS - _ticks_carica;
    bn::fixed profondita = bn::fixed(TRAMPOLINO_AFFONDAMENTO * trascorsi) / TRAMPOLINO_RITARDO_TICKS;

    d.chr_y = top - d.box_dim + profondita;
    d.chr_vy = ZERO;
    d.chr_vx = ZERO;
    d.onGround = true;

    // dog::update() ha già posizionato lo sprite in questo frame: lo riallineo subito
    d.sprite->set_y(d.chr_y - HALF_SCREEN_H);
}