#include "bn_core.h"
#include "bn_random.h"
#include "bn_keypad.h"
#include "bn_camera_ptr.h"
#include "bn_music_items.h"
#include "bn_bg_palettes.h"
#include "bn_sprite_palettes.h"
#include "bn_colors.h"
#include "bn_sprite_text_generator.h"
#include "bn_regular_bg_ptr.h"
#include "bn_regular_bg_map_cell_info.h"
#include "bn_regular_bg_items_country.h"
#include "common_variable_8x16_sprite_font.h"
#include "bn_log.h"

#include "globals.h"
#include "game_constants.h"
#include "hud.h"
#include "bau.h"
#include "enemy.h"
#include "item.h"
#include "door.h"
#include "dog.h"
#include "dog_selection.h"
#include "game_timer.h"
#include "schemi.h"
#include "enemies_table.h"
#include "items_table.h"


namespace
{
    // Dissolvenza verso il nero (true) o dal nero (false).
    // Usa il fade delle palette: agisce su tutti gli sprite e gli sfondi,
    // senza dover abilitare il blending oggetto per oggetto.
    template<typename Step>
    void fade_screen(bool to_black, Step&& step)
    {
        static constexpr int FRAMES = 60;

        for (int i = 1; i <= FRAMES; ++i)
        {
            step();

            bn::fixed intensity = to_black ? bn::fixed(i) / FRAMES
                : bn::fixed(FRAMES - i) / FRAMES;

            bn::bg_palettes::set_fade(bn::colors::black, intensity);
            bn::sprite_palettes::set_fade(bn::colors::black, intensity);
            bn::core::update();
        }
    }

    // Versione a gioco fermo (usata per il fade-out)
    void fade_screen(bool to_black)
    {
        fade_screen(to_black, []() {});
    }

    // Camera e parallasse: la posizione e' clampata ai bordi della mappa,
    // cosi' funziona subito anche dopo un cambio schema.
    void update_camera(bn::regular_bg_ptr& bg0, bn::regular_bg_ptr& bg1,
        bn::regular_bg_ptr& foreground, bn::regular_bg_ptr& foregroundfg)
    {
        bn::fixed cam_x = g_dog->chr_x - HALF_SCREEN_W;
        bn::fixed cam_y = g_dog->chr_y - HALF_SCREEN_H;
        const collision_map_info& map = get_collision_map(g_schema);
        if (cam_x < 0) cam_x = 0;
        if (cam_x > bn::fixed(map.map_w) - SCREEN_W) cam_x = bn::fixed(map.map_w) - SCREEN_W;
        if (cam_y < 0) cam_y = 0;
        if (cam_y > bn::fixed(map.map_h) - SCREEN_H) cam_y = bn::fixed(map.map_h) - SCREEN_H;

        g_camera->set_x(cam_x.integer());
        g_camera->set_y(cam_y.integer());

        bg0.set_x(cam_x * bn::fixed(-0.25));
        bg1.set_x(cam_x * bn::fixed(-0.5));

        foreground.set_x(cam_x * bn::fixed(-1.0) - bn::fixed(MAP_HALF_W) - HALF_SCREEN_W);
        foreground.set_y(cam_y * bn::fixed(-1.0) - bn::fixed(MAP_HALF_H) - HALF_SCREEN_H);
        foregroundfg.set_x(foreground.x());
        foregroundfg.set_y(foreground.y());
    }
}

int main()
{
    bn::core::init();

    bn::sprite_text_generator text_generator(common::variable_8x16_sprite_font);
    update_text_init(&text_generator);

    //bn::music_items::qwak.play(0.25);

    int skin_selezionato = dog_selection_screen(text_generator);

    // Costruzione oggetti di gioco in ordine di dipendenza
    g_camera.emplace(bn::camera_ptr::create(0, 0));
    g_bau.emplace();
    g_enemies.emplace();
    g_items.emplace();
    g_timer.emplace();
    g_dog.emplace(skin_selezionato);

    g_schema = 1;
    int arrival_door = -1;   // -1 = prima partita: il cane resta dove nasce

    while (true)
    {
        // ------------------------------------------------------------
        // Caricamento dello schema corrente
        // ------------------------------------------------------------
        // Sfondi dello schema: parallasse (bg0, bg1), livello tile e primo piano
        bn::regular_bg_ptr bg0 = create_schema_bg0(g_schema);
        bn::regular_bg_ptr bg1 = create_schema_bg1(g_schema);
        bg0.set_priority(3);
        bg1.set_priority(3);

        bn::regular_bg_ptr foreground = create_schema_bg(g_schema);
        bn::regular_bg_ptr foregroundfg = create_schema_fg(g_schema);
        foregroundfg.set_priority(0);
        foreground.set_priority(2);

        // Porte dello schema
        const door_list& dl = get_schema_doors(g_schema);
        bn::vector<door*, MAX_DOORS> doors;
        for (int i = 0; i < dl.count; ++i)
            doors.push_back(new door(dl.data[i]));

        // Il cane compare sopra la porta di arrivo
        if (arrival_door >= 0 && arrival_door < dl.count)
        {
            g_dog->chr_x = bn::fixed(dl.data[arrival_door].x);
            g_dog->chr_y = bn::fixed(dl.data[arrival_door].y);
            g_dog->chr_vx = bn::fixed(0);
            g_dog->chr_vy = bn::fixed(0);
            g_dog->invulnerability = 60;   // i nemici nascono a caso: un attimo di respiro
        }

        // Spawn nemici
        {
            int n_enemies = 0;
            const enemy_def* edefs = get_schema_enemies(g_schema, n_enemies);

            for (int i = 0; i < n_enemies; ++i) {
                enemy* new_enemy = new enemy(edefs[i]);
                g_enemies->push_back(new_enemy);
            }
        }
        // Spawn oggetti
        {
            int n_items = 0;
            const item_def* defs = get_schema_items(g_schema, n_items);

            g_dog->chiavi_raccolte = 0;
            g_dog->chiavi_richieste = 0;

            for (int i = 0; i < n_items; ++i) {
                item* new_item = new item(defs[i]);
                g_items->push_back(new_item);
                if (defs[i].tipo == ITEM_TIPO_CHIAVE)
                    g_dog->chiavi_richieste++;
            }
        }
        update_text_clear();

        update_camera(bg0, bg1, foreground, foregroundfg);

        // ------------------------------------------------------------
        // Loop dello schema: termina quando il cane usa una porta
        // ------------------------------------------------------------
        const door_info* used_door = nullptr;

        // Un frame di gioco (senza bn::core::update)
        auto game_step = [&]()
            {
                g_dog->update();
                g_bau->update();

                for (enemy* e : *g_enemies)
                    e->update();
                for (item* e : *g_items)
                    e->update();
                for (door* d : doors)
                    if (d->update())
                        used_door = &d->info();

                update_camera(bg0, bg1, foreground, foregroundfg);

                update_text_tick();
                hud_update(g_dog->life, g_dog->score, g_dog->chiavi_richieste - g_dog->chiavi_raccolte);


            };

        // Fade-in a gioco attivo: gli sprite prendono la posizione giusta
        // (nel costruttore sono tutti creati nello stesso punto)
        if (arrival_door >= 0)
            fade_screen(false, game_step);

        while (!used_door)
        {
            game_step();
            bn::core::update();
        }

        // ------------------------------------------------------------
        // Transizione verso lo schema di destinazione
        // ------------------------------------------------------------
        fade_screen(true);

        for (enemy* e : *g_enemies)
            delete e;
        g_enemies->clear();

        for (item* i : *g_items)
            delete i;
        g_items->clear();

        for (door* d : doors)
            delete d;
        doors.clear();

        // eventuale bau in volo
        g_bau->ticks = 0;
        g_bau->sprite->set_visible(false);

        g_schema = used_door->dest_schema;
        arrival_door = used_door->dest_door;
        // foreground e foregroundfg vengono distrutti a fine iterazione e ricreati sopra
    }
}
