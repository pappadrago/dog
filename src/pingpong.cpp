#include "pingpong.h"
#include "percorso_sprite.h"
#include "globals.h"
#include "dog.h"

namespace
{
    constexpr int TREMOLIO_TICKS = 24;
    constexpr int TREMOLIO_AMPIEZZA = 1;
    constexpr int TILE_PAUSA = 19;
}

pingpong::pingpong(const pingpong_def& def, uint32_t sfasamento_ticks) :
    _def(def), _tick_iniziale(g_ostacoli_ticks), _sfasamento_ticks(sfasamento_ticks)
{
    constexpr int S = bn::fixed::scale();

    // --- Sanificazione dei parametri ---
    if (_def.vy_x100 < 1)
        _def.vy_x100 = 1;
    if (_def.gravita_x100 < 1)
        _def.gravita_x100 = 1;
    if (_def.vx_x100 < 0)
        _def.vx_x100 = 0;
    if (_def.sprite_size != PERCORSO_SPRITE_32)
        _def.sprite_size = PERCORSO_SPRITE_16;

    const int vy = _def.vy_x100;
    const int g = _def.gravita_x100;

    // --- Invarianti della traiettoria (calcolate una volta sola) ---
    // Parabola di un lancio verticale con velocita' vy e gravita' g (px/tick, px/tick^2, in x100):
    //   durata del volo   T = 2*vy/g   (arrotondata, minimo 2 tick)
    //   altezza massima   H = vy^2/(2g)
    //   semigittata       R = vx*T/2   (distanza dal centro a ciascun estremo)
    // H e R sono salvati in formato raw di bn::fixed (px * S).
    _durata_volo = (2 * vy + g / 2) / g;
    if (_durata_volo < 2)
        _durata_volo = 2;
    _altezza_raw = int((int64_t(vy) * vy * S) / (int64_t(2) * g * 100));
    _semigittata_raw = int((int64_t(_def.vx_x100) * _durata_volo * S) / 200);

    // --- Invarianti temporali ---
    // Ciclo = andata (volo + pausa) + ritorno (volo + pausa).
    _durata_fase = _durata_volo + int(_def.pausa);
    _durata_ciclo = _durata_fase * 2;

    // Il tremolio occupa gli ultimi ticks della pausa (o tutta la pausa se piu' corta).
    // Cade sempre dopo la fine del volo, quindi non serve altro controllo.
    const int durata_tremolio = int(_def.pausa) < TREMOLIO_TICKS ? int(_def.pausa) : TREMOLIO_TICKS;
    _inizio_tremolio = _durata_fase - durata_tremolio;

    punto();
    if (_def.sprite_size == PERCORSO_SPRITE_32)
    {
        sprite = bn::sprite_items::zombie.create_sprite(
            chr_x - HALF_SCREEN_W, chr_y - HALF_SCREEN_H, 18);
        box_dim = bn::fixed(16);
        box_halfdim = bn::fixed(16);
    }
    else
    {
        sprite = bn::sprite_items::items.create_sprite(
            chr_x - HALF_SCREEN_W, chr_y - HALF_SCREEN_H, TILE_PAUSA);
        box_dim = bn::fixed(8);
        box_halfdim = bn::fixed(8);
    }
    sprite->set_camera(g_camera);
    sprite->set_bg_priority(1);
}

// Calcola posizione (chr_x, chr_y) e tile dell'oggetto in funzione di g_ostacoli_ticks.
//
// Movimento: l'oggetto rimbalza tra due estremi, centro_x - R e centro_x + R, a quota base_y.
// Ogni tratto e' un arco di parabola (salto), seguito da una pausa all'estremo:
//
//   |-- volo (T) --|-- pausa --|-- volo (T) --|-- pausa --|   poi ricomincia
//   |------ andata (sx -> dx) -|----- ritorno (dx -> sx) --|
//
// Lo stato e' una funzione pura del tempo (nessun accumulo tra un frame e l'altro):
// per questo update() puo' saltare i frame con il cane lontano senza perdere la fase.
//
// Trucco: il ritorno e' l'andata percorsa al contrario. Si calcola un solo "progresso"
// u in [0, S] (S = 1.0 in fixed raw), che al ritorno viene specchiato (S - p):
//   x = centro_x - R + 2R*u        (da sinistra a destra al crescere di u)
//   y = base_y - H*4u(1-u)         (parabola simmetrica: vale uguale in entrambi i versi)
void pingpong::punto()
{
    constexpr int S = bn::fixed::scale();

    // Posizione nel ciclo completo e identificazione della fase.
    const int tempo = int((g_ostacoli_ticks - _tick_iniziale + _sfasamento_ticks) % _durata_ciclo);
    const bool ritorno = tempo >= _durata_fase;
    const int t = ritorno ? tempo - _durata_fase : tempo;   // tick dentro la fase, [0, durata_fase)
    const bool in_pausa = t >= _durata_volo;

    // Progresso del volo p in [0, S]; durante la pausa resta fermo all'estremo (p = S).
    const int tt = in_pausa ? _durata_volo : t;
    const int p = int((int64_t(tt) * S) / _durata_volo);
    const int u = ritorno ? S - p : p;                      // 0 = estremo sinistro, S = destro

    // Orizzontale: da -R a +R rispetto al centro.
    const int64_t dx = (int64_t(2) * _semigittata_raw * u) / S - _semigittata_raw;

    // Verticale: u4 = 4u(1-u) vale 0 agli estremi e S (=1.0) a meta' volo.
    const int u4 = int((int64_t(4) * u * (S - u)) / S);
    const int64_t salita = (int64_t(_altezza_raw) * u4) / S;

    // Arrotondo a pixel interi (l'hardware lavora a pixel interi, evita jitter sub-pixel)
    chr_x = bn::fixed((bn::fixed(_def.centro_x) + bn::fixed::from_data(int(dx))).round_integer());
    chr_y = bn::fixed((bn::fixed(_def.base_y) - bn::fixed::from_data(int(salita))).round_integer());

    // Tremolio: solo negli ultimi tick della pausa, come avviso prima della ripartenza.
    if (t >= _inizio_tremolio)
        chr_y += ((g_ostacoli_ticks >> 1) & 1) ? TREMOLIO_AMPIEZZA : -TREMOLIO_AMPIEZZA;

    // Il costruttore chiama punto() prima di creare lo sprite.
    aggiorna_sprite_percorso(sprite, _def.sprite_size, in_pausa);
}

void pingpong::update()
{
    if (bn::abs(g_dog->chr_x - chr_x) > SCREEN_W ||
        bn::abs(g_dog->chr_y - chr_y) > SCREEN_H)
        return;   // cane lontano: non aggiorno (punto() dipende solo dal tempo, la fase resta corretta)

    punto();
    sprite->set_x(chr_x - HALF_SCREEN_W);
    sprite->set_y(chr_y - HALF_SCREEN_H);
    controlla_cane();
}

void pingpong::controlla_cane()
{
    if (g_dog->invulnerability > 0)
        return;

    if (bn::abs(g_dog->chr_x - chr_x) < g_dog->box_halfdim + box_halfdim &&
        bn::abs(g_dog->chr_y - chr_y) < g_dog->box_dim + box_dim)
    {
        g_dog->dog_damage_and_jump(_def.danno, g_dog->chr_x < chr_x);
    }
}