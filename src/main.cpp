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
#include "game_context.h"

// Generatore casuale globale condiviso da tutte le classi
bn::random random1Instance;

int main()
{
    bn::core::init();

    bn::regular_bg_ptr bg0 = bn::regular_bg_items::bg_country.create_bg(0);
    bn::regular_bg_ptr bg1 = bn::regular_bg_items::bg_country.create_bg(1);
    bn::regular_bg_ptr bg3 = bn::regular_bg_items::dayplatf.create_bg(0);

    bn::sprite_text_generator text_generator(common::variable_8x16_sprite_font);
    update_text_init(&text_generator);
    update_text(0);

    bn::music_items::qwak.play(0.25);

    bg3.set_y(-32);
    bg3.set_x(896 - HALF_SCREEN_W);

    // bn::camera_ptr camera = bn::camera_ptr::create(0, 0);
    // bg3.set_camera(camera);

    int skin_selezionato = dog_selection_screen(text_generator);

    GameContext ctx(text_generator, bg0, bg1, bg3, skin_selezionato);

    bg3.set_camera(ctx.camera);
    const bn::regular_bg_map_item& bg3_map_item = bn::regular_bg_items::country.map_item();

    while (true)
    {
        // Reset stato palla tra un round e laltro
        ctx.the_ball.pickedup = false;
        ctx.the_ball.rolling = false;
        ctx.the_ball.chr_y = 1000;

        // --- Attendi START ---
        while (true)
        {
            random1Instance.get_int(); // mantieni variabilita RNG
            if (bn::keypad::start_released()) break;

            update_text_schema(ctx.the_dog.schema);
            if (ctx.the_dog.chr_x > 100) ctx.the_dog.chr_x = 100;

            ctx.the_dog.update();
            ctx.the_human.update();
            bn::core::update();
        }

        // --- Spawn o potenziamento nemici ---
        if (ctx.the_dog.schema % 2 == 0)
        {
            enemy w(&ctx.the_ball, &ctx.the_bau, &ctx.the_dog);
            w.player->set_camera(ctx.camera);
            w.do_spawn();
            ctx.enemies.push_back(bn::move(w));
        }
        else if (ctx.the_dog.schema % 2 == 1)
        {
            for (enemy& e : ctx.enemies)
                e.max_vx += bn::fixed(0.2);
        }

        update_text_clear();
        ctx.the_human.ticks = 120;

        // --- Loop principale del round ---
        while (true)
        {
            ctx.the_dog.update();
            ctx.the_human.update();
            ctx.the_ball.update();
            ctx.the_bau.update();

            if (ctx.the_helper.active)
                ctx.the_helper.update();

            if (bn::keypad::l_pressed() && !ctx.the_helper.active)
                ctx.the_helper.spawn(ctx.the_dog.chr_x - 150);

            for (enemy& e : ctx.enemies)
                e.update();

            // Legge tile mappa sotto il cane (per usi futuri)
            int tx = ctx.the_dog.chr_x.division(8).integer();
            int ty = (ctx.the_dog.chr_y - 32).division(8).integer();
            bn::regular_bg_map_cell cella = bg3_map_item.cell(bn::point(tx, ty));
            bn::regular_bg_map_cell_info cell_info(cella);
            (void)cell_info;

            // Segue il cane con la camera
            if (ctx.the_dog.chr_x > HALF_SCREEN_W && ctx.the_dog.chr_x < 1792 - HALF_SCREEN_W)
            {
                bn::fixed cam_x = ctx.the_dog.chr_x - HALF_SCREEN_W;
                ctx.camera.set_x(cam_x);
                bg0.set_x(cam_x * bn::fixed(-0.25));
                bg1.set_x(cam_x * bn::fixed(-0.5));
            }

            update_text_tick();
            bn::core::update();

            if (ctx.the_dog.palline_riportate == 3)
            {
                ctx.the_dog.palline_riportate = 0;
                ctx.the_dog.schema++;
                break; // inizia round successivo
            }
        }
    }
}
