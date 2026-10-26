#include <climits>

#include "bn_core.h"
#include "bn_regular_bg_ptr.h"
#include "bn_camera_ptr.h"

#include "globals.h"
#include "dog.h"
#include "schemi.h"

// Camera e parallasse: la posizione e' clampata ai bordi della mappa,
// cosi' funziona subito anche dopo un cambio schema.
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