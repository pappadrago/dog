#include "hud.h"
#include "dog.h"
#include "bn_format.h"
#include "bn_string.h"
#include "common_variable_8x16_sprite_font.h"

static constexpr int SHOW_FRAMES = 120;
static constexpr int FADE_FRAMES = 60;

static bn::sprite_text_generator* s_generator = nullptr;
static bn::vector<bn::sprite_ptr, 24> s_sprites;
static int s_timer = 0;
static int s_frame = 0;

const bn::string_view testi[] = {
    "PREMI START",
    "BRAVO!", "BRAVO CANE!", "ALLA GRANDE!", "QUA LA ZAMPA!",
    "OTTIMO!", "CHE CAMPIONE!", "BRAVISSIMO!", "SEI IL MIGLIORE!",
    "INCREDIBILE!", "CHE SALTO!", "VELOCISSIMO!", "INARRESTABILE!",
    "FANTASTICO!", "GRAZIE!", "BRAVO AMICO!", "PERFETTO!",
    "CHE NASO!", "SEI IN FORMA!", "ANCORA ANCORA!", "HAI VINTO!",
    "SUPER CANE!", "MITICO!", "CHE CLASSE!", "PROPRIO BRAVO!",
    "RIPORTAMELA!", "TRE...DUE...UNO...", "DAI VELOCE!", "E ORA...VAI!",
    "HOP HOP HOP!", "PRONTO?", "CI SEI?", "STAI FERMO!",
    "OCCHIO LA!", "GUARDA BENE!", "ATTENZIONE!", "CONCENTRATI!",
    "TIENITI PRONTO!", "VIA!!!", "ADESSO!", "ORA!!!",
    "PRENDILA!", "ECCOLA!", "ZOMPACI SU!", "NON PERDERLA!",
    "DRITTO LI!", "FORZA!!!", "SALTA!!!", "CORRI!!!", "LA VEDI?"
};

const int NUM_TESTI = sizeof(testi) / sizeof(testi[0]);

void update_text(int status, bn::sprite_text_generator* gen)
{
    if (gen)
    {
        s_generator = gen;
        return;
    }
    s_sprites.clear();
    if (!s_generator || status < 0 || status >= NUM_TESTI)
        return;

    s_generator->set_left_alignment();
    s_generator->generate(-100, 10, testi[status], s_sprites);
    s_timer = SHOW_FRAMES + FADE_FRAMES;
}

void update_text(int status, dog* _dog, bn::sprite_text_generator* gen)
{
    if (gen)
    {
        s_generator = gen;
        return;
    }
    s_sprites.clear();
    if (!s_generator || status < 0 || status >= NUM_TESTI)
        return;

    s_generator->set_left_alignment();
    bn::string<32> text;
    text.append(bn::format<24>("Schema {}. Premi START.", _dog->schema));
    s_generator->generate(-100, 10, text, s_sprites);
    s_timer = SHOW_FRAMES + FADE_FRAMES;
}

void update_text(int status)
{
    s_sprites.clear();
    if (!s_generator || status < 0 || status >= NUM_TESTI)
        return;

    s_generator->set_left_alignment();
    s_generator->generate(-100, 10, testi[status], s_sprites);
    s_timer = SHOW_FRAMES + FADE_FRAMES;
}

void update_text_tick()
{
    s_frame++;

    if (s_timer <= 0 || s_sprites.empty())
        return;

    s_timer--;

    if (s_timer <= 0)
    {
        s_sprites.clear();
        return;
    }

    if (s_timer <= FADE_FRAMES)
    {
        int soglia = (s_timer * 4) / FADE_FRAMES;
        bool visibile = (s_frame % 4) < soglia;
        for (auto& s : s_sprites)
            s.set_visible(visibile);
    }
}
