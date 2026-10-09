#include "dog_selection.h"
#include "bn_keypad.h"
#include "bn_core.h"
#include "bn_array.h"
#include "bn_sprite_animate_actions.h"
#include "bn_sprite_items_fox1632.h"
#include "bn_sprite_items_enemies.h"
#include "bn_sprite_items_enemies2.h"
#include "bn_regular_bg_ptr.h"
#include "bn_regular_bg_items_cover.h"
#include "bn_regular_bg_items_bg_s1.h"
#include "globals.h"

namespace
{
    struct cameo_look { const bn::sprite_item* item; int tile; };

    // Riusa gli stessi sprite/tile dei nemici veri (vedi enemy.cpp) come piccola vetrina
    constexpr cameo_look CAMEO_LOOKS[] = {
        { &bn::sprite_items::enemies,  0 },   // spadaccino

        { &bn::sprite_items::enemies,  3 },   // spadaccino
        { &bn::sprite_items::enemies,  6 },   // spadaccino
        { &bn::sprite_items::enemies,  9 },   // spadaccino
        { &bn::sprite_items::enemies,  12 },   // spadaccino
        { &bn::sprite_items::enemies,  15 },   // spadaccino
        { &bn::sprite_items::enemies,  18 },   // spadaccino


        { &bn::sprite_items::enemies,  21 },   // arciere
        { &bn::sprite_items::enemies2, 15 },   // blob
        { &bn::sprite_items::enemies2, 9 },   // druido
    };
    constexpr int NUM_CAMEO_LOOKS = sizeof(CAMEO_LOOKS) / sizeof(CAMEO_LOOKS[0]);
}

void dog_selection_screen(bn::sprite_text_generator& text_generator)
{
    static constexpr int NUM_DOGS = 1;
    static constexpr int SPACING = 40;
    static constexpr int START_X = -100;
    static constexpr int DOG_Y = -48;
    static constexpr int CURSOR_Y = DOG_Y - 8;

    static constexpr int      CAMEO_Y = 64;    // "in basso", coordinata fissa
    static constexpr bn::fixed CAMEO_EDGE_X = 150;   // fuori schermo, entrata/uscita
    static constexpr bn::fixed CAMEO_TARGET_OFFSET = 20;   // "poco oltre" il centro
    static constexpr bn::fixed CAMEO_WALK_SPEED = 0.5;
    static constexpr bn::fixed CAMEO_RUN_SPEED = 2.0;

    bn::array<bn::optional<bn::sprite_ptr>, NUM_DOGS> sprites;
    bn::array<bn::optional<bn::sprite_animate_action<14>>, NUM_DOGS> anims;

    const bn::sprite_item* items[NUM_DOGS] = {
        &bn::sprite_items::fox1632,
    };

    for (int i = 0; i < NUM_DOGS; i++)
    {
        int x = START_X + i * SPACING;
        sprites[i] = items[i]->create_sprite(x, DOG_Y, 0);
        anims[i] = bn::create_sprite_animate_action_forever(
            *sprites[i], 6, items[i]->tiles_item(), 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55);
    }

    bn::sprite_ptr cursor = bn::sprite_items::fox1632.create_sprite(START_X, CURSOR_Y, 61);

    bn::vector<bn::sprite_ptr, 32> txt_sprites;
    text_generator.set_center_alignment();
    text_generator.generate(0, 0, "premi START per iniziare", txt_sprites);

    int selected = 0;
    int input_delay = 0;


    bn::regular_bg_ptr bg0 = bn::regular_bg_items::bg_s1.create_bg(0);
    bn::regular_bg_ptr bg1 = bn::regular_bg_items::bg_s1.create_bg(1);
    bg0.set_priority(3);
    bg1.set_priority(3);
    bn::regular_bg_ptr fg0 = bn::regular_bg_items::cover.create_bg(0);

    fg0.set_y(-24);

    // --- Stato del nemico in vetrina, in basso ---
    int cameo_look_idx = 0;
    bool cameo_da_sinistra = true;
    int cameo_fase = 0;   // 0 = cammina verso il centro, 1 = corre verso l'uscita
    bn::fixed cameo_x;
    bn::optional<bn::sprite_ptr> cameo_sprite;
    bn::optional<bn::sprite_animate_action<4>> cameo_anim;

    auto spawn_cameo = [&](bool da_sinistra, int look_idx)
        {
            const cameo_look& look = CAMEO_LOOKS[look_idx];

            cameo_da_sinistra = da_sinistra;
            cameo_x = da_sinistra ? -CAMEO_EDGE_X : CAMEO_EDGE_X;
            cameo_fase = 0;

            cameo_sprite = look.item->create_sprite(cameo_x, CAMEO_Y, look.tile);
            cameo_sprite->set_horizontal_flip(!da_sinistra);   // guarda verso il centro

            cameo_anim = bn::create_sprite_animate_action_forever(
                *cameo_sprite, 8, look.item->tiles_item(),
                look.tile + 0, look.tile + 1, look.tile + 2, look.tile + 1);   // camminata, lenta
        };

    spawn_cameo(true, 0);

    while (true)
    {
        if (input_delay > 0) input_delay--;
        if (input_delay == 0)
        {
            if (bn::keypad::right_pressed() && selected < NUM_DOGS - 1) { selected++; input_delay = 12; }
            else if (bn::keypad::left_pressed() && selected > 0) { selected--; input_delay = 12; }
        }
        cursor.set_x(START_X + 36 + selected * SPACING);
        for (int i = 0; i < NUM_DOGS; i++) if (anims[i].has_value()) anims[i]->update();


        // --- Aggiorna il nemico in vetrina ---
        {
            bn::fixed target_x = cameo_da_sinistra ? CAMEO_TARGET_OFFSET : -CAMEO_TARGET_OFFSET;

            if (cameo_fase == 0)
            {
                cameo_x += cameo_da_sinistra ? CAMEO_WALK_SPEED : -CAMEO_WALK_SPEED;
                if ((cameo_da_sinistra && cameo_x >= target_x) ||
                    (!cameo_da_sinistra && cameo_x <= target_x))
                {
                    cameo_fase = 1;
                    cameo_sprite->set_horizontal_flip(cameo_da_sinistra);   // ora torna indietro

                    const cameo_look& look = CAMEO_LOOKS[cameo_look_idx];
                    cameo_anim = bn::create_sprite_animate_action_forever(
                        *cameo_sprite, 3, look.item->tiles_item(),
                        look.tile + 0, look.tile + 1, look.tile + 2, look.tile + 1);   // corsa, veloce
                }
            }
            else
            {
                cameo_x += cameo_da_sinistra ? -CAMEO_RUN_SPEED : CAMEO_RUN_SPEED;
                if ((cameo_da_sinistra && cameo_x <= -CAMEO_EDGE_X) ||
                    (!cameo_da_sinistra && cameo_x >= CAMEO_EDGE_X))
                {
                    cameo_look_idx = (cameo_look_idx + 1) % NUM_CAMEO_LOOKS;
                    spawn_cameo(!cameo_da_sinistra, cameo_look_idx);   // prossimo, dal lato opposto
                }
            }

            cameo_sprite->set_x(cameo_x);
            cameo_anim->update();
        }

        if (bn::keypad::start_pressed())
            return;

        int n = g_rng.get_int();
        n++;
        bg0.set_x(bn::fixed(-0.1) + bg0.x());
        bg1.set_x(bn::fixed(-0.2) + bg1.x());
        bn::core::update();
    }
}