#include "dog_selection.h"
#include "bn_keypad.h"
#include "bn_core.h"
#include "bn_array.h"
#include "bn_sprite_animate_actions.h"
#include "bn_sprite_items_dog.h"
#include "bn_sprite_items_dog2.h"
#include "bn_sprite_items_dog3.h"
#include "bn_sprite_items_dog4.h"
#include "bn_sprite_items_dog5.h"
#include "bn_sprite_items_bau.h"
#include "bn_regular_bg_ptr.h"
#include "bn_regular_bg_items_cover.h"
#include "globals.h"

int dog_selection_screen(bn::sprite_text_generator& text_generator)
{
    static constexpr int NUM_DOGS = 5;
    static constexpr int SPACING  = 40;
    static constexpr int START_X  = -100;
    static constexpr int DOG_Y    = 32;
    static constexpr int CURSOR_Y = DOG_Y - 8;

    bn::array<bn::optional<bn::sprite_ptr>, NUM_DOGS> sprites;
    bn::array<bn::optional<bn::sprite_animate_action<5>>, NUM_DOGS> anims;

    const bn::sprite_item* items[NUM_DOGS] = {
        &bn::sprite_items::dog, &bn::sprite_items::dog2, &bn::sprite_items::dog3,
        &bn::sprite_items::dog4, &bn::sprite_items::dog5
    };

    for (int i = 0; i < NUM_DOGS; i++)
    {
        int x = START_X + i * SPACING;
        sprites[i] = items[i]->create_sprite(x, DOG_Y, 0);
        anims[i]   = bn::create_sprite_animate_action_forever(
            *sprites[i], 5, items[i]->tiles_item(), (0+i)%5, (1+i)%5, (2+i)%5, (3+i)%5, (4+i)%5);
    }

    bn::sprite_ptr cursor = bn::sprite_items::bau.create_sprite(START_X, CURSOR_Y, 0);

    bn::vector<bn::sprite_ptr, 32> txt_sprites;
    text_generator.set_center_alignment();
    text_generator.generate(0, 60, "SCEGLI IL TUO CANE e PREMI START", txt_sprites);

    int selected   = 0;
    int input_delay = 0;


    bn::regular_bg_ptr bg0 = bn::regular_bg_items::cover.create_bg(0);

    while (true)
    {
        if (input_delay > 0) input_delay--;
        if (input_delay == 0)
        {
            if (bn::keypad::right_pressed() && selected < NUM_DOGS - 1) { selected++; input_delay = 12; }
            else if (bn::keypad::left_pressed() && selected > 0)        { selected--; input_delay = 12; }
        }
        cursor.set_x(START_X + 36 + selected * SPACING);
        for (int i = 0; i < NUM_DOGS; i++) if (anims[i].has_value()) anims[i]->update();
        for (int i = 0; i < NUM_DOGS; i++)
        {
            sprites[i]->set_scale(i == selected ? bn::fixed(1.5) : bn::fixed(1.0));
            sprites[i]->set_y(i == selected ? DOG_Y - 9 : DOG_Y);
        }
        if (bn::keypad::start_pressed()) return selected;

        g_rng.get_int();
        bn::core::update();
    }
}
