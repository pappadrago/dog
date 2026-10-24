#include "piattaforma_mobile.h"
#include "globals.h"
#include "dog.h"
#include "enemy.h"
#include "bn_math.h"
#include "bn_sprite_items_fox1632.h"

static constexpr bn::fixed META_LARGHEZZA = 16;
static constexpr bn::fixed META_ALTEZZA = 4;
static constexpr bn::fixed TOLLERANZA = 6;

// `velocita_gradi_x16` è espresso in gradi * 16 (fixed-point Q8.4).
// Per ottenere i gradi reali si divide per 16. Il divisore è costante
// e può essere calcolato una sola volta all'inizializzazione.
static constexpr int PASSO_DIVISORE = 16;
static constexpr bn::fixed RAD_TO_DEG = bn::fixed(57.2958);
static constexpr bn::fixed LUNGHEZZA_MIN = 4;   // evita divisioni per ~0 (estremi di un segmento)


// Normalizza un angolo gradi nel range [0, 360)
static bn::fixed normalizza_angolo(bn::fixed angolo)
{
    while (angolo >= 360) angolo -= 360;
    while (angolo < 0)    angolo += 360;
    return angolo;
}

// Determina se il movimento da `vecchio` a `nuovo` ha attraversato l'angolo `target`.
// Se oraria=true, il movimento e in senso orario; altrimenti in senso antiorario.
static bool attraversa(bn::fixed vecchio, bn::fixed nuovo, bn::fixed target, bool oraria)
{
    // Spostamenti relativi rispetto a `vecchio`, in funzione della direzione
    bn::fixed rel_target = oraria ? (target - vecchio) : (vecchio - target);
    bn::fixed rel_nuovo = oraria ? (nuovo - vecchio) : (vecchio - nuovo);

    // Normalizza nel range [0, 360) per confrontare gli avanamenti
    while (rel_target < 0) rel_target += 360;
    while (rel_nuovo < 0) rel_nuovo += 360;

    // L'attraversamento avviene se il target e stato superato durante la rotazione
    return rel_target <= rel_nuovo;
}

piattaforma_mobile::piattaforma_mobile(const piattaforma_mobile_def& d) : def(d)
{

    _angolo = _angolo_iniziale = 0;
    _velocita_lineare = bn::fixed(def.velocita_px_x16).division(PASSO_DIVISORE);   // px/frame
    calcola_posizione();   // calcola anche _passo_angolo per il primo frame
    _px = _x; _py = _y;

    sprite = bn::sprite_items::fox1632.create_sprite(_x - HALF_SCREEN_W, _y - HALF_SCREEN_H, 60);
    sprite->set_camera(g_camera);
    sprite->set_bg_priority(1);

    attiva = _attiva_iniziale = d.parte_attiva;
    if (attiva)
        _ticks_vita = d.durata_ticks;
}

void piattaforma_mobile::attiva_piattaforma()
{
    if (attiva || distrutta) return;
    attiva = true;
    _ticks_vita = def.durata_ticks;
    _px = _x; _py = _y;
}

// void piattaforma_mobile::calcola_posizione()
// {
//     bn::fixed cos_a = bn::degrees_lut_cos_safe(_angolo);
//     bn::fixed sin_a = bn::degrees_lut_sin_safe(_angolo);

//     bn::fixed lx = bn::fixed(def.semiasse_x).multiplication(cos_a);
//     bn::fixed ly = bn::fixed(def.semiasse_y).multiplication(sin_a);

//     bn::fixed cos_r = bn::degrees_lut_cos_safe(bn::fixed(def.rotazione));
//     bn::fixed sin_r = bn::degrees_lut_sin_safe(bn::fixed(def.rotazione));

//     _x = bn::fixed(def.centro_x) + lx.multiplication(cos_r) - ly.multiplication(sin_r);
//     _y = bn::fixed(def.centro_y) + lx.multiplication(sin_r) + ly.multiplication(cos_r);
// }

void piattaforma_mobile::calcola_posizione()
{
    bn::fixed cos_a = bn::degrees_lut_cos_safe(_angolo);
    bn::fixed sin_a = bn::degrees_lut_sin_safe(_angolo);

        // Passo angolare del PROSSIMO frame per avere velocità lineare costante:
    // dθ = v / len(θ), con len(θ) = sqrt(a²·sin²θ + b²·cos²θ)
    {
        bn::fixed ax2 = bn::fixed(def.semiasse_x * def.semiasse_x);
        bn::fixed ay2 = bn::fixed(def.semiasse_y * def.semiasse_y);
        bn::fixed len2 = ax2.multiplication(sin_a.multiplication(sin_a))
                       + ay2.multiplication(cos_a.multiplication(cos_a));

        // sqrt intera con 4 bit di precisione in più: sqrt(len2 * 256) = len * 16
        int len16 = bn::sqrt(len2.data() >> 4);
        bn::fixed len = bn::fixed::from_data(len16 << 8);   // len16 / 16
        if (len < LUNGHEZZA_MIN)
            len = LUNGHEZZA_MIN;

        _passo_angolo = (_velocita_lineare * RAD_TO_DEG).division(len);
    }

    bn::fixed lx = bn::fixed(def.semiasse_x).multiplication(cos_a);
    bn::fixed ly = bn::fixed(def.semiasse_y).multiplication(sin_a);

    bn::fixed cos_r = bn::degrees_lut_cos_safe(bn::fixed(def.rotazione));
    bn::fixed sin_r = bn::degrees_lut_sin_safe(bn::fixed(def.rotazione));

    bn::fixed fx = bn::fixed(def.centro_x) + lx.multiplication(cos_r) - ly.multiplication(sin_r);
    bn::fixed fy = bn::fixed(def.centro_y) + lx.multiplication(sin_r) + ly.multiplication(cos_r);

    _x = bn::fixed(fx.round_integer());
    _y = bn::fixed(fy.round_integer());
}

void piattaforma_mobile::rinasci()
{
    distrutta = false;
    dog_agganciato = false;
    nemico_agganciato = nullptr;
    _ticks_dopo_salita_rimanenti = -1;

    _angolo = _angolo_iniziale;
    attiva = _attiva_iniziale;
    _ticks_vita = attiva ? def.durata_ticks : -1;

    calcola_posizione();
    _px = _x; _py = _y;

    sprite = bn::sprite_items::fox1632.create_sprite(_x - HALF_SCREEN_W, _y - HALF_SCREEN_H, 60);
    sprite->set_camera(g_camera);
    sprite->set_bg_priority(1);
}

void piattaforma_mobile::distruggi()
{
    distrutta = true;
    dog_agganciato = false;
    nemico_agganciato = nullptr;
    sprite.reset();
}

void piattaforma_mobile::muovi()
{
    // --- Gestione stato distrutta (respawn) ---
    if (distrutta)
    {
        if (_ticks_respawn_rimanenti > 0)
        {
            _ticks_respawn_rimanenti--;
            if (_ticks_respawn_rimanenti == 0)
                rinasci();
        }
        return;
    }

    // --- Salva posizione precedente per calcolare delta di trasporto ---
    _px = _x; _py = _y;

    // --- Aggiorna movimento angolare ---
    if (attiva)
    {
        if (_ticks_pausa_rimanenti > 0)
        {
            _ticks_pausa_rimanenti--;
        }
        else
        {
            bn::fixed vecchio = _angolo;
            bn::fixed nuovo = _angolo + (def.oraria ? _passo_angolo : -_passo_angolo);
            nuovo = normalizza_angolo(nuovo);

            if (def.pausa_angolo_0 > 0 && attraversa(vecchio, nuovo, 0, def.oraria))
            {
                _angolo = 0;
                _ticks_pausa_rimanenti = def.pausa_angolo_0;
            }
            else if (def.pausa_angolo_180 > 0 && attraversa(vecchio, nuovo, 180, def.oraria))
            {
                _angolo = 180;
                _ticks_pausa_rimanenti = def.pausa_angolo_180;
            }
            else
            {
                _angolo = nuovo;
            }

            calcola_posizione();
        }
    }

    // --- Aggiorna sprite ---
    sprite->set_x(_x - HALF_SCREEN_W);
    sprite->set_y(_y - HALF_SCREEN_H);

    // --- Gestione autodistruzione in seguito all'attivazione ---
    if (attiva && def.durata_ticks >= 0)
    {
        if (_ticks_vita > 0) _ticks_vita--;

        // Lampeggio prima della distruzione
        static constexpr int TICKS_LAMPEGGIO = 60;
        if (_ticks_vita <= TICKS_LAMPEGGIO)
            sprite->set_visible((_ticks_vita / 4) % 2);

        if (_ticks_vita == 0)
        {
            distruggi();
            return;
        }
    }

    // --- Gestione autodistruzione in seguito alla salita del cane ---
    if (_ticks_dopo_salita_rimanenti >= 0)
    {
        if (_ticks_dopo_salita_rimanenti > 0)
        {
            _ticks_dopo_salita_rimanenti--;
        }
        else
        {
            distruggi();
            if (def.ticks_prima_di_tornare >= 0)
                _ticks_respawn_rimanenti = def.ticks_prima_di_tornare;
        }
    }
}

void piattaforma_mobile::trasporta_se_agganciato()
{
    if (distrutta) return;

    if (dog_agganciato)
    {
        g_dog->chr_x += delta_x();
        g_dog->chr_y += delta_y();
        g_dog->chr_vy = ZERO;
        g_dog->onGround = true;
    }
    if (nemico_agganciato && !nemico_agganciato->distrutto)
    {
        nemico_agganciato->chr_x += delta_x();
        nemico_agganciato->chr_y += delta_y();
        nemico_agganciato->chr_vy = ZERO;
        nemico_agganciato->onGround = true;
    }
}

bool piattaforma_mobile::continua_aggancio(live_obj& o)
{
    bool salta_via = o.chr_vy < 0;
    bool fuori_x = bn::abs(o.chr_x - _x) >= META_LARGHEZZA;
    if (salta_via || fuori_x) return false;

    o.chr_y = (_y - META_ALTEZZA) - o.box_dim;
    o.chr_vy = ZERO;
    o.onGround = true;
    return true;
}

bool piattaforma_mobile::prova_nuovo_aggancio(live_obj& o)
{
    bn::fixed piedi = o.chr_y + o.box_dim;
    bool sopra_in_x = bn::abs(o.chr_x - _x) < META_LARGHEZZA;
    bool vicino_sopra = bn::abs(piedi - (_y - META_ALTEZZA)) < TOLLERANZA;
    bool non_salta_su = o.chr_vy >= 0;

    if (sopra_in_x && vicino_sopra && non_salta_su)
    {
        o.chr_y = (_y - META_ALTEZZA) - o.box_dim;
        o.chr_vy = ZERO;
        o.onGround = true;
        return true;
    }
    return false;
}

// --- Gestione cane ---

// Collisione "solido": risoluzione AABB con spinta (push-out) lungo l'asse minore.
// Il cane viene spinto fuori dalla piattaforma e non vi rimane agganciato.
void piattaforma_mobile::aggancia_cane_solido()
{
    bn::fixed dx = g_dog->chr_x - _x;
    bn::fixed dy = g_dog->chr_y - _y;
    bn::fixed overlap_x = (META_LARGHEZZA + g_dog->box_halfdim) - bn::abs(dx);
    bn::fixed overlap_y = (META_ALTEZZA + g_dog->box_dim) - bn::abs(dy);

    if (overlap_x <= 0 || overlap_y <= 0)
    {
        // Nessuna sovrapposizione: il cane non è a contatto
        dog_agganciato = false;
    }
    else if (overlap_x < overlap_y)
    {
        // Spinta orizzontale (sovrapposizione minore in X)
        g_dog->chr_x += (dx > 0) ? overlap_x : -overlap_x;
        g_dog->chr_vx = ZERO;
        dog_agganciato = false;
    }
    else if (dy < 0)
    {
        // Cane sopra la piattaforma: appoggio dall'alto
        g_dog->chr_y = (_y - META_ALTEZZA) - g_dog->box_dim;
        g_dog->chr_vy = ZERO;
        g_dog->onGround = true;
        dog_agganciato = true;
    }
    else
    {
        // Cane sotto la piattaforma: spinta verso il basso
        g_dog->chr_y = (_y + META_ALTEZZA) + g_dog->box_dim;
        if (g_dog->chr_vy < 0) g_dog->chr_vy = ZERO;
        dog_agganciato = false;
    }
}

// Collisione "cloud": il cane si appoggia dall'alto con tolleranza,
// senza subire spinte orizzontali.
void piattaforma_mobile::aggancia_cane_cloud()
{
    bool era_agganciato = dog_agganciato;
    bool agganciato_ora = era_agganciato ? continua_aggancio(*g_dog) : prova_nuovo_aggancio(*g_dog);
    dog_agganciato = agganciato_ora;
}

// Imposta il timer di autodistruzione quando il cane sale sulla piattaforma.
// Condizioni: il cane non era già agganciato, la piattaforma prevede il timer,
// e il timer non è già in conteggio.
void piattaforma_mobile::imposta_timer_dopo_salita(bool appena_agganciato)
{
    if (appena_agganciato &&
        def.ticks_dopo_salita >= 0 &&
        _ticks_dopo_salita_rimanenti < 0)
    {
        _ticks_dopo_salita_rimanenti = def.ticks_dopo_salita;
    }
}

// --- Gestione nemici ---

// Un nemico alla volta può appoggiarsi sulla piattaforma (solo dall'alto),
// indipendentemente dal tipo di collisione della piattaforma.
void piattaforma_mobile::aggancia_nemico()
{
    if (nemico_agganciato)
    {
        if (nemico_agganciato->distrutto || !continua_aggancio(*nemico_agganciato))
            nemico_agganciato = nullptr;
    }
    else
    {
        for (enemy* e : *g_enemies)
        {
            if (e->distrutto) continue;
            if (prova_nuovo_aggancio(*e))
            {
                nemico_agganciato = e;
                break;
            }
        }
    }
}

void piattaforma_mobile::verifica_nuovo_aggancio()
{
    if (distrutta) return;

    // --- cane ---
    bool cane_era_agganciato = dog_agganciato;
    if (def.tipo_collisione == PIATTAFORMA_MOBILE_SOLIDO)
        aggancia_cane_solido();
    else
        aggancia_cane_cloud();

    imposta_timer_dopo_salita(!cane_era_agganciato && dog_agganciato);

    // --- nemici ---
    aggancia_nemico();
}