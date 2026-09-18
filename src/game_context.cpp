#include "game_context.h"

GameContext::GameContext(bn::sprite_text_generator& text_gen,
                         bn::regular_bg_ptr&        background0,
                         bn::regular_bg_ptr&        background1,
                         bn::regular_bg_ptr&        background3,
                         int                        selected_skin)
    : rng()
    , camera(bn::camera_ptr::create(0, 0))
    , text_generator(text_gen)
    , bg0(background0)
    , bg1(background1)
    , bg3(background3)
    , the_bau(this)
    , the_ball()
    , the_human( &the_ball)
    , enemies()
    , the_dog(selected_skin, &the_ball, &the_human, &the_bau, enemies)
    , the_helper(this)
    , skin_index(selected_skin)
{
    // --- Camera sul bg di gioco ---
    bg3.set_camera(camera);

    // --- Bau ---
    the_bau.sprite->set_camera(camera);

    // --- Ball ---
    the_ball.player->set_camera(camera);
    the_ball.shadow->set_camera(camera);
    the_ball.shadowRed->set_camera(camera);
    the_ball.player->set_bg_priority(2);

    // --- Human ---
    the_human.player->set_camera(camera);
    the_human.player->set_bg_priority(2);
    the_human.barra_rossa->set_camera(camera);
    the_human.barra_verde->set_camera(camera);

    // --- Dog ---
    the_dog.player->set_camera(camera);
    the_dog.player->set_bg_priority(2);
}
