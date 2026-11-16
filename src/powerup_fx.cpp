#include "powerup_fx.h"
#include "globals.h"
#include "dog.h"
#include "enemy.h"
#include "schemi.h"
#include "bn_core.h"
#include "bn_colors.h"
#include "bn_bg_palettes.h"
#include "bn_format.h"
#include "bn_vector.h"
#include "bn_string.h"
#include "bn_optional.h"
#include "bn_sprite_ptr.h"
#include "bn_sprite_palette_ptr.h"
#include "bn_sprite_palette_item.h"
#include "bn_sprite_tiles_ptr.h"
#include "bn_sprite_shape_size.h"
#include "bn_bpp_mode.h"
#include "bn_keypad.h"
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
#include "bn_sprite_items_windgust.h"
#include "bn_sprite_items_snake.h"
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

static bn::vector<bn::sprite_palette_ptr, 16> getPalettes() {
    bn::vector<bn::sprite_palette_ptr, 16> palettes;
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
    add(bn::sprite_items::windgust.palette_item().create_palette_optional());
    add(bn::sprite_items::snake.palette_item().create_palette_optional());
    if (g_boss.has_value())
        add(bn::sprite_items::zombie.palette_item().create_palette_optional());

    return palettes;
}

void powerup_freeze(bn::sprite_text_generator& text_generator)
{
    // Palette da scurire. NON ci sono: fox1632 (dog) e powerup_fx.
    // create_palette_optional riusa le palette già in VRAM.
    bn::vector<bn::sprite_palette_ptr, 16> palettes = getPalettes();
    // Il cane deve restare ben visibile (niente lampeggio da invulnerabilità/carica)
    g_dog->sprite->set_visible(true);

    // Sprite dell'animazione, centrato sul cane (coordinate mondo + camera, come gli altri sprite)
    bn::sprite_ptr fx = bn::sprite_items::lavabubble.create_sprite(
        g_dog->chr_x - HALF_SCREEN_W, g_dog->chr_y - HALF_SCREEN_H, 0);
    fx.set_camera(g_camera);
    fx.set_bg_priority(0);
    fx.set_z_order(-16);

    bn::vector<bn::sprite_ptr, 32> txt;
    bn::vector<const char*, 9> descriptions;
    struct powerup_description
    {
        uint16_t flag;
        const char* text;
    };
    constexpr powerup_description powerup_descriptions[] = {
        { POWERUP_CORSA, "CORSA: TIENI PREMUTO L" },
        { POWERUP_SALTO, "SALTO: SALTA PIU' ALTO" },
        { POWERUP_BAU, "BAU: COLPI PIU' FORTI" },
        { POWERUP_RESISTENZA, "RESISTENZA: +30 VITA" },
        { POWERUP_DOPPIO_SALTO, "DOPPIO SALTO: PREMI B" },
        { POWERUP_WALL_JUMP, "SALTO A MURO: PREMI B" },
        { POWERUP_DASH, "DASH: PREMI R A TERRA" },
        { POWERUP_AIR_DASH, "AIR DASH: PREMI R IN ARIA" },
        { POWERUP_GROUND_POUND, "GROUND POUND: GIU' IN ARIA" },
    };
    for (const powerup_description& description : powerup_descriptions)
    {
        if (g_powerup_pending & description.flag)
            descriptions.push_back(description.text);
    }
    if (descriptions.empty())
        descriptions.push_back("POWER UP!");

    text_generator.set_center_alignment();
    int description_frames = TOTAL_FRAMES / descriptions.size();
    int shown_description = -1;
    text_generator.set_left_alignment();

    for (int f = 0; f < TOTAL_FRAMES; ++f)
    {
        int description_index = f / description_frames;
        if (description_index >= descriptions.size())
            description_index = descriptions.size() - 1;
        if (description_index != shown_description)
        {
            txt.clear();
            text_generator.set_center_alignment();
            text_generator.generate(0, -56, descriptions[description_index], txt);
            text_generator.set_left_alignment();
            shown_description = description_index;
        }

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

void pause_game(bn::sprite_text_generator& text_generator)
{
    bn::vector<bn::sprite_palette_ptr, 16> palettes = getPalettes();
    bn::fixed intensity = bn::fixed(0.55);   // 1.0 = nero totale
    bn::bg_palettes::set_fade(bn::colors::black, intensity);   // tutti gli sfondi
    for (bn::sprite_palette_ptr& p : palettes)
        p.set_fade(bn::colors::black, intensity);

    bn::vector<bn::sprite_ptr, 24> txt;
    text_generator.set_center_alignment();
    text_generator.generate(0, -20, bn::format<32>("IN PAUSA - SCHEMA {}", g_schema), txt);
    text_generator.set_left_alignment();

    constexpr int MAP_WIDTH = 192;
    constexpr int MAP_HEIGHT = 32;
    constexpr int SPRITE_WIDTH = 64;
    constexpr int TILES_PER_SPRITE = 32;
    const collision_map_info& collision = get_collision_map(g_schema);
    auto map_coordinate = [](int coordinate, int map_size, int cell_count)
    {
        int cell = coordinate * cell_count / map_size;
        if (cell < 0)
            return 0;
        if (cell >= cell_count)
            return cell_count - 1;
        return cell;
    };

    int dog_x = map_coordinate(int(g_dog->chr_x), collision.map_w, MAP_WIDTH);
    int dog_y = map_coordinate(int(g_dog->chr_y), collision.map_h, MAP_HEIGHT);

    int door_count = 0;
    const door_def* doors = get_schema_doors(g_schema, door_count);
    int exit_x = -1;
    int exit_y = -1;
    if (door_count > 0)
    {
        const door_def& exit = doors[door_count - 1];
        exit_x = map_coordinate(exit.x, collision.map_w, MAP_WIDTH);
        exit_y = map_coordinate(exit.y, collision.map_h, MAP_HEIGHT);
    }

    auto map_x = [&](bn::fixed x)
    {
        return map_coordinate(x.integer(), collision.map_w, MAP_WIDTH);
    };
    auto map_y = [&](bn::fixed y)
    {
        return map_coordinate(y.integer(), collision.map_h, MAP_HEIGHT);
    };

    constexpr bn::color minimap_colors[16] = {
        bn::colors::black,  // trasparente
        bn::color(0x2108),
        bn::color(0x6318),
        bn::colors::white,   // cane
        bn::colors::green,  // uscita
        bn::colors::red,    // nemici
        bn::color(0x03FF),  // chiavi
        bn::color(0x021F),  // powerup
        bn::colors::black,
        bn::colors::black,
        bn::colors::black,
        bn::colors::black,
        bn::colors::black,
        bn::colors::black,
        bn::colors::black,
        bn::colors::black
    };
    const bn::sprite_palette_item minimap_palette_item(
        bn::span<const bn::color>(minimap_colors), bn::bpp_mode::BPP_4);
    bn::sprite_palette_ptr minimap_palette = minimap_palette_item.create_palette();
    bn::vector<bn::sprite_ptr, 3> minimap_sprites;

    for (int chunk = 0; chunk < MAP_WIDTH / SPRITE_WIDTH; chunk++)
    {
        bn::sprite_tiles_ptr tiles = bn::sprite_tiles_ptr::allocate(TILES_PER_SPRITE, bn::bpp_mode::BPP_4);
        bn::span<bn::tile> tile_data = tiles.vram().value();
        for (bn::tile& tile : tile_data)
            for (uint32_t& row : tile.data)
                row = 0;

        for (int y = 0; y < MAP_HEIGHT; y++)
        {
            for (int local_x = 0; local_x < SPRITE_WIDTH; local_x++)
            {
                int x = chunk * SPRITE_WIDTH + local_x;
                int tile_x = x * collision.columns / MAP_WIDTH;
                int tile_y = y * collision.rows / MAP_HEIGHT;
                int color_index = collision.data[tile_y * collision.columns + tile_x] ? 2 : 1;
                if (x == dog_x && y == dog_y)
                    color_index = 3;
                if (x == exit_x && y == exit_y)
                    color_index = 4;

                int tile_index = (y / 8) * (SPRITE_WIDTH / 8) + local_x / 8;
                int tile_row = y % 8;
                int shift = (local_x % 8) * 4;
                tile_data[tile_index].data[tile_row] |= uint32_t(color_index) << shift;
            }
        }

        auto draw_cross = [&](int x, int y, int color_index)
        {
            for (int dy = -1; dy <= 1; dy++)
            {
                for (int dx = -1; dx <= 1; dx++)
                {
                    if (dx != 0 && dy != 0)
                        continue;

                    int pixel_x = x + dx;
                    int pixel_y = y + dy;
                    int local_x = pixel_x - chunk * SPRITE_WIDTH;
                    if (pixel_x < chunk * SPRITE_WIDTH || local_x >= SPRITE_WIDTH ||
                        pixel_y < 0 || pixel_y >= MAP_HEIGHT)
                        continue;

                    int tile_index = (pixel_y / 8) * (SPRITE_WIDTH / 8) + local_x / 8;
                    int tile_row = pixel_y % 8;
                    int shift = (local_x % 8) * 4;
                    uint32_t mask = 0xF << shift;
                    uint32_t& row = tile_data[tile_index].data[tile_row];
                    row = (row & ~mask) | (uint32_t(color_index) << shift);
                }
            }
        };

        if (exit_x >= 0)
            draw_cross(exit_x, exit_y, 4);

        if (g_items.has_value())
        {
            for (item* map_item : *g_items)
            {
                if (!map_item->active || map_item->ticks2action != 0)
                    continue;

                int color_index = map_item->tipo == ITEM_TIPO_CHIAVE ? 6 :
                    map_item->tipo == ITEM_TIPO_POWERUP ? 7 : 0;
                if (color_index != 0)
                    draw_cross(map_x(map_item->chr_x), map_y(map_item->chr_y), color_index);
            }
        }

        if (g_enemies.has_value())
        {
            for (enemy* map_enemy : *g_enemies)
            {
                if (!map_enemy->is_dead() && !map_enemy->distrutto)
                    draw_cross(map_x(map_enemy->chr_x), map_y(map_enemy->chr_y), 5);
            }
        }

        if (g_boss.has_value() && !g_boss->distrutto)
            draw_cross(map_x(g_boss->chr_x), map_y(g_boss->chr_y), 5);

        draw_cross(dog_x, dog_y, 3);

        bn::sprite_ptr minimap = bn::sprite_ptr::create(
            bn::fixed(-64 + chunk * SPRITE_WIDTH), bn::fixed(48),
            bn::sprite_shape_size(SPRITE_WIDTH, MAP_HEIGHT),
            tiles, minimap_palette);
        minimap.set_bg_priority(0);
        minimap.set_z_order(-64);
        minimap_sprites.push_back(bn::move(minimap));
    }

    while (bn::keypad::start_held())
        bn::core::update();

    while (!bn::keypad::start_pressed())
        bn::core::update();

    bn::bg_palettes::set_fade_intensity(0);
    for (bn::sprite_palette_ptr& p : palettes)
        p.set_fade_intensity(0);

    txt.clear();
    minimap_sprites.clear();
}