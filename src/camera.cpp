#include <climits>

#include "bn_core.h"
#include "bn_regular_bg_ptr.h"
#include "bn_camera_ptr.h"

#include "globals.h"
#include "dog.h"
#include "schemi.h"

namespace
{
    // --- Parametri della camera (px) ---
    constexpr int CAM_HALF_W      = 120;
    constexpr int CAM_HALF_H      = 80;

    // Dead zone orizzontale: il cane puo' muoversi di +/- DZ_X dal punto di riferimento senza scroll
    constexpr int DZ_X            = 12;
    // Look-ahead: il punto di riferimento si sposta di LOOK_AHEAD px nella direzione di marcia
    constexpr int LOOK_AHEAD      = 64;
    constexpr int LOOK_STEP       = 1;       // px/frame con cui l'offset insegue il target
    constexpr int CAM_X_MIN_STEP  = 3;       // velocita' minima di inseguimento in X (px/frame)

    // Dead zone verticale (asimmetrica, come SMB3: tollera i salti, segue le cadute)
    constexpr int DZ_Y_UP         = 24;      // px sopra il centro
    constexpr int DZ_Y_DOWN       = 8;       // px sotto il centro
    constexpr int CAM_Y_MIN_STEP  = 3;       // velocita' minima quando il cane esce dalla fascia
    constexpr int CAM_Y_RECENTER  = 1;       // a terra, la camera rientra piano verso il centro

    // Se il cane e' piu' lontano di cosi' dal centro schermo (teletrasporto, respawn): snap
    constexpr int SNAP_X          = 160;
    constexpr int SNAP_Y          = 120;

    inline int clamp_int(int v, int lo, int hi)
    {
        if (lo > hi) return lo;
        if (v < lo) return lo;
        if (v > hi) return hi;
        return v;
    }

    inline int abs_int(int v) { return v < 0 ? -v : v; }
}

// Camera e parallasse: la posizione e' clampata ai bordi della mappa,
// cosi' funziona subito anche dopo un cambio schema.
// Stile SMB3: dead zone + inseguimento a velocita' limitata + look-ahead orizzontale.
void update_camera(bn::regular_bg_ptr& bg0, bn::regular_bg_ptr& bg1,
    bn::regular_bg_ptr& foreground, bn::regular_bg_ptr& foregroundfg)
{
    // Offset costanti del foreground: -(MAP_HALF) - HALF_SCREEN
    static int FG_OFF_X = -(MAP_HALF_W + 120);
    static int FG_OFF_Y = -(MAP_HALF_H + 80);

    static int s_schema = -1;
    static int s_max_x = 0, s_max_y = 0;
    static int s_last_x = INT_MIN, s_last_y = INT_MIN;

    // Stato persistente della camera (senza shake)
    static int s_cam_x = 0, s_cam_y = 0;
    static int s_look = 0;                 // offset di look-ahead corrente, -LOOK_AHEAD..+LOOK_AHEAD

    const int dog_x = g_dog->chr_x.integer();
    const int dog_y = g_dog->chr_y.integer();

    bool snap = false;

    // Ricalcola i limiti solo quando cambia lo schema
    if (s_schema != g_schema)
    {
        const collision_map_info& map = get_collision_map(g_schema);
        s_max_x = (map.map_w > 240) ? map.map_w - 240 : 0;
        s_max_y = (map.map_h > 160) ? map.map_h - 160 : 0;

        FG_OFF_X = -((map.map_w >> 1) + 120);
        FG_OFF_Y = -((map.map_h >> 1) + 80);

        s_schema = g_schema;
        s_last_x = s_last_y = INT_MIN;   // forza il refresh
        snap = true;
    }

    // Il cane e' lontano dalla vista (respawn, porta, ecc.): niente scroll lungo, salta subito
    if (abs_int(dog_x - (s_cam_x + CAM_HALF_W)) > SNAP_X ||
        abs_int(dog_y - (s_cam_y + CAM_HALF_H)) > SNAP_Y)
        snap = true;

    if (snap)
    {
        s_look  = 0;
        s_cam_x = clamp_int(dog_x - CAM_HALF_W, 0, s_max_x);
        s_cam_y = clamp_int(dog_y - CAM_HALF_H, 0, s_max_y);
    }
    else
    {
        // ------------------------------------------------------------
        // Asse X: look-ahead + dead zone, inseguimento a velocita' limitata
        // ------------------------------------------------------------
        // NB: chr_vx.integer() troncherebbe 0.92 a 0 (la friction gira prima della camera): confronto in fixed
        const bool in_movimento = g_dog->chr_vx > bn::fixed(0.1) || g_dog->chr_vx < bn::fixed(-0.1)
            || g_dog->dash_ticks > 0;
        if (in_movimento)
        {
            const int target_look = g_dog->dir * LOOK_AHEAD;
            if (s_look < target_look)      s_look = (s_look + LOOK_STEP > target_look) ? target_look : s_look + LOOK_STEP;
            else if (s_look > target_look) s_look = (s_look - LOOK_STEP < target_look) ? target_look : s_look - LOOK_STEP;
        }

        // Posizione a schermo che il cane deve occupare, +/- dead zone
        // il centro camera sta DAVANTI al cane: il cane appare dalla parte opposta alla marcia
        const int ref_x    = CAM_HALF_W - s_look;
        const int left_x   = s_cam_x + ref_x - DZ_X;
        const int right_x  = s_cam_x + ref_x + DZ_X;

        int desired_x = s_cam_x;
        if (dog_x < left_x)       desired_x = dog_x - (ref_x - DZ_X);
        else if (dog_x > right_x) desired_x = dog_x - (ref_x + DZ_X);

        // velocita' massima: sempre sufficiente a non perdere il cane (dash incluso)
        int max_step_x = (g_dog->chr_vx < ZERO ? -g_dog->chr_vx : g_dog->chr_vx).integer() + 2;
        if (max_step_x < CAM_X_MIN_STEP) max_step_x = CAM_X_MIN_STEP;

        s_cam_x += clamp_int(desired_x - s_cam_x, -max_step_x, max_step_x);
        s_cam_x = clamp_int(s_cam_x, 0, s_max_x);

        // ------------------------------------------------------------
        // Asse Y: dead zone asimmetrica; a terra rientra piano al centro
        // ------------------------------------------------------------
        const int vy        = g_dog->chr_vy.integer();
        const int dog_scr_y = dog_y - s_cam_y;                 // y del cane a schermo
        const int top_y     = CAM_HALF_H - DZ_Y_UP;
        const int bottom_y  = CAM_HALF_H + DZ_Y_DOWN;

        int desired_y = s_cam_y;
        int max_step_y = CAM_Y_RECENTER;

        if (dog_scr_y < top_y)
        {
            // sale (salto, trampolino, scala): segue a velocita' proporzionale alla salita
            desired_y  = dog_y - top_y;
            max_step_y = (-vy + 1 > CAM_Y_MIN_STEP) ? (-vy + 1) : CAM_Y_MIN_STEP;
        }
        else if (dog_scr_y > bottom_y)
        {
            // scende (caduta, ground pound a max_fall): non va perso di vista
            desired_y  = dog_y - bottom_y;
            max_step_y = (vy + 1 > CAM_Y_MIN_STEP) ? (vy + 1) : CAM_Y_MIN_STEP;
        }
        else if (g_dog->onGround && abs_int(dog_scr_y - CAM_HALF_H) > 3)
        {
            // fermo a terra dentro la fascia: riporta con calma il cane verso il centro
            desired_y = dog_y - CAM_HALF_H;
        }

        s_cam_y += clamp_int(desired_y - s_cam_y, -max_step_y, max_step_y);
        s_cam_y = clamp_int(s_cam_y, 0, s_max_y);
    }

    // Posizione finale: lo shake e' solo visivo e non entra nello stato persistente
    int cx = s_cam_x;
    int cy = s_cam_y;

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

    cx = clamp_int(cx, 0, s_max_x);
    cy = clamp_int(cy, 0, s_max_y);

    // Aggiorna solo cio' che e' cambiato
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
