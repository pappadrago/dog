#include "snake.h"
#include "bomb.h"
#include "globals.h"
#include "dog.h"
#include "collision.h"
#include "healthbar.h"
#include "bn_math.h"
#include "bn_sound_items.h"
#include "bn_sprite_items_enemies2.h"

// Tile dello spritesheet "snake" (16x16): adatta agli indici reali
static constexpr int TILE_TESTA   = 12;
static constexpr int TILE_CORPO_A = 13;
static constexpr int TILE_CORPO_B = 14;
static constexpr int TILE_CODA    = 15;

// Stessi valori di enemy_config (enemy.cpp) per le fireball dei nemici
static constexpr int       FIREBALL_TILE  = 23;
static constexpr int       FIREBALL_TTL   = 120;
static constexpr bn::fixed FIREBALL_SPEED = bn::fixed(1.5);

static constexpr bn::fixed SOGLIA_DIR     = bn::fixed(0.05);

static bn::fixed wrap360(bn::fixed a)
{
    while (a >= 360) a -= 360;
    while (a < 0)    a += 360;
    return a;
}

static void orient_segment(bn::sprite_ptr& sprite, bn::fixed dx, bn::fixed dy)
{
    int dx_scaled = (dx * 16).integer();
    int dy_scaled = (dy * 16).integer();
    if (!dx_scaled && !dy_scaled)
        return;

    bool flip = dx_scaled < 0;
    int angle_y = flip ? dy_scaled : -dy_scaled;
    int angle_x = flip ? -dx_scaled : dx_scaled;
    bn::fixed angle = bn::degrees_atan2(angle_y, angle_x);

    sprite.set_horizontal_flip(flip);
    sprite.set_rotation_angle_safe(angle);
}

// Lunghezza approssimata del percorso: serve a tradurre px in gradi di fase
static bn::fixed calcola_perimetro(const snake_def& d)
{
    bn::fixed a(d.semiasse_x), b(d.semiasse_y);
    switch (d.percorso)
    {
    case SNAKE_RETTANGOLO:
    case SNAKE_OTTO:
        return (a + b) * 4;
    default:   // ellisse: pi * (a + b), esatta per il cerchio
        return (a + b).multiplication(bn::fixed(3.14159));
    }
}

snake::snake(const snake_def& d) : def(d)
{
    life = d.life;
    _dying = life <= 0;
    _perimetro = calcola_perimetro(d);
    _step  = bn::fixed(SNAKE_SPACING_PX * 360).division(_perimetro);
    _delta = bn::fixed(d.velocita_x10 * 36).division(_perimetro);   // (v/10) * 360 / perimetro
    _sgn   = d.oraria ? 1 : -1;
    _fase  = wrap360(bn::fixed(d.fase_iniziale));
    _fire_cd = d.delay;

    calcola_posizioni();
    chr_x = _x[0];
    chr_y = _y[0];

    for (int k = 0; k < SNAKE_SEGMENTS; ++k)
    {
        _segment_life[k] = d.life;
        int tile = (k == 0) ? TILE_TESTA
                 : (k == SNAKE_SEGMENTS - 1) ? TILE_CODA
                 : ((k & 1) ? TILE_CORPO_A : TILE_CORPO_B);

        bn::sprite_ptr s = bn::sprite_items::enemies2.create_sprite(
            _x[k] - HALF_SCREEN_W, _y[k] - HALF_SCREEN_H, tile);
        s.set_camera(g_camera);
        s.set_bg_priority(1);
        s.set_z_order(k == 0 ? 0 : 1);
        _segs.push_back(bn::move(s));
    }
}

// Posizione sul percorso per una data fase (gradi 0..360)
void snake::punto(bn::fixed fase, bn::fixed& x, bn::fixed& y) const
{
    bn::fixed a(def.semiasse_x), b(def.semiasse_y);
    bn::fixed cx(def.centro_x),  cy(def.centro_y);

    switch (def.percorso)
    {
    case SNAKE_RETTANGOLO:
    {
        // ascissa curvilinea: velocita' costante anche con lati di lunghezza diversa
        bn::fixed s = fase.multiplication(_perimetro).division(360);
        bn::fixed w = a * 2, h = b * 2;
        if (s < w)        { x = cx - a + s;     y = cy - b; }
        else {
            s -= w;
            if (s < h)    { x = cx + a;         y = cy - b + s; }
            else {
                s -= h;
                if (s < w){ x = cx + a - s;     y = cy + b; }
                else      { x = cx - a;         y = cy + b - (s - w); }
            }
        }
        break;
    }
    case SNAKE_OTTO:
        x = cx + a.multiplication(bn::degrees_lut_sin_safe(fase));
        y = cy + b.multiplication(bn::degrees_lut_sin_safe(wrap360(fase * 2)));
        break;
    default:
        x = cx + a.multiplication(bn::degrees_lut_cos_safe(fase));
        y = cy + b.multiplication(bn::degrees_lut_sin_safe(fase));
        break;
    }
}

// Il percorso e' deterministico: il segmento k sta dove la testa era k*_step gradi fa.
// Niente buffer di storico, solo 16 valutazioni da LUT per frame.
void snake::calcola_posizioni()
{
    for (int k = 0; k < _segment_count; ++k)
        punto(wrap360(_fase - _step * (k * _sgn)), _x[k], _y[k]);
}

void snake::update()
{
    if (is_dead())
    {
        update_death();
        return;
    }

    if (invulnerability > 0)
        invulnerability--;

    bn::fixed old_head = _x[0];
    bn::fixed old_tail = _x[_segment_count - 1];

    _fase = wrap360(_fase + _delta * _sgn);
    calcola_posizioni();
    chr_x = _x[0];
    chr_y = _y[0];

    bn::fixed dh = _x[0] - old_head;
    if (dh > SOGLIA_DIR)       dir = DIR_RIGHT;
    else if (dh < -SOGLIA_DIR) dir = DIR_LEFT;

    bn::fixed dt = _x[_segment_count - 1] - old_tail;
    if (dt > SOGLIA_DIR)       _dir_coda = DIR_RIGHT;
    else if (dt < -SOGLIA_DIR) _dir_coda = DIR_LEFT;

    _anim++;
    aggiorna_sprite();

    if (_fire_cd > 0)
        _fire_cd--;
    else if (spara())
        _fire_cd = def.fireball_period;

    check_dog();
}

void snake::aggiorna_sprite()
{
    bool anima_corpo = (_anim & 7) == 0;
    bool visibile = invulnerability == 0 || (invulnerability % 2);

    for (int k = 0; k < _segment_count; ++k)
    {
        bn::sprite_ptr& s = _segs[k];
        s.set_x(_x[k] - HALF_SCREEN_W);
        s.set_y(_y[k] - HALF_SCREEN_H);
        s.set_visible(visibile);

        int tile = k == 0 ? TILE_TESTA
                 : k == _segment_count - 1 ? TILE_CODA
                 : (((_anim >> 3) + k) & 1) ? TILE_CORPO_A : TILE_CORPO_B;
        if (k == 0 || k == _segment_count - 1 || anima_corpo)
            s.set_tiles(bn::sprite_items::enemies2.tiles_item(), tile);
    }

    bn::fixed head_dx = _x[0] - _x[1];
    bn::fixed head_dy = _y[0] - _y[1];
    bn::fixed tail_dx = _x[_segment_count - 2] - _x[_segment_count - 1];
    bn::fixed tail_dy = _y[_segment_count - 2] - _y[_segment_count - 1];
    orient_segment(_segs[0], head_dx, head_dy);
    orient_segment(_segs[_segment_count - 1], tail_dx, tail_dy);

    for (int k = _segment_count; k < SNAKE_SEGMENTS; ++k)
        _segs[k].set_visible(false);
}

// Fireball SENZA aim: dritta in orizzontale, nel verso in cui guarda la testa
bool snake::spara()
{
    if (bn::abs(g_dog->chr_x - chr_x) > SCREEN_W)   // fuori schermo: non sprecare uno slot
        return false;

    bomb_def d;
    d.tipoProjec    = BOMB_MINA ;
    d.x             = chr_x + bn::fixed(8 * dir);
    d.y             = chr_y;
    d.vx            = FIREBALL_SPEED * dir;
    d.vy            = ZERO;
    d.ignoreGravity = false;
    d.ignoreWalls   = false;
    d.max_bounces   = 0;
    d.tileIndex     = FIREBALL_TILE;
    d.angle         = ZERO;   // 0 = niente rotazione, il flip segue il segno di vx

    if (!spawn_bomb(d))
        return false;
    bn::sound_items::fire_lighting.play();
    return true;
}

// Contatto col cane: vale per qualunque segmento
void snake::check_dog()
{
    if (g_dog->invulnerability > 0)
        return;

    for (int k = 0; k < SNAKE_SEGMENTS; ++k)
    {
        if (check_collision_16(_segs[k]))
        {
            g_dog->dog_damage_and_jump(contact_damage, g_dog->chr_x < _x[k]);
            return;
        }
    }
}

int snake::overlapping_segment(bn::fixed x, bn::fixed y, bn::fixed reach) const
{
    int first = _segment_count > 2 ? 1 : 0;
    int last = _segment_count > 2 ? _segment_count - 1 : _segment_count;
    int closest = -1;
    bn::fixed closest_distance = bn::fixed(32767);

    for (int k = first; k < last; ++k)
    {
        if (bn::abs(_x[k] - x) < reach && bn::abs(_y[k] - y) < reach)
        {
            bn::fixed distance = bn::abs(_x[k] - x) + bn::abs(_y[k] - y);
            if (closest < 0 || distance < closest_distance)
            {
                closest = k;
                closest_distance = distance;
            }
        }
    }

    return closest;
}

void snake::take_damage(int damage, int segment_index)
{
    if (is_dead())
        return;

    if (damage < 1 || segment_index < 0 || segment_index >= _segment_count)
        return;

    if (_segment_count > 2 && (segment_index == 0 || segment_index == _segment_count - 1))
        return;

    _segment_life[segment_index] -= damage;
    if (_segment_life[segment_index] < 0)
        _segment_life[segment_index] = 0;

    life = _segment_life[segment_index];
    g_healthbar->show(this);   // richiede healthbar::show(const live_obj*)

    if (_segment_life[segment_index] == 0)
    {
        if (segment_index == 0 || segment_index == _segment_count - 1)
        {
            _dying = true;
            life = 0;
            _death_ticks = _segment_count * 4;
            invulnerability = 0;
            screen_shake(GROUND_POUND_SHAKE_TICKS, GROUND_POUND_SHAKE_AMP);
        }
        else
        {
            for (int k = segment_index; k < _segment_count - 1; ++k)
                _segment_life[k] = _segment_life[k + 1];

            --_segment_count;
            _segs[_segment_count].set_visible(false);
        }
    }
}

void snake::be_hit_by_bark(bn::fixed power, int segment_index)
{
    int damage = (BAU_DAMAGE + power.multiplication(bn::fixed(BAU_DAMAGE))).integer();
    take_damage(damage, segment_index);
    invulnerability = 30;   // bau::update() lo sovrascrive con la durata del bau per evitare colpi multipli
}

// Morte: i segmenti spariscono uno alla volta dalla coda verso la testa
void snake::update_death()
{
    if (_death_ticks > 0)
        _death_ticks--;

    int vivi = (_death_ticks + 3) / 4;
    for (int k = 0; k < SNAKE_SEGMENTS; ++k)
        _segs[k].set_visible(k < _segment_count && k < vivi);

    if (_death_ticks == 0)
    {
        distrutto = true;
    }
}
