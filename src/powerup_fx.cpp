#include "powerup_fx.h"
#include "globals.h"
#include "dog.h"
#include "schemi.h"
#include "bn_core.h"
#include "bn_colors.h"
#include "bn_bg_palettes.h"
#include "bn_vector.h"
#include "bn_string.h"
#include "bn_optional.h"
#include "bn_sprite_ptr.h"
#include "bn_sprite_palette_ptr.h"
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

    bn::vector<bn::sprite_ptr, 16> txt;
    text_generator.set_center_alignment();
    text_generator.generate(0, -20, "IN PAUSA", txt);
    text_generator.set_left_alignment();

    constexpr int MAP_COLUMNS = 28;
    constexpr int MAP_ROWS = 6;
    char map[MAP_ROWS][MAP_COLUMNS];
    for (int y = 0; y < MAP_ROWS; y++)
        for (int x = 0; x < MAP_COLUMNS; x++)
            map[y][x] = ' ';

    const collision_map_info& collision = get_collision_map(g_schema);
    for (int y = 0; y < MAP_ROWS; y++)
    {
        int tile_y_start = y * collision.rows / MAP_ROWS;
        int tile_y_end = (y + 1) * collision.rows / MAP_ROWS;
        for (int x = 0; x < MAP_COLUMNS; x++)
        {
            int tile_x_start = x * collision.columns / MAP_COLUMNS;
            int tile_x_end = (x + 1) * collision.columns / MAP_COLUMNS;
            for (int tile_y = tile_y_start; tile_y < tile_y_end; tile_y++)
            {
                for (int tile_x = tile_x_start; tile_x < tile_x_end; tile_x++)
                {
                    int cell = tile_y * collision.columns + tile_x;
                    bool solid = collision.data[cell] != 0;
                    bool surface = tile_y == 0 || collision.data[cell - collision.columns] == 0;
                    if (solid && surface)
                        map[y][x] = '#';
                }
            }
        }
    }

    auto map_coordinate = [](int coordinate, int map_size, int cell_count)
    {
        int cell = coordinate * cell_count / map_size;
        if (cell < 0)
            return 0;
        if (cell >= cell_count)
            return cell_count - 1;
        return cell;
    };

    int dog_x = map_coordinate(int(g_dog->chr_x), collision.map_w, MAP_COLUMNS);
    int dog_y = map_coordinate(int(g_dog->chr_y), collision.map_h, MAP_ROWS);
    map[dog_y][dog_x] = 'D';

    int door_count = 0;
    const door_def* doors = get_schema_doors(g_schema, door_count);
    if (door_count > 0)
    {
        const door_def& exit = doors[door_count - 1];
        int exit_x = map_coordinate(exit.x, collision.map_w, MAP_COLUMNS);
        int exit_y = map_coordinate(exit.y, collision.map_h, MAP_ROWS);
        map[exit_y][exit_x] = 'E';
    }

    bn::vector<bn::sprite_ptr, 128> minimap_sprites;
    text_generator.set_center_alignment();
    for (int y = 0; y < MAP_ROWS; y++)
    {
        bn::string<MAP_COLUMNS + 1> line;
        for (int x = 0; x < MAP_COLUMNS; x++)
            line.push_back(map[y][x]);
        text_generator.generate(0, 26 + y * 6, line, minimap_sprites);
    }
    text_generator.generate(0, 66, "D DOG   E USCITA", minimap_sprites);
    text_generator.set_left_alignment();

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