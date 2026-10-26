#include "globals.h"
#include "bn_core.h"
#include "bn_colors.h"
#include "bn_bg_palettes.h"
#include "bn_sprite_palette_ptr.h"
#include "bn_vector.h"
#include "bau.h"
#include "enemy.h"
#include "item.h"
#include "cassa.h"
#include "dog.h"
#include "boss.h"
#include "scala.h"
#include "piattaforma.h"
#include "game_timer.h"


uint16_t g_powerup_pending = 0;

bn::random g_rng;
bn::optional<bn::camera_ptr> g_camera;

bn::optional<bau> g_bau;
bn::optional<bau> g_bau_carico;
bn::optional<dog> g_dog;
bn::optional<boss> g_boss;
bn::optional<bn::vector<enemy*, MAX_ENEMIES>> g_enemies;
bn::optional<bn::vector<cassa*, MAX_CASSE>> g_casse;
bn::optional<bn::vector<item*, MAX_ENEMIES>> g_items;
bn::optional<game_timer> g_timer;
bn::optional<bn::vector<piattaforma*, MAX_PIATTAFORME>> g_piattaforme;
bn::optional<bn::vector<lava_zona*, MAX_LAVA>> g_lava;
bn::optional<bn::vector<piattaforma_mobile*, MAX_PIATTAFORME_MOBILI>> g_piattaforme_mobili;

int g_schema = 1;

bn::fixed cap(bn::fixed val, bn::fixed max) {
    if (val > max) val = max;
    if (val < -max) val = -max;
    return val;
}
bn::fixed capzero(bn::fixed val, bn::fixed max) {
    if (val > max) val = max;
    if (val < 0) val = bn::fixed(0);
    return val;
}
int capzero(int val, int max) {
    if (val > max) val = max;
    if (val < 0) val = 0;
    return val;
}

int g_shake_ticks = 0;
int g_shake_durata = 1;      // mai 0: serve da divisore
int g_shake_ampiezza = 0;

void screen_shake(int ticks, int ampiezza)
{
    g_shake_ticks = g_shake_durata = ticks;
    g_shake_ampiezza = ampiezza;
}

#include "healthbar.h"
bn::optional<healthbar> g_healthbar;



// Dissolvenza verso il nero (true) o dal nero (false).
 // Usa il fade delle palette: agisce su tutti gli sprite e gli sfondi,
 // senza dover abilitare il blending oggetto per oggetto.
void g_fade_screen(bool to_black)
{
    static constexpr int FRAMES = 60;

    for (int i = 1; i <= FRAMES; ++i)
    {

        bn::fixed intensity = to_black ? bn::fixed(i) / FRAMES
            : bn::fixed(FRAMES - i) / FRAMES;

        bn::bg_palettes::set_fade(bn::colors::black, intensity);
        bn::sprite_palettes::set_fade(bn::colors::black, intensity);
        bn::core::update();
    }
}

#include "bn_sprite_items_weapons.h"
#include "bn_sprite_items_enemies.h"
#include "bn_sprite_items_enemies2.h"
#include "bn_sprite_items_items.h"
#include "bn_sprite_items_doors.h"
#include "bn_sprite_items_enemies2.h"
#include "bn_sprite_items_fox1632.h"

template<typename Step>
void fade_screen_except_dog(bool to_black, bn::sprite_text_generator& text_generator, Step&& step)
{
    static constexpr int FRAMES = 120;

    // Una palette_ptr per ogni palette sprite da dissolvere (create_palette riusa quelle già in VRAM).
    // La palette di fox1632 (dog) NON è in lista.
    bn::vector<bn::sprite_palette_ptr, 10> palettes;
    palettes.push_back(bn::sprite_items::enemies.palette_item().create_palette());
    palettes.push_back(bn::sprite_items::enemies2.palette_item().create_palette());
    palettes.push_back(bn::sprite_items::weapons.palette_item().create_palette());
    palettes.push_back(bn::sprite_items::items.palette_item().create_palette());
    palettes.push_back(bn::sprite_items::doors.palette_item().create_palette());

    g_dog->sprite->set_visible(true);
    bn::sprite_animate_action<8> actionStand = bn::create_sprite_animate_action_once(
        *g_dog->sprite, 6, bn::sprite_items::fox1632.tiles_item(), 35, 36, 37, 38, 39, 40, 41);

    for (int i = 1; i <= FRAMES; ++i)
    {
        step();

        bn::fixed intensity = to_black ? bn::fixed(i) / FRAMES
            : bn::fixed(FRAMES - i) / FRAMES;

        bn::bg_palettes::set_fade(bn::colors::black, intensity);   // tutti gli sfondi
        for (bn::sprite_palette_ptr& p : palettes)
            p.set_fade(bn::colors::black, intensity);

        if (!actionStand.done())
            actionStand.update();
        bn::core::update();
    }

    bn::vector<bn::sprite_ptr, 32> txt_sprites;
    text_generator.set_center_alignment();
    text_generator.generate(0, 0, "premi START per ricominciare", txt_sprites);

    bn::sprite_animate_action<8> actionSleep = bn::create_sprite_animate_action_forever(
        *g_dog->sprite, 15, bn::sprite_items::fox1632.tiles_item(), 29, 30, 31, 32, 33, 34);

    while (true) {
        actionSleep.update();
        if (bn::keypad::start_pressed())
            break;
        if (bn::keypad::select_pressed()){
            g_schema = 1;
            break;
        }
        bn::core::update();
    }
}

void fade_screen_except_dog(bool to_black, bn::sprite_text_generator& text_generator)
{
    fade_screen_except_dog(to_black, text_generator, []() {});
}

bn::optional<bn::vector<trampolino*, MAX_TRAMPOLINI>> g_trampolini;
bn::optional<bn::vector<scala*, MAX_SCALE>> g_scale;