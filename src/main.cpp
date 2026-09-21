#include "bn_core.h"
#include "bn_random.h"
#include "bn_keypad.h"
#include "bn_camera_ptr.h"
#include "bn_music_items.h"
#include "bn_bg_palettes.h"
#include "bn_sprite_text_generator.h"
#include "bn_regular_bg_ptr.h"
#include "bn_regular_bg_map_cell_info.h"
#include "bn_regular_bg_items_bg_country.h"
#include "bn_regular_bg_items_country.h"
#include "bn_regular_bg_items_s1.h"
#include "bn_regular_bg_items_s1fg.h"
#include "common_variable_8x16_sprite_font.h"
#include "bn_log.h"

#include "globals.h"
#include "game_constants.h"
#include "hud.h"
#include "bau.h"
#include "enemy.h"
#include "item.h"
#include "dog.h"
#include "dog_selection.h"
#include "game_timer.h"


int main()
{
    bn::core::init();



    bn::sprite_text_generator text_generator(common::variable_8x16_sprite_font);
    update_text_init(&text_generator);
    update_text(0);

    //bn::music_items::qwak.play(0.25);

    int skin_selezionato = dog_selection_screen(text_generator);

    bn::regular_bg_ptr bg0 = bn::regular_bg_items::bg_country.create_bg(0);
    bn::regular_bg_ptr bg1 = bn::regular_bg_items::bg_country.create_bg(1);
    bn::regular_bg_ptr foreground = bn::regular_bg_items::s1.create_bg(0);
    bn::regular_bg_ptr foregroundfg = bn::regular_bg_items::s1fg.create_bg(0);

    foreground.set_x(-bn::fixed(MAP_HALF_W) - HALF_SCREEN_W);
    foreground.set_y(0);

    foregroundfg.set_x(-bn::fixed(MAP_HALF_W) - HALF_SCREEN_W);
    foregroundfg.set_y(0);


    foregroundfg.set_priority(0);
    foreground.set_priority(2);
    bg0.set_priority(3);
    bg1.set_priority(3);


    // Costruzione oggetti di gioco in ordine di dipendenza
    g_camera.emplace(bn::camera_ptr::create(0, 0));
    g_bau.emplace();
    g_enemies.emplace();
    g_items.emplace();
    g_timer.emplace();
    g_dog.emplace(skin_selezionato);

    while (true)
    {
        // --- Spawn o potenziamento nemici ---

{
            enemy* new_enemy = new enemy(TIPO_NEMICO_SPADACCINO_PATTUGLIATORE, ATTRIBUTO_NO);
            g_enemies->push_back(new_enemy);
            new_enemy->init();
        }

        for (int i = 0; i < 8; ++i) {
            enemy* new_enemy = new enemy(i%6, ATTRIBUTO_AIM);
            g_enemies->push_back(new_enemy);
            new_enemy->init();
        }
        for (int i = 0; i < 8; ++i) {
            item* new_item = new item();
            g_items->push_back(new_item);
            new_item->do_spawn();
        }
        update_text_clear();

        // --- Loop principale del round ---
        bn::fixed max_cpu_usage;

        while (true) {


            g_dog->update();
            g_bau->update();

            for (enemy* e : *g_enemies)
                e->update();
            for (item* e : *g_items)
                e->update();

            // Segue il cane con la camera
            if (g_dog->chr_x > HALF_SCREEN_W && g_dog->chr_x < bn::fixed(MAP_W) - HALF_SCREEN_W)
            {
                bn::fixed cam_x = g_dog->chr_x - HALF_SCREEN_W;
                g_camera->set_x(cam_x.integer());
                bg0.set_x(cam_x * bn::fixed(-0.25));
                bg1.set_x(cam_x * bn::fixed(-0.5));
                foreground.set_x(cam_x * bn::fixed(-1.0) - bn::fixed(MAP_HALF_W) - HALF_SCREEN_W);
                foregroundfg.set_x(foreground.x());
            }
            if (g_dog->chr_y > HALF_SCREEN_H && g_dog->chr_y < bn::fixed(MAP_H) - HALF_SCREEN_H)
            {
                bn::fixed cam_y = g_dog->chr_y - HALF_SCREEN_H;
                g_camera->set_y(cam_y.integer());
                foreground.set_y(cam_y * bn::fixed(-1.0) - bn::fixed(MAP_HALF_H) - HALF_SCREEN_H);
                foregroundfg.set_y(foreground.y());
            }

            update_text_tick();

            // 1. Aggiorna il timer
            bool timer_scaduto = g_timer->update(text_generator);

            // 2. Avviso visivo sotto i 10 secondi — lampeggio del testo
            if (g_timer->active && g_timer->seconds_left() <= 10)
            {
                bool visibile = (g_timer->frames_left / 8) % 2 == 0;
                for (auto& s : g_timer->text_sprites)
                    s.set_visible(visibile);
            }

            bn::core::update();
        }
    }
}
