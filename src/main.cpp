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
#include "bn_regular_bg_items_s1.h"
#include "bn_regular_bg_map_ptr.h"
#include "bn_regular_bg_map_cell_info.h"
#include "bn_memory.h"
#include "round_6x6_sprite_font.h"
#include "bn_log.h"


#include "bn_color.h"
#include "bn_regular_bg_ptr.h"
#include "bn_regular_bg_actions.h"
#include "bn_bg_palette_actions.h"

// Includi l'asset dello sfondo d'introduzione generato nella cartella graphics/
#include "bn_regular_bg_items_intro.h"
#include "bn_regular_bg_items_megadog.h"

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
#include "platform_map.h"
#include <bn_blending.h>
#include "piattaforma.h"
#include "healthbar.h"


namespace
{
    // Resetta in ordine (da sinistra a destra) una serie di bn::optional.
// Chiamare con l'ordine inverso rispetto alla creazione.
    template<typename... Opts>
    void reset_all(Opts&... opts)
    {
        (opts.reset(), ...);
    }
    // Elimina tutti gli oggetti puntati dal contenitore e lo svuota.
    template<typename Vec>
    void delete_all_and_clear(Vec& v)
    {
        for (auto* p : v)
            delete p;
        v.clear();
    }

    // Versione variadica: più contenitori in una sola chiamata, nell'ordine indicato.
    template<typename... Vecs>
    void delete_all(Vecs&... vs)
    {
        (delete_all_and_clear(vs), ...);
    }
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
#include <climits>

    BN_CODE_IWRAM   // opzionale: esegue la funzione dalla IWRAM (più veloce della ROM)
        void update_camera(bn::regular_bg_ptr& bg0, bn::regular_bg_ptr& bg1,
            bn::regular_bg_ptr& foreground, bn::regular_bg_ptr& foregroundfg)
    {
        // Offset costanti del foreground: -(MAP_HALF) - HALF_SCREEN
        static int FG_OFF_X = -(MAP_HALF_W + 120);
        static int FG_OFF_Y = -(MAP_HALF_H + 80);

        static int s_schema = -1;
        static int s_max_x = 0, s_max_y = 0;
        static int s_last_x = INT_MIN, s_last_y = INT_MIN;

        // Ricalcola i limiti solo quando cambia lo schema
        if (s_schema != g_schema)
        {
            const collision_map_info& map = get_collision_map(g_schema);
            s_max_x = map.map_w - 240;
            s_max_y = map.map_h - 160;

            FG_OFF_X = -((map.map_w >> 1) + 120);
            FG_OFF_Y = -((map.map_h >> 1) + 80);


            s_schema = g_schema;
            s_last_x = s_last_y = INT_MIN;   // forza il refresh
        }

        // Tutto in interi: l'hardware lavora a pixel interi
        int cx = g_dog->chr_x.integer() - 120;
        int cy = g_dog->chr_y.integer() - 80;

        if (cx < 0) cx = 0; else if (cx > s_max_x) cx = s_max_x;
        if (cy < 0) cy = 0; else if (cy > s_max_y) cy = s_max_y;

        if (g_shake_ticks > 0)
        {
            // Se puoi, precalcola all'avvio dello shake:
            //   g_shake_amp_q8 = (g_shake_ampiezza << 8) / g_shake_durata;
            // e qui usa: int amp = (g_shake_amp_q8 * g_shake_ticks) >> 8;
            int amp = g_shake_ampiezza * g_shake_ticks / g_shake_durata;
            if (amp < 1) amp = 1;

            cx += (g_shake_ticks & 1) ? amp : -amp;
            cy += ((g_shake_ticks >> 1) & 1) ? (amp >> 1) : -(amp >> 1);
            g_shake_ticks--;
        }

        // Aggiorna solo ciò che è cambiato
        if (cx != s_last_x)
        {
            s_last_x = cx;
            g_camera->set_x(cx);
            bg0.set_x(-(cx >> 2));            // * -0.25
            bg1.set_x(-(cx >> 1));            // * -0.5
            const int fx = FG_OFF_X - cx;     // * -1.0
            foreground.set_x(fx);
            foregroundfg.set_x(fx);
        }

        if (cy != s_last_y)
        {
            s_last_y = cy;
            g_camera->set_y(cy);
            const int fy = FG_OFF_Y - cy;
            foreground.set_y(fy);
            foregroundfg.set_y(fy);
        }
    }
}

void show_intro_screen()
{

    // 1. Crea il background statico dell'intro
    bn::regular_bg_ptr intro_bg = bn::regular_bg_items::intro.create_bg(0, 0);
    bn::regular_bg_ptr mega_bg = bn::regular_bg_items::megadog.create_bg(0, 0);



    intro_bg.set_y(-48);   // scorrimento lento verso sinistra
    mega_bg.set_y(48);   // scorrimento lento verso sinistra



    int FRAMES = 180;

    for (int i = 0; i < 180 * 2; ++i)
    {

        if (i <= FRAMES) {
            bn::fixed intensity = bn::fixed(FRAMES - i) / FRAMES;
            bn::bg_palettes::set_fade(bn::colors::black, intensity);
            bn::sprite_palettes::set_fade(bn::colors::black, intensity);
        }
        intro_bg.set_y(intro_bg.y() + bn::fixed(0.25));   // scorrimento lento verso sinistra

        mega_bg.set_y(mega_bg.y() - bn::fixed(0.125));
        if (bn::keypad::start_pressed())
            break;
        bn::core::update();
    }
    fade_screen(true);

    // --- FADE OUT ---
    // Sfumatura dal colore originale a nero totale (intensità 1.0) in 60 frame (~1 secondo)
    //bn::regular_bg_palette_fade_to_action fade_out(intro_bg.palette(), 60, 1.0);

    //while(!fade_out.done())
    //{
    //    fade_out.update();
    //    bn::core::update();
    //}

    // Quando la funzione termina, intro_bg va fuori dallo scope e la memoria VRAM viene liberata.
}

int main()
{
    bn::core::init();
    if (MUSIC_ON==1)
        bn::music_items::superfrog_title.play(0.25);
    show_intro_screen();

    // bn::sprite_text_generator text_generator(common::variable_8x8_sprite_font);
    bn::sprite_text_generator text_generator(round_font::round_6x6_sprite_font);
    update_text_init(&text_generator);

    //
    g_schema = 1;
    while (true) {
        dog_selection_screen(text_generator);
        // Costruzione oggetti di gioco in ordine di dipendenza
        g_camera.emplace(bn::camera_ptr::create(0, 0));
        g_bau.emplace();
        g_bau_carico.emplace();
        g_enemies.emplace();
        g_items.emplace();
        g_casse.emplace();
        g_piattaforme.emplace();
        g_lava.emplace();
        g_timer.emplace();
        g_dog.emplace();
        g_piattaforme_mobili.emplace();
        g_healthbar.emplace();
        g_trampolini.emplace();


        int arrival_door = -1;   // -1 = prima partita: il cane resta dove nasce
        bool game_over = false;
        if (MUSIC_ON==1)
            bn::music_items::brimble_superfrog_world_1.play(0.25);
        while (!game_over)
        {
            // ------------------------------------------------------------
            // Caricamento dello schema corrente
            // ------------------------------------------------------------
            // Sfondi dello schema: parallasse (bg0, bg1), livello tile e primo piano
            bn::regular_bg_ptr bg0 = create_schema_bg0(g_schema);
            bn::regular_bg_ptr bg1 = create_schema_bg1(g_schema);
            bg0.set_priority(3);
            bg1.set_priority(3);

            bn::regular_bg_ptr foregroundfg = create_schema_foreground(g_schema);

            foregroundfg.set_priority(0);

            carica_collisione_schema(g_schema);   // PRIMA di tutto: porte/nemici/item/piattaforme ne dipendono

            static bn::unique_ptr<platform_map> plat_map;
            plat_map.reset(new platform_map(get_schema_sprite_level_bg_item(g_schema)));

            bn::regular_bg_item item_scrivibile(
                get_schema_sprite_level_bg_item(g_schema).tiles_item(),
                get_schema_sprite_level_bg_item(g_schema).palette_item(),
                plat_map->map_item);
            bn::regular_bg_ptr foreground = item_scrivibile.create_bg(0, 0);
            foreground.set_priority(2);

            // Porte dello schema

            int n_doors = 0;
            const door_def* doorDef = get_schema_doors(g_schema, n_doors);

            bn::vector<door*, MAX_DOORS> doors;
            for (int i = 0; i < n_doors; ++i)
                doors.push_back(new door(doorDef[i]));


            bn::vector<const enemy_def*, MAX_ENEMIES> da_rinascere;

            // Il cane compare sopra la porta di arrivo
            if (arrival_door >= 0 && arrival_door < n_doors)
            {
                g_dog->chr_x = bn::fixed(doors[arrival_door]->chr_x);
                g_dog->chr_y = bn::fixed(doors[arrival_door]->chr_y);
                g_dog->chr_vx = ZERO;
                g_dog->chr_vy = ZERO;
                g_dog->invulnerability = 60;   // i nemici nascono a caso: un attimo di respiro
            }

            // Spawn nemici
            {
                int n_enemies = 0;
                const enemy_def* edefs = get_schema_enemies(g_schema, n_enemies);

                for (int i = 0; i < n_enemies; ++i) {
                    enemy* new_enemy = new enemy(edefs[i]);
                    new_enemy->def_origine = &edefs[i];
                    g_enemies->push_back(new_enemy);

                }
            }
            // Spawn casse
            {
                int n_casse = 0;
                const cassa_def* edefs = get_schema_casse(g_schema, n_casse);

                for (int i = 0; i < n_casse; ++i) {
                    cassa* new_cassa = new cassa(edefs[i]);
                    new_cassa->def_origine = &edefs[i];
                    g_casse->push_back(new_cassa);

                }
            }
            // Spawn oggetti
            {
                int n_items = 0;
                const item_def* defs = get_schema_items(g_schema, n_items);

                g_dog->chiavi_raccolte = 0;
                g_dog->chiavi_richieste = 0;

                for (int i = 0; i < n_items; ++i) {
                    if (defs[i].tipo == ITEM_TIPO_CHIAVE)
                    {
                        g_dog->chiavi_richieste++;
                        if (g_dog->ha_chiave(defs[i].attributo))
                        {
                            g_dog->chiavi_raccolte++;
                            continue;   // già raccolta: non riappare
                        }
                    }
                    item* new_item = new item(defs[i]);
                    g_items->push_back(new_item);
                }
            }
            {
                int n_piattaforme = 0;
                const piattaforma_def* pdefs = get_schema_piattaforme(g_schema, n_piattaforme);
                for (int i = 0; i < n_piattaforme; i++)
                    g_piattaforme->push_back(new piattaforma(pdefs[i], *plat_map, foreground.map()));
            }
            {
                int n_lava = 0;
                const lava_zona_def* ldefs = get_schema_lava(g_schema, n_lava);
                for (int i = 0; i < n_lava; i++)
                    g_lava->push_back(new lava_zona(ldefs[i], *plat_map, foreground.map()));
            }
            {
                int n = 0;
                const piattaforma_mobile_def* defs = get_schema_piattaforme_mobili(g_schema, n);
                for (int i = 0; i < n; i++)
                    g_piattaforme_mobili->push_back(new piattaforma_mobile(defs[i]));
            }
            {
                int n = 0;
                const trampolino_def* defs = get_schema_trampolini(g_schema, n);
                for (int i = 0; i < n; i++)
                    g_trampolini->push_back(new trampolino(defs[i]));
            }

            update_text_clear();

            update_camera(bg0, bg1, foreground, foregroundfg);

            // ------------------------------------------------------------
            // Loop dello schema: termina quando il cane usa una porta
            // ------------------------------------------------------------
            const door_def* used_door = nullptr;

            //        g_boss.emplace(400, 100, 10, enemy_attr::ATTR_FIREBALL | enemy_attr::ATTR_BASH | enemy_attr::ATTR_MELEE | enemy_attr::ATTR_SUMMON);

            // Un frame di gioco (senza bn::core::update)
            auto game_step = [&]()
                {
                    for (piattaforma* p : *g_piattaforme)
                        p->update();

                    for (piattaforma_mobile* pm : *g_piattaforme_mobili)
                        pm->muovi();

                    for (piattaforma_mobile* pm : *g_piattaforme_mobili)
                        pm->trasporta_se_agganciato();

                    g_dog->update();
                    for (trampolino* t : *g_trampolini)
                        t->update();
                    for (cassa* e : *g_casse)
                        e->update();
                    for (enemy* e : *g_enemies)
                        e->update();
                    g_healthbar->update();

                    for (int i = g_enemies->size() - 1; i >= 0; --i)
                    {
                        enemy* e = (*g_enemies)[i];
                        if (e->distrutto)
                        {
                            if ((e->attributo & enemy_attr::ATTR_RESPAWN) && e->def_origine && !da_rinascere.full())
                                da_rinascere.push_back(e->def_origine);   // tornerà quando dog si allontana
                            if (g_healthbar->target == e) {
                                g_healthbar->target = nullptr;
                                g_healthbar->ticks = 0;
                                g_healthbar->sprite->set_visible(false);
                            }
                            for (piattaforma_mobile* pm : *g_piattaforme_mobili)
                                if (pm->nemico_agganciato == e) pm->nemico_agganciato = nullptr;
                            delete e;
                            g_enemies->erase(g_enemies->begin() + i);
                        }
                    }

                    // Respawn: quando il punto di nascita è fuori portata da dog
                    for (int i = da_rinascere.size() - 1; i >= 0; --i)
                    {
                        const enemy_def* d = da_rinascere[i];
                        if (!g_enemies->full() &&
                            bn::abs(bn::fixed(d->x) - g_dog->chr_x) > ENEMY_RESPAWN_RANGE_X)
                        {
                            enemy* nuovo = new enemy(*d);
                            nuovo->def_origine = d;
                            g_enemies->push_back(nuovo);
                            da_rinascere.erase(da_rinascere.begin() + i);
                        }
                    }

                    for (piattaforma_mobile* pm : *g_piattaforme_mobili)
                        pm->verifica_nuovo_aggancio();

                    g_dog->update_animations();

                    g_bau->update();
                    g_bau_carico->update();

                    if (g_boss.has_value())
                        g_boss->update();

                    for (item* e : *g_items)
                        e->update();
                    for (door* d : doors)
                        if (d->update())
                            used_door = &d->info();
                    for (lava_zona* z : *g_lava)
                        z->update();

                    for (int i = g_piattaforme_mobili->size() - 1; i >= 0; --i) {
                        if ((*g_piattaforme_mobili)[i]->distrutta)
                        {
                            delete (*g_piattaforme_mobili)[i];
                            g_piattaforme_mobili->erase(g_piattaforme_mobili->begin() + i);
                        }
                    }
                    update_camera(bg0, bg1, foreground, foregroundfg);

                    update_text_tick();
                    hud_update();
                };

            // Fade-in a gioco attivo: gli sprite prendono la posizione giusta
            // (nel costruttore sono tutti creati nello stesso punto)
            if (arrival_door >= 0)
                fade_screen(false, game_step);

            while (!used_door && !game_over)
            {
                game_step();
                bn::core::update();
                if (g_dog->life <= 0)
                    game_over = true;
            }

            // ------------------------------------------------------------
            // Transizione verso lo schema di destinazione
            // ------------------------------------------------------------

            if (game_over)
                fade_screen_except_dog(true, text_generator);
            else
                fade_screen(true);

            delete_all(*g_enemies,
                *g_items,
                doors,
                *g_trampolini,
                *g_casse,
                *g_piattaforme,
                *g_piattaforme_mobili,
                *g_lava);

            // eventuale bau in volo
            g_bau->ticks = 0;
            g_bau->sprite->set_visible(false);

            g_bau_carico->ticks = 0;
            g_bau_carico->sprite->set_visible(false);

            if (game_over)
                break;   // bg0, bg1, foreground... vengono distrutti qui
            g_schema = used_door->dest_schema;
            arrival_door = used_door->dest_door;
            // foreground e foregroundfg vengono distrutti a fine iterazione e ricreati sopra
        }
        // --- teardown partita ---
        hud_reset();
        update_text_clear();
        g_shake_ticks = 0;

        // ordine inverso rispetto alla creazione; la camera per ultima
        // perché gli sprite ne tengono un riferimento
        // ordine inverso rispetto alla creazione; la camera per ultima
        // perché gli sprite ne tengono un riferimento
        reset_all(g_boss,
            g_healthbar,
            g_trampolini,          // spostato: prima della camera
            g_casse,
            g_piattaforme_mobili,
            g_dog,
            g_timer,
            g_lava,
            g_piattaforme,
            g_items,
            g_enemies,
            g_bau_carico,
            g_bau,
            g_camera);
        // toglie il nero lasciato dal fade
        bn::bg_palettes::set_fade_intensity(0);
        bn::sprite_palettes::set_fade_intensity(0);
    }
}