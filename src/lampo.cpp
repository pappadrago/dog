#include "lampo.h"
#include "bn_bg_palette_ptr.h"
#include "bn_colors.h"
#include "bn_optional.h"

static constexpr int LAMPO_FRAMES = 14;

static bn::optional<bn::bg_palette_ptr> s_pal0, s_pal1;
static int s_ticks = 0;

static void applica(bn::fixed intensita)
{
    if (s_pal0) s_pal0->set_fade(bn::colors::white, intensita);
    if (s_pal1) s_pal1->set_fade(bn::colors::white, intensita);
}

void lampo_init(bn::regular_bg_ptr& bg0, bn::regular_bg_ptr& bg1)
{
    s_pal0 = bg0.palette();
    s_pal1 = bg1.palette();
    s_ticks = 0;
}

void lampo_start()
{
    s_ticks = LAMPO_FRAMES;
    applica(1);   // bianco pieno subito, poi decade
}

void lampo_update()
{
    if (s_ticks <= 0) return;
    --s_ticks;
    applica(bn::fixed(s_ticks) / LAMPO_FRAMES);
}

void lampo_reset()
{
    if (s_pal0 || s_pal1) applica(0);
    s_pal0.reset();
    s_pal1.reset();
    s_ticks = 0;
}