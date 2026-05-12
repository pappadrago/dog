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
#include "bn_regular_bg_items_dayplatf.h"
#include "common_variable_8x16_sprite_font.h"

#include "game_constants.h"
#include "hud.h"
#include "ball.h"
#include "bau.h"
#include "enemy.h"
#include "human.h"
#include "dog.h"
#include "helper_dog.h"
#include "dog_selection.h"

// Generatore casuale globale condiviso da tutte le classi
bn::random random1Instance;

int main()
{
    bn::core::init();

    bn::regular_bg_ptr bg0 = bn::regular_bg_items::bg_country.create_bg(0);
    bn::regular_bg_ptr bg1 = bn::regular_bg_items::bg_country.create_bg(1);
    bn::regular_bg_ptr bg3 = bn::regular_bg_items::dayplatf.create_bg(0);

    bn::sprite_text_generator text_generator(common::variable_8x16_sprite_font);
    update_text(0, &text_generator);

    bn::music_items::qwak.play(0.25);

    bg3.set_y(-32);
    bg3.set_x(896 - HALF_SCREEN_W);

    bn::camera_ptr camera = bn::camera_ptr::create(0, 0);
    bg3.set_camera(camera);

    int skin_selezionato = dog_selection_screen(text_generator);

    bau bau;
    bau.sprite->set_camera(camera);

    ball ball;
    ball.player->set_camera(camera);
    ball.shadow->set_camera(camera);
    ball.shadowRed->set_camera(camera);
    ball.player->set_bg_priority(2);

    human human(&ball);
    human.player->set_camera(camera);
    human.player->set_bg_priority(2);
    human.barra_rossa->set_camera(camera);
    human.barra_verde->set_camera(camera);

    bn::vector<enemy, MAX_ENEMIES> enemies;

    dog dog(skin_selezionato, &ball, &human, &bau, enemies);
    dog.player->set_camera(camera);
    dog.player->set_bg_priority(2);

    helper_dog helper(skin_selezionato, &ball, &human, enemies, &camera);

    const bn::regular_bg_map_item& bg3_map_item = bn::regular_bg_items::country.map_item();

    while (true)
    {
        // Reset stato palla tra un round e laltro
        ball.pickedup = false;
        ball.rolling  = false;
        ball.chr_y    = 1000;

        // --- Attendi START ---
        while (true)
        {
            random1Instance.get_int(); // consuma un valore per mantenere variabilita
            if (bn::keypad::start_released()) break;

            update_text(0, &dog);
            if (dog.chr_x > 100) dog.chr_x = 100;

            dog.update();
            human.update();
            bn::core::update();
        }

        // --- Spawn o potenziamento nemici ---
        if (dog.schema % 2 == 0)
        {
            enemy w(&ball, &bau);
            w.player->set_camera(camera);
            w.do_spawn();
            enemies.push_back(bn::move(w));
        }
        else if (dog.schema % 2 == 1)
        {
            for (enemy& e : enemies)
                e.max_vx += bn::fixed(0.2);
        }

        update_text(-1);
        human.ticks = 120;

        // --- Loop principale del round ---
        while (true)
        {
            dog.update();
            human.update();
            ball.update();
            bau.update();

            if (helper.active)
                helper.update();

            if (bn::keypad::l_pressed() && !helper.active)
                helper.spawn(dog.chr_x - 150);

            for (enemy& e : enemies)
                e.update();

            // Legge tile mappa sotto il cane (per usi futuri)
            int tx = (dog.chr_x).division(8).integer();
            int ty = ((dog.chr_y - 32)).division(8).integer();
            bn::regular_bg_map_cell cella = bg3_map_item.cell(bn::point(tx, ty));
            bn::regular_bg_map_cell_info info(cella);
            (void)info;

            // Segue il cane con la camera
            if (dog.chr_x > HALF_SCREEN_W && dog.chr_x < 1792 - HALF_SCREEN_W)
            {
                bn::fixed cam_x = dog.chr_x - HALF_SCREEN_W;
                camera.set_x(cam_x);
                bg0.set_x(cam_x * bn::fixed(-0.25));
                bg1.set_x(cam_x * bn::fixed(-0.5));
            }

            update_text_tick();
            bn::core::update();

            // Fine round: 3 palline riportate
            if (dog.palline_riportate == 3)
            {
                dog.palline_riportate = 0;
                dog.schema++;
                break;
            }
        }
    }
}
