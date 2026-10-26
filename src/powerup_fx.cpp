#include "powerup_fx.h"
#include "globals.h"
#include "dog.h"
#include "bn_core.h"
#include "bn_colors.h"
#include "bn_bg_palettes.h"
#include "bn_vector.h"
#include "bn_optional.h"
#include "bn_sprite_ptr.h"
#include "bn_sprite_palette_ptr.h"
// nuovo asset (vedi sotto)
#include "bn_sprite_items_enemies.h"
#include "bn_sprite_items_enemies2.h"
#include "bn_sprite_items_weapons.h"
#include "bn_sprite_items_lavabubble.h"
#include "bn_sprite_items_items.h"
#include "bn_sprite_items_doors.h"
#include "bn_sprite_items_diecloud.h"
#include "bn_sprite_items_healthbar.h"
#include "bn_sprite_items_zombie.h"
#include "item.h"
#include <bn_log.h>

namespace
{
    constexpr int FX_FRAMES = 23;    // frame dello spritesheet powerup_fx
    constexpr int FX_TICKS = 5;    // update per frame di animazione
    constexpr int HOLD_FRAMES = 30;   // pausa dopo l'ultimo frame
    constexpr int FADE_IN = 20;
    constexpr int FADE_OUT = 20;
    constexpr int TOTAL_FRAMES = FX_FRAMES * FX_TICKS + HOLD_FRAMES;
    constexpr bn::fixed MAX_DARK = bn::fixed(0.75);   // 1.0 = nero totale
}

void powerup_freeze(bn::sprite_text_generator& text_generator)
{
    // Palette da scurire. NON ci sono: fox1632 (dog) e powerup_fx.
    // create_palette_optional riusa le palette già in VRAM.
    bn::vector<bn::sprite_palette_ptr, 10> palettes;
    auto add = [&](auto&& opt)
        {
            if (opt.has_value())
                palettes.push_back(*opt);
        };
    add(bn::sprite_items::enemies.palette_item().create_palette_optional());
    add(bn::sprite_items::enemies2.palette_item().create_palette_optional());
    add(bn::sprite_items::weapons.palette_item().create_palette_optional());
    add(bn::sprite_items::items.palette_item().create_palette_optional());
    add(bn::sprite_items::doors.palette_item().create_palette_optional());
    add(bn::sprite_items::diecloud.palette_item().create_palette_optional());
    add(bn::sprite_items::healthbar.palette_item().create_palette_optional());
    if (g_boss.has_value())
        add(bn::sprite_items::zombie.palette_item().create_palette_optional());

    // Il cane deve restare ben visibile (niente lampeggio da invulnerabilità/carica)
    g_dog->sprite->set_visible(true);

    // Sprite dell'animazione, centrato sul cane (coordinate mondo + camera, come gli altri sprite)
    bn::sprite_ptr fx = bn::sprite_items::lavabubble.create_sprite(
        g_dog->chr_x - HALF_SCREEN_W, g_dog->chr_y - HALF_SCREEN_H, 0);
    fx.set_camera(g_camera);
    fx.set_bg_priority(0);
    fx.set_z_order(-16);

    bn::vector<bn::sprite_ptr, 16> txt;
    text_generator.set_center_alignment();

    if (g_powerup_pending & POWERUP_CORSA)
        text_generator.generate(0, -56, "CORRI PREMENDO IL TASTO L", txt);
    else
        text_generator.generate(0, -56, "POWER UP!", txt);
    text_generator.set_left_alignment();

    for (int f = 0; f < TOTAL_FRAMES; ++f)
    {
        bn::fixed intensity = MAX_DARK;
        if (f < FADE_IN)
            intensity = MAX_DARK * f / FADE_IN;
        else if (f >= TOTAL_FRAMES - FADE_OUT)
            intensity = MAX_DARK * (TOTAL_FRAMES - f) / FADE_OUT;

        bn::bg_palettes::set_fade(bn::colors::black, intensity);   // tutti gli sfondi
        for (bn::sprite_palette_ptr& p : palettes)
            p.set_fade(bn::colors::black, intensity);

        int frame = f / FX_TICKS;
        if (frame > FX_FRAMES - 1)
            frame = FX_FRAMES - 1;
        fx.set_tiles(bn::sprite_items::lavabubble.tiles_item(), frame);

        bn::core::update();   // la musica continua, il gioco no
    }

    // Ripristino
    bn::bg_palettes::set_fade_intensity(0);
    for (bn::sprite_palette_ptr& p : palettes)
        p.set_fade_intensity(0);
}