#include "globals.h"
#include "dog.h"
#include "collision.h"
#include "enemy.h"
#include "boss.h"
#include "bau.h"
#include "item.h"
#include "bn_sprite_items_fox1632.h"
#include "bn_log.h"
#include "cassa.h"
#include "trampolino.h"
#include "bn_sound_items.h"
#include "bn_string.h"

BN_CODE_IWRAM void bau::update()
{
    if (ticks2Anim > 0) {
        ticks2Anim--;
        if (ticks2Anim == 0)
            sprite->set_visible(false);
        else
            sprite->set_visible(ticks2Anim % 2);
        return;
    }

    if (ticks == 0)
        return;

    if (chr_vx != 0)
    {
        chr_x += chr_vx;
    }

    bn::fixed reach = hit_half + 8;   // 8 = mezza hitbox del nemico
    for (enemy* enem : *g_enemies)
    {
        if (enem->invulnerability > 0 || enem->is_dead()) continue;

        bool hit = bn::abs(enem->sprite->x() - sprite->x()) < reach &&
            bn::abs(enem->sprite->y() - sprite->y()) < reach;
        if (hit)
        {
            enem->beHitByBark(enem->chr_x < g_dog->chr_x ? DIR_LEFT : DIR_RIGHT, power);
            if (power < bn::fixed(1)) {
                ticks = 0;
                ticks2Anim = 20;
            }
            break;
        }
    }

    if (g_boss.has_value() && g_boss->invulnerability == 0)
    {
        bn::fixed boss_half = g_boss->dimensione / 2;
        bool hit_boss = bn::abs(g_boss->chr_x - chr_x) < hit_half + boss_half &&
            bn::abs(g_boss->chr_y - chr_y) < hit_half + boss_half;
        if (hit_boss)
        {
            int damage = (BAU_DAMAGE + power.multiplication(BAU_DAMAGE)).integer();
            g_boss->subisci_colpo(damage);
            ticks = 0;
            ticks2Anim = 20;
        }
    }

    if (power >= bn::fixed(1)) {
        for (item* switch_item : *g_items)
        {
            if (!switch_item->active || switch_item->tipo != ITEM_TIPO_INTERRUTTORE)
                continue;

            bool hit = bn::abs(switch_item->chr_x - chr_x) < hit_half + switch_item->box_halfdim &&
                bn::abs(switch_item->chr_y - chr_y) < hit_half + switch_item->box_dim;
            if (hit)
            {
                switch_item->commuta_sblocco();
                g_dog->show_str("yeah");
                ticks = 0;
                ticks2Anim = 20;
                break;
            }
        }
    }
    // Oggetti spingibili: il bau li lancia in orizzontale e in verticale
    auto colpisci = [&](spingibile* oggetto)
        {
            if (oggetto->ticks_immune_bau > 0) return;

            bool hit = bn::abs(oggetto->chr_x - chr_x) < hit_half + oggetto->box_halfdim &&
                bn::abs(oggetto->chr_y - chr_y) < hit_half + oggetto->box_dim;
            if (hit)
            {
                oggetto->colpito_da_bau(chr_vx < 0 ? DIR_LEFT : DIR_RIGHT, power);   // via in direzione del bau
                oggetto->ticks_immune_bau = ticks + 1;   // un solo colpo per bau, come i nemici

                ticks = 0;
                ticks2Anim = 20;
            }
        };

    if (g_casse.has_value())
        for (cassa* c : *g_casse)
            if (!c->distrutta)
                colpisci(c);

    if (g_trampolini.has_value())
        for (trampolino* t : *g_trampolini)
            colpisci(t);

    if (ticks > 0) {
        ticks--;
        if (ticks == 0) {
            ticks2Anim = 1;
        }
    }
    sprite->set_x(chr_x - HALF_SCREEN_W);
    sprite->set_y(chr_y - HALF_SCREEN_H);

}