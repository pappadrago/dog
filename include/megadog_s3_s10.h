#pragma once

#include <array>
#include "schemi.h"
#include "scala.h"
#include "bn_regular_bg_items_s3.h"
#include "bn_regular_bg_items_s4.h"
#include "bn_regular_bg_items_s5.h"
#include "bn_regular_bg_items_s6.h"
#include "bn_regular_bg_items_s7.h"
#include "bn_regular_bg_items_s8.h"
#include "bn_regular_bg_items_s9.h"
#include "bn_regular_bg_items_s10.h"
#include "bn_regular_bg_items_bg_s1.h"
#include "bn_regular_bg_items_s1fg.h"

namespace megadog_s3_s10
{
    constexpr int COLUMNS = 192;
    constexpr int ROWS = 32;
    using collision_grid = std::array<uint8_t, COLUMNS * ROWS>;

    constexpr void line(collision_grid& grid, int x1, int x2, int y, uint8_t value = 1)
    {
        for(int x = x1; x <= x2; ++x)
        {
            grid[y * COLUMNS + x] = value;
        }
    }

    constexpr collision_grid make_collision(int level)
    {
        collision_grid grid{};
        line(grid, 0, COLUMNS - 1, 31);

        if(level == 0)
        {
            line(grid, 5, 17, 28); line(grid, 22, 33, 24);
            line(grid, 12, 24, 25); line(grid, 34, 45, 21);
            line(grid, 40, 51, 19); line(grid, 57, 69, 27);
            line(grid, 68, 78, 23); line(grid, 89, 103, 24);
            line(grid, 91, 104, 18); line(grid, 111, 124, 14);
            line(grid, 124, 137, 20); line(grid, 132, 144, 20);
            line(grid, 150, 163, 14); line(grid, 151, 163, 26);
            line(grid, 169, 181, 10); line(grid, 175, 186, 15);
        }
        else if(level == 1)
        {
            line(grid, 43, 50, 31, 0); line(grid, 107, 116, 31, 0);
            line(grid, 8, 20, 28); line(grid, 24, 34, 23);
            line(grid, 34, 43, 25, 2); line(grid, 48, 59, 21);
            line(grid, 42, 52, 28); line(grid, 54, 66, 24);
            line(grid, 70, 81, 19); line(grid, 82, 98, 25, 2);
            line(grid, 86, 96, 22); line(grid, 100, 110, 27);
            line(grid, 105, 118, 18); line(grid, 118, 129, 28);
            line(grid, 132, 145, 23); line(grid, 128, 143, 23, 2);
            line(grid, 149, 161, 18); line(grid, 157, 169, 20);
            line(grid, 165, 178, 23);
        }
        else if(level == 2)
        {
            line(grid, 58, 67, 31, 0); line(grid, 128, 139, 31, 0);
            line(grid, 7, 19, 28); line(grid, 25, 38, 23);
            line(grid, 48, 70, 27, 2); line(grid, 76, 91, 23);
            line(grid, 42, 54, 18); line(grid, 59, 70, 27);
            line(grid, 74, 87, 23); line(grid, 93, 106, 18);
            line(grid, 105, 121, 27, 2); line(grid, 112, 125, 26);
            line(grid, 131, 142, 22); line(grid, 136, 153, 23);
            line(grid, 147, 160, 17); line(grid, 165, 178, 19);
            line(grid, 166, 178, 22);
        }
        else if(level == 3)
        {
            line(grid, 6, 17, 28); line(grid, 23, 34, 24);
            line(grid, 15, 25, 26); line(grid, 34, 45, 22);
            line(grid, 42, 53, 20); line(grid, 55, 66, 18);
            line(grid, 62, 73, 16); line(grid, 78, 91, 24);
            line(grid, 82, 93, 12); line(grid, 102, 113, 8);
            line(grid, 101, 114, 20); line(grid, 123, 134, 13);
            line(grid, 126, 140, 16); line(grid, 143, 154, 18);
            line(grid, 151, 165, 22); line(grid, 163, 175, 23);
            line(grid, 172, 183, 27);
        }
        else if(level == 4)
        {
            line(grid, 65, 73, 31, 0); line(grid, 115, 123, 31, 0);
            line(grid, 76, 79, 22); line(grid, 109, 112, 20);
            line(grid, 7, 18, 28); line(grid, 21, 32, 24);
            line(grid, 26, 40, 25); line(grid, 49, 63, 21, 2);
            line(grid, 35, 47, 20); line(grid, 52, 63, 26);
            line(grid, 66, 76, 28); line(grid, 69, 84, 26, 2);
            line(grid, 80, 92, 24); line(grid, 96, 108, 18);
            line(grid, 91, 105, 20); line(grid, 110, 120, 26);
            line(grid, 124, 136, 22); line(grid, 117, 132, 25, 2);
            line(grid, 140, 152, 17); line(grid, 145, 159, 19);
            line(grid, 157, 169, 13); line(grid, 166, 181, 24, 2);
            line(grid, 174, 186, 20);
        }
        else if(level == 5)
        {
            line(grid, 91, 99, 31, 0); line(grid, 145, 153, 31, 0);
            line(grid, 5, 16, 28); line(grid, 20, 31, 24);
            line(grid, 18, 30, 25); line(grid, 38, 51, 20);
            line(grid, 35, 46, 20); line(grid, 50, 61, 26);
            line(grid, 64, 76, 21); line(grid, 52, 60, 18);
            line(grid, 58, 71, 16); line(grid, 78, 91, 22);
            line(grid, 80, 91, 16); line(grid, 99, 110, 27);
            line(grid, 98, 111, 17); line(grid, 113, 125, 22);
            line(grid, 86, 102, 28); line(grid, 138, 155, 28);
            line(grid, 119, 132, 23); line(grid, 131, 142, 22);
            line(grid, 140, 153, 18); line(grid, 149, 160, 17);
            line(grid, 154, 165, 26); line(grid, 161, 175, 24);
            line(grid, 168, 179, 21); line(grid, 180, 188, 16);
        }
        else if(level == 6)
        {
            line(grid, 8, 19, 28); line(grid, 25, 36, 24);
            line(grid, 24, 35, 25); line(grid, 47, 59, 21);
            line(grid, 42, 53, 20); line(grid, 59, 70, 16);
            line(grid, 70, 83, 17); line(grid, 76, 87, 12);
            line(grid, 93, 104, 8); line(grid, 94, 107, 23);
            line(grid, 110, 121, 12); line(grid, 119, 132, 19);
            line(grid, 126, 137, 25);
            line(grid, 136, 147, 18); line(grid, 144, 157, 15);
            line(grid, 159, 170, 23); line(grid, 167, 180, 22);
            line(grid, 177, 187, 28);
        }
        else
        {
            line(grid, 78, 92, 31, 0); line(grid, 143, 155, 31, 0);
            line(grid, 6, 18, 28); line(grid, 22, 34, 24);
            line(grid, 17, 30, 26); line(grid, 39, 52, 22, 2);
            line(grid, 38, 50, 20); line(grid, 54, 66, 16);
            line(grid, 70, 81, 23); line(grid, 62, 76, 18);
            line(grid, 93, 104, 28); line(grid, 83, 98, 24, 2);
            line(grid, 74, 96, 28); line(grid, 138, 160, 28);
            line(grid, 107, 119, 23); line(grid, 106, 120, 19);
            line(grid, 123, 135, 18); line(grid, 139, 150, 25);
            line(grid, 130, 144, 25, 2); line(grid, 156, 168, 20);
            line(grid, 155, 170, 20); line(grid, 173, 185, 16);
            line(grid, 174, 185, 26);
        }
        return grid;
    }

    constexpr auto collision_3 = make_collision(0);
    constexpr auto collision_4 = make_collision(1);
    constexpr auto collision_5 = make_collision(2);
    constexpr auto collision_6 = make_collision(3);
    constexpr auto collision_7 = make_collision(4);
    constexpr auto collision_8 = make_collision(5);
    constexpr auto collision_9 = make_collision(6);
    constexpr auto collision_10 = make_collision(7);

    constexpr collision_map_info collision_info_3 =
        { collision_3.data(), COLUMNS, ROWS, COLUMNS * 16, ROWS * 16 };
    constexpr collision_map_info collision_info_4 =
        { collision_4.data(), COLUMNS, ROWS, COLUMNS * 16, ROWS * 16 };
    constexpr collision_map_info collision_info_5 =
        { collision_5.data(), COLUMNS, ROWS, COLUMNS * 16, ROWS * 16 };
    constexpr collision_map_info collision_info_6 =
        { collision_6.data(), COLUMNS, ROWS, COLUMNS * 16, ROWS * 16 };
    constexpr collision_map_info collision_info_7 =
        { collision_7.data(), COLUMNS, ROWS, COLUMNS * 16, ROWS * 16 };
    constexpr collision_map_info collision_info_8 =
        { collision_8.data(), COLUMNS, ROWS, COLUMNS * 16, ROWS * 16 };
    constexpr collision_map_info collision_info_9 =
        { collision_9.data(), COLUMNS, ROWS, COLUMNS * 16, ROWS * 16 };
    constexpr collision_map_info collision_info_10 =
        { collision_10.data(), COLUMNS, ROWS, COLUMNS * 16, ROWS * 16 };

    constexpr cassa_def casse[] = {
        { CASSA_SCATOLA, 560, 480, false },
        { CASSA_SCATOLA, 1040, 480, false },
        { CASSA_SCATOLA, 1984, 480, false },
        { CASSA_SCATOLA, 2304, 480, false },
        { CASSA_SCATOLA, 880, 480, false },
        { CASSA_SCATOLA, 2560, 480, false },
    };
    constexpr trampolino_def trampolini[] = {
        { 1184, 480 }, { 1472, 480 }, { 1920, 480 }, { 2656, 480 },
    };
    constexpr lava_zona_def lava[] = {
        { 43, 31, 50, 31, { { 1, 2, 12, 12 }, { 2, 3, 12, 12 } }, 2, 12, 15 },
        { 58, 31, 67, 31, { { 1, 2, 12, 12 }, { 2, 3, 12, 12 } }, 2, 12, 15 },
        { 128, 31, 139, 31, { { 1, 2, 12, 12 }, { 2, 3, 12, 12 } }, 2, 12, 15 },
        { 91, 31, 99, 31, { { 1, 2, 12, 12 }, { 2, 3, 12, 12 } }, 2, 12, 15 },
        { 145, 31, 153, 31, { { 1, 2, 12, 12 }, { 2, 3, 12, 12 } }, 2, 12, 15 },
    };
    constexpr item_def items[] = {
        { ITEM_TIPO_CHIAVE, 128, 480, 448 },
        { ITEM_TIPO_POWERUP, POWERUP_DOPPIO_SALTO, 608, 320 },
        { ITEM_TIPO_ENERGIA, 30, 1536, 416 },
        { ITEM_TIPO_CHIAVE, 129, 736, 400 },
        { ITEM_TIPO_POWERUP, POWERUP_DASH, 944, 320 },
        { ITEM_TIPO_ENERGIA, 30, 1856, 416 },
        { ITEM_TIPO_CHIAVE, 130, 864, 400 },
        { ITEM_TIPO_POWERUP, POWERUP_WALL_JUMP, 1216, 320 },
        { ITEM_TIPO_ENERGIA, 30, 2208, 416 },
        { ITEM_TIPO_CHIAVE, 131, 1104, 384 },
        { ITEM_TIPO_POWERUP, POWERUP_AIR_DASH, 1520, 272 },
        { ITEM_TIPO_ENERGIA, 30, 2384, 416 },
        { ITEM_TIPO_CHIAVE, 132, 976, 416 },
        { ITEM_TIPO_POWERUP, POWERUP_GROUND_POUND, 1312, 336 },
        { ITEM_TIPO_ENERGIA, 30, 2160, 416 },
        { ITEM_TIPO_INTERRUTTORE, 41, 1248, 368 },
        { ITEM_TIPO_CHIAVE, 133, 1280, 400 },
        { ITEM_TIPO_POWERUP, POWERUP_BAU | POWERUP_RESISTENZA, 1760, 336 },
        { ITEM_TIPO_ENERGIA, 30, 2496, 416 },
        { ITEM_TIPO_CHIAVE, 134, 1184, 384 },
        { ITEM_TIPO_POWERUP, POWERUP_DOPPIO_SALTO | POWERUP_CORSA, 1664, 320 },
        { ITEM_TIPO_ENERGIA, 30, 2112, 416 },
        { ITEM_TIPO_CHIAVE, 135, 1440, 400 },
        { ITEM_TIPO_POWERUP, POWERUP_WALL_JUMP | POWERUP_AIR_DASH, 1888, 320 },
        { ITEM_TIPO_ENERGIA, 30, 2624, 416 },
    };
    constexpr piattaforma_def piattaforme[] = {
        { 76, 22, 79, 22, { 6, 7, 15, 16 }, { 4, 5, 13, 14 }, 0, 41 },
        { 109, 20, 112, 20, { 6, 7, 15, 16 }, { 4, 5, 13, 14 }, 1, 41 },
    };
    constexpr piattaforma_mobile_def mobili[] = {
        { 960, 400, 0, 72, 0, 8, true, true, 0, -1, PIATTAFORMA_MOBILE_SOLIDO, 0, 0, -1, -1 },
        { 1776, 352, 0, 64, 0, 8, false, true, 0, -1, PIATTAFORMA_MOBILE_SOLIDO, 0, 0, -1, -1 },
        { 2528, 384, 0, 80, 0, 8, true, true, 0, -1, PIATTAFORMA_MOBILE_CLOUD, 0, 0, -1, -1 },
    };
    constexpr enemy_def enemies[] = {
        { enemy_tipo::MOVE_PATROL, enemy_attr::ATTR_BOMB | enemy_attr::ATTR_RESPAWN, 704, 464, 60 },
        { enemy_tipo::MOVE_STATIC, enemy_attr::ATTR_LIGHTNING | enemy_attr::ATTR_RESPAWN, 1504, 464, 90 },
        { enemy_tipo::MOVE_CHASE_SHY, enemy_attr::ATTR_GRAVITY_ARROW | enemy_attr::ATTR_AIM | enemy_attr::ATTR_RESPAWN, 1184, 400, 80 },
        { enemy_tipo::MOVE_STATIC_JUMP, enemy_attr::ATTR_SHELL | enemy_attr::ATTR_RESPAWN, 2016, 464, 70 },
        { enemy_tipo::MOVE_GHOST, enemy_attr::ATTR_FIREBALL | enemy_attr::ATTR_AIM | enemy_attr::ATTR_RESPAWN, 832, 400, 100 },
        { enemy_tipo::MOVE_MELEE, enemy_attr::ATTR_MELEE | enemy_attr::ATTR_RESPAWN, 1696, 464, 50 },
        { enemy_tipo::MOVE_PATROL, enemy_attr::ATTR_MINA | enemy_attr::ATTR_RESPAWN, 2288, 464, 80 },
        { enemy_tipo::MOVE_CHASE, enemy_attr::ATTR_ARROW | enemy_attr::ATTR_AIM | enemy_attr::ATTR_RESPAWN, 2608, 464, 60 },
        { enemy_tipo::MOVE_STATIC, enemy_attr::ATTR_BOMB | enemy_attr::ATTR_RESPAWN, 1408, 464, 80 },
        { enemy_tipo::MOVE_GHOST_SHY, enemy_attr::ATTR_FIREBALL | enemy_attr::ATTR_AIM | enemy_attr::ATTR_RESPAWN, 2448, 368, 60 },
        { enemy_tipo::MOVE_STATIC_JUMP, enemy_attr::ATTR_GRAVITY_ARROW | enemy_attr::ATTR_RESPAWN, 2784, 464, 75 },
        { enemy_tipo::MOVE_CHASE, enemy_attr::ATTR_SHELL | enemy_attr::ATTR_RESPAWN, 992, 464, 60 },
        { enemy_tipo::MOVE_MELEE, enemy_attr::ATTR_BASH | enemy_attr::ATTR_RESPAWN, 1536, 464, 50 },
        { enemy_tipo::MOVE_CHASE_SHY, enemy_attr::ATTR_MINA | enemy_attr::ATTR_AIM | enemy_attr::ATTR_RESPAWN, 2224, 464, 80 },
        { enemy_tipo::MOVE_STATIC, enemy_attr::ATTR_LIGHTNING | enemy_attr::ATTR_RESPAWN, 2688, 464, 80 },
        { enemy_tipo::MOVE_PATROL, enemy_attr::ATTR_BOMB | enemy_attr::ATTR_SHIELD | enemy_attr::ATTR_RESPAWN, 2912, 464, 90 },
    };
    constexpr door_def doors[] = {
        { 32, 480, 2, 1, 127 }, { 3024, 480, 4, 0, 128 },
        { 32, 480, 3, 1, 128 }, { 3024, 480, 5, 0, 129 },
        { 32, 480, 4, 1, 129 }, { 3024, 480, 6, 0, 130 },
        { 32, 480, 5, 1, 130 }, { 3024, 480, 7, 0, 131 },
        { 32, 480, 6, 1, 131 }, { 3024, 480, 8, 0, 132 },
        { 32, 480, 7, 1, 132 }, { 3024, 480, 9, 0, 133 },
        { 32, 480, 8, 1, 133 }, { 3024, 480, 10, 0, 134 },
        { 32, 480, 9, 1, 134 }, { 3024, 480, 1, 0, 135 },
    };
    constexpr scala_def scale[] = {
        { 22, 24, 30, SCALA_CON_PIANO }, { 106, 11, 18, SCALA_CON_PIANO },
        { 32, 23, 30, SCALA_CON_PIANO }, { 128, 16, 26, SCALA_CON_PIANO },
        { 30, 20, 30, SCALA_CON_PIANO }, { 145, 15, 27, SCALA_CON_PIANO },
        { 53, 18, 30, SCALA_CON_PIANO }, { 112, 8, 20, SCALA_CON_PIANO },
        { 32, 21, 30, SCALA_CON_PIANO }, { 112, 17, 30, SCALA_CON_PIANO },
        { 77, 18, 30, SCALA_CON_PIANO }, { 166, 10, 22, SCALA_CON_PIANO },
        { 45, 20, 30, SCALA_CON_PIANO }, { 135, 12, 24, SCALA_CON_PIANO },
        { 32, 20, 30, SCALA_CON_PIANO }, { 165, 16, 30, SCALA_CON_PIANO },
    };
    constexpr spike_def spikes[] = {
        { 27, 31, SPIKE_ALTA_16, false, 90, 30, 4, 45, 40, 0, 10 },
        { 28, 31, SPIKE_ALTA_16, false, 90, 30, 4, 45, 40, 60, 10 },
        { 71, 31, SPIKE_ALTA_32, false, 120, 40, 4, 60, 60, 30, 15 },
        { 72, 31, SPIKE_ALTA_32, false, 120, 40, 4, 60, 60, 90, 15 },
        { 89, 31, SPIKE_ALTA_16, false, 100, 30, 4, 50, 40, 0, 10 },
        { 90, 31, SPIKE_ALTA_16, false, 100, 30, 4, 50, 40, 60, 10 },
        { 120, 31, SPIKE_ALTA_32, false, 120, 40, 4, 60, 60, 30, 15 },
        { 121, 31, SPIKE_ALTA_32, false, 120, 40, 4, 60, 60, 90, 15 },
        { 55, 18, SPIKE_ALTA_16, true, 90, 30, 4, 45, 40, 0, 10 },
        { 56, 18, SPIKE_ALTA_16, true, 90, 30, 4, 45, 40, 60, 10 },
        { 130, 25, SPIKE_ALTA_32, true, 120, 40, 4, 60, 60, 30, 15 },
        { 131, 25, SPIKE_ALTA_32, true, 120, 40, 4, 60, 60, 90, 15 },
        { 74, 31, SPIKE_ALTA_16, false, 110, 30, 4, 50, 45, 0, 10 },
        { 75, 31, SPIKE_ALTA_16, false, 110, 30, 4, 50, 45, 60, 10 },
        { 165, 20, SPIKE_ALTA_32, true, 120, 40, 4, 60, 60, 30, 15 },
        { 166, 20, SPIKE_ALTA_32, true, 120, 40, 4, 60, 60, 90, 15 },
    };
    constexpr oggetto_percorso_def oggetti_percorso[] = {
        { 2240, 320, 48, 24, PERCORSO_RETTANGOLO, 12, true, 0, PERCORSO_SPRITE_16, 10 },
        { 2752, 352, 64, 32, PERCORSO_RETTANGOLO, 15, false, 90, PERCORSO_SPRITE_16, 10 },
    };
    constexpr ground_friction_def frizioni[] = {
        { 42, 31, 60, 31, bn::fixed(0.01) },
        { 97, 31, 115, 31, bn::fixed(0.08) },
    };

    struct stage_contents
    {
        const cassa_def* casse; int casse_count;
        const trampolino_def* trampolini; int trampolini_count;
        const lava_zona_def* lava; int lava_count;
        const item_def* items; int items_count;
        const piattaforma_def* piattaforme; int piattaforme_count;
        const piattaforma_mobile_def* mobili; int mobili_count;
        const enemy_def* enemies; int enemies_count;
        const door_def* doors; int doors_count;
        const collision_map_info* map;
        const scala_def* scale; int scale_count;
        const spike_def* spikes; int spikes_count;
        const oggetto_percorso_def* oggetti; int oggetti_count;
        const ground_friction_def* frizioni; int frizioni_count;
    };

    constexpr stage_contents stages[] = {
        { casse, 1, trampolini, 1, lava, 0, items, 3, piattaforme, 0, mobili, 0,
          enemies, 2, doors, 2, &collision_info_3, scale, 2, spikes, 0, oggetti_percorso, 0, frizioni, 0 },
        { casse + 1, 0, trampolini, 0, lava, 1, items + 3, 3, piattaforme, 0, mobili, 1,
          enemies + 2, 2, doors + 2, 2, &collision_info_4, scale + 2, 2, spikes, 2, oggetti_percorso, 0, frizioni, 0 },
        { casse + 1, 1, trampolini, 0, lava + 1, 2, items + 6, 3, piattaforme, 0, mobili, 0,
          enemies + 4, 2, doors + 4, 2, &collision_info_5, scale + 4, 2, spikes + 2, 2, oggetti_percorso, 0, frizioni, 0 },
        { casse + 2, 0, trampolini + 1, 1, lava + 3, 0, items + 9, 3, piattaforme, 0, mobili + 1, 1,
          enemies + 6, 2, doors + 6, 2, &collision_info_6, scale + 6, 2, spikes + 4, 2, oggetti_percorso, 0, frizioni, 0 },
        { casse + 2, 1, trampolini + 1, 0, lava + 3, 0, items + 12, 4, piattaforme, 2, mobili, 0,
          enemies + 8, 2, doors + 8, 2, &collision_info_7, scale + 8, 2, spikes + 6, 2, oggetti_percorso, 0, frizioni, 0 },
        { casse + 3, 0, trampolini + 2, 1, lava + 3, 2, items + 16, 3, piattaforme, 0, mobili, 0,
          enemies + 10, 2, doors + 10, 2, &collision_info_8, scale + 10, 2, spikes + 8, 2, oggetti_percorso, 1, frizioni, 0 },
        { casse + 3, 1, trampolini + 2, 0, lava + 5, 0, items + 19, 3, piattaforme, 0, mobili + 2, 1,
          enemies + 12, 2, doors + 12, 2, &collision_info_9, scale + 12, 2, spikes + 10, 2, oggetti_percorso + 1, 1, frizioni, 0 },
        { casse + 4, 2, trampolini + 3, 1, lava + 5, 0, items + 22, 3, piattaforme, 0, mobili, 0,
          enemies + 14, 2, doors + 14, 2, &collision_info_10, scale + 14, 2, spikes + 12, 4, oggetti_percorso + 1, 1, frizioni, 2 },
    };

    constexpr schema_def make_schema(const stage_contents& stage, bn::regular_bg_item level_bg)
    {
        return {
            stage.casse, stage.casse_count,
            stage.trampolini, stage.trampolini_count,
            stage.lava, stage.lava_count,
            stage.items, stage.items_count,
            stage.piattaforme, stage.piattaforme_count,
            stage.mobili, stage.mobili_count,
            stage.enemies, stage.enemies_count,
            stage.doors, stage.doors_count,
            level_bg, bn::regular_bg_items::bg_s1, bn::regular_bg_items::s1fg,
            stage.map,
            stage.scale, stage.scale_count,
            nullptr, 0,
            stage.spikes, stage.spikes_count,
            stage.oggetti, stage.oggetti_count,
            stage.frizioni, stage.frizioni_count
        };
    }

    constexpr schema_def make_schema_3() { return make_schema(stages[0], bn::regular_bg_items::s3); }
    constexpr schema_def make_schema_4() { return make_schema(stages[1], bn::regular_bg_items::s4); }
    constexpr schema_def make_schema_5() { return make_schema(stages[2], bn::regular_bg_items::s5); }
    constexpr schema_def make_schema_6() { return make_schema(stages[3], bn::regular_bg_items::s6); }
    constexpr schema_def make_schema_7() { return make_schema(stages[4], bn::regular_bg_items::s7); }
    constexpr schema_def make_schema_8() { return make_schema(stages[5], bn::regular_bg_items::s8); }
    constexpr schema_def make_schema_9() { return make_schema(stages[6], bn::regular_bg_items::s9); }
    constexpr schema_def make_schema_10() { return make_schema(stages[7], bn::regular_bg_items::s10); }
}
