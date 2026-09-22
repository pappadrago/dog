#include "hud.h"
#include "bn_format.h"
#include "bn_string.h"
#include "bn_sprite_ptr.h"
#include "bn_vector.h"

static constexpr int SHOW_FRAMES = 120;
static constexpr int FADE_FRAMES = 60;

// --- Layout HUD (coordinate schermo, origine al centro: x -120..120, y -80..80) ---
// Se i numeri risultano troppo vicini alle etichette basta ritoccare queste costanti:
// il font a larghezza variabile rende impossibile calcolarle a priori.
static constexpr int HUD_Y             = -76;
static constexpr int HUD_LIFE_LABEL_X  = -114;   // etichetta "VITA", allineata a sinistra
static constexpr int HUD_LIFE_NUM_X    = -82;    // bordo destro del numero energia

static constexpr int HUD_SCORE_LABEL_X = 75;     // bordo destro dell'etichetta "PUNTI"
static constexpr int HUD_SCORE_NUM_X   = 119;    // bordo destro del punteggio
static constexpr int HUD_LIFE_DIGITS   = 3;     // energia con zeri iniziali: 100, 075, 005
static constexpr int HUD_LIFE_MAX      = 999;
static constexpr int HUD_SCORE_DIGITS  = 6;
static constexpr int HUD_SCORE_MAX     = 999999;

static constexpr int HUD_LOW_LIFE      = 25;     // sotto questa soglia il numero lampeggia piano
static constexpr int HUD_FLASH_FRAMES  = 40;     // lampeggio veloce dopo un danno
static constexpr int HUD_ROLL_DIV      = 6;      // velocita' contatore: ~1/6 della differenza per frame

static bn::sprite_text_generator* s_generator = nullptr;
static bn::vector<bn::sprite_ptr, 24> s_sprites;
static int s_timer = 0;
static int s_frame = 0;

// --- Stato HUD ---
static bn::vector<bn::sprite_ptr, 8> s_life_label;
static bn::vector<bn::sprite_ptr, 4> s_life_digits;
static bn::vector<bn::sprite_ptr, 8> s_score_label;
static bn::vector<bn::sprite_ptr, 8> s_score_digits;
static bool s_hud_ready   = false;
static int  s_life_shown  = 0;
static int  s_life_target = 0;
static int  s_score_shown = 0;
static int  s_score_target = 0;
static int  s_flash       = 0;
static int  s_hud_frame   = 0;

void update_text_init(bn::sprite_text_generator* gen)
{
    s_generator = gen;

    // Il testo deve stare davanti a tutto: sfondi (priorita' 0-3) e sprite di gioco.
    // Priorita' 0 = davanti; z_order negativo = davanti agli altri sprite di pari priorita'.
    if (s_generator)
    {
        s_generator->set_bg_priority(0);
        s_generator->set_z_order(-32);
    }
}

void update_text_clear()
{
    s_sprites.clear();
    s_timer = 0;
}

void update_text_tick()
{
    s_frame++;
    if (s_timer <= 0 || s_sprites.empty()) return;
    s_timer--;
    if (s_timer <= 0) { s_sprites.clear(); return; }
    if (s_timer <= FADE_FRAMES)
    {
        int soglia = (s_timer * 4) / FADE_FRAMES;
        bool visibile = (s_frame % 4) < soglia;
        for (auto& s : s_sprites) s.set_visible(visibile);
    }
}

// ---------------------------------------------------------------------------
// HUD
// ---------------------------------------------------------------------------

namespace
{
    // Fa un passo da shown verso target: 1/HUD_ROLL_DIV della differenza, almeno 1
    int approach(int shown, int target)
    {
        int diff = target - shown;
        if (diff == 0)
            return shown;

        int step = diff / HUD_ROLL_DIV;
        if (step == 0)
            step = diff > 0 ? 1 : -1;
        return shown + step;
    }

    // Testo allineato a destra: la cifra delle unita' resta ferma mentre il numero scorre
    void draw_right(bn::ivector<bn::sprite_ptr>& out, int x, int y, const bn::string_view& text)
    {
        out.clear();
        s_generator->set_right_alignment();
        s_generator->generate(x, y, text, out);
        s_generator->set_left_alignment();
    }

    void draw_left(bn::ivector<bn::sprite_ptr>& out, int x, int y, const bn::string_view& text)
    {
        out.clear();
        s_generator->set_left_alignment();
        s_generator->generate(x, y, text, out);
    }

    // Numero con zeri iniziali a larghezza fissa (es. 75 su 3 cifre -> "075")
    bn::string<8> padded(int value, int digits)
    {
        bn::string<8> txt;
        int div = 1;
        for (int i = 1; i < digits; ++i)
            div *= 10;
        for (int i = 0; i < digits; ++i)
        {
            txt.push_back(char('0' + (value / div) % 10));
            div /= 10;
        }
        return txt;
    }

    void draw_life()
    {
        draw_right(s_life_digits, HUD_LIFE_NUM_X, HUD_Y, padded(s_life_shown, HUD_LIFE_DIGITS));
    }

    void draw_score()
    {
        draw_right(s_score_digits, HUD_SCORE_NUM_X, HUD_Y, padded(s_score_shown, HUD_SCORE_DIGITS));
    }

    void set_visible(bn::ivector<bn::sprite_ptr>& v, bool visible)
    {
        for (auto& s : v)
            s.set_visible(visible);
    }
}

void hud_reset()
{
    s_life_label.clear();
    s_life_digits.clear();
    s_score_label.clear();
    s_score_digits.clear();
    s_hud_ready = false;
    s_flash = 0;
}

void hud_update(int life, int score)
{
    if (!s_generator)
        return;

    if (life < 0)  life = 0;
    if (life > HUD_LIFE_MAX) life = HUD_LIFE_MAX;
    if (score < 0) score = 0;
    if (score > HUD_SCORE_MAX) score = HUD_SCORE_MAX;

    s_hud_frame++;

    // Prima chiamata: etichette fisse e contatori che partono dal valore reale
    if (!s_hud_ready)
    {
        draw_left(s_life_label, HUD_LIFE_LABEL_X, HUD_Y, "H:");

        s_score_label.clear();
        s_generator->set_right_alignment();
        s_generator->generate(HUD_SCORE_LABEL_X, HUD_Y, "P:", s_score_label);
        s_generator->set_left_alignment();

        s_life_shown = s_life_target = life;
        s_score_shown = s_score_target = score;
        draw_life();
        draw_score();
        s_hud_ready = true;
    }

    // Energia persa: lampeggio veloce del numero
    if (life < s_life_target)
        s_flash = HUD_FLASH_FRAMES;

    s_life_target  = life;
    s_score_target = score;

    // Il contatore rincorre il valore reale; si rigenerano gli sprite solo se la cifra cambia
    int life_next = approach(s_life_shown, s_life_target);
    if (life_next != s_life_shown)
    {
        s_life_shown = life_next;
        draw_life();
    }

    int score_next = approach(s_score_shown, s_score_target);
    if (score_next != s_score_shown)
    {
        s_score_shown = score_next;
        draw_score();
    }

    // Lampeggio dell'energia: veloce dopo il danno, lento se quasi finita
    bool life_visible = true;
    if (s_flash > 0)
    {
        s_flash--;
        life_visible = ((s_flash / 2) % 2) == 0;
    }
    else if (s_life_target > 0 && s_life_target <= HUD_LOW_LIFE)
    {
        life_visible = ((s_hud_frame / 16) % 2) == 0;
    }
    set_visible(s_life_digits, life_visible);
}
