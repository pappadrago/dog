#include "globals.h"
#include "enemy.h"
#include "bau.h"
#include "dog.h"
#include "item.h"
#include "schemi.h"
#include "healthbar.h"
#include "bn_random.h"
#include "bn_sprite_items_weapons.h"
#include "bn_sprite_items_enemies.h"
#include "bn_sprite_items_king.h"
#include "bn_sprite_items_enemies2.h"
#include "bn_math.h"
#include "bn_log.h"

BN_CODE_IWRAM void enemy::throw_aimed_projectile(uint8_t tipoProjec, uint8_t tileIndex,
    bool ignoreGravity, bool ignoreWalls,
    bn::fixed speed)
{
    if (bomba) return;   // un colpo alla volta per nemico

    bn::fixed dx = g_dog->chr_x - chr_x;
    if (bn::abs(dx) > SCREEN_W) return;      // cane troppo lontano

    dir = (dx > 0) ? DIR_RIGHT : DIR_LEFT;

    bn::fixed vx, vy, angle = ZERO;
    bool aim = has(ATTR_AIM);

    if (aim && dx != ZERO)
    {
        // Mira a tiro: proiettile punta verso il cane (precisione intera, come in boss.cpp).
        bn::fixed dy = g_dog->chr_y - chr_y;
        angle = bn::degrees_atan2(dy.integer(), dx.integer());

        vx = bn::degrees_lut_cos_safe(angle).multiplication(speed);
        vy = bn::degrees_lut_sin_safe(angle).multiplication(speed);
        angle = -angle;
    }
    else
    {
        // Tiro dritto in orizzontale, senza verticale.
        vx = speed.multiplication(dir);
        vy = ZERO;

    }

    // L'angolo di rotazione del sprite segue la traiettoria:
    //   0° = rivolto a destra, quindi rotazione = -90° - angle.
    bomb_def def;
    def.tipoProjec = tipoProjec;
    def.x = chr_x;
    def.y = chr_y + enemy_config::SPAWN_Y_OFFSET;
    def.vx = vx;
    def.vy = vy;

    def.ignoreGravity = ignoreGravity;
    def.ignoreWalls = ignoreWalls;
    def.max_bounces = 0;
    def.ttl = enemy_config::PROJECTILE_LIFETIME;
    def.tileIndex = tileIndex;
    def.angle = angle;

    bomba.emplace(def);
}
