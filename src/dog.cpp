#include "globals.h"
#include "constanti_fisica.h"
#include "dog.h"
#include "bau.h"
#include "enemy.h"
#include "item.h"
#include "lava.h"
#include "collision.h"
#include "bn_keypad.h"
#include "bn_math.h"
#include "bn_random.h"
#include "bn_sound_items.h"
#include "bn_sprite_items_fox.h"
#include "bn_sprite_items_fox1632.h"
#include "bn_sprite_items_zombie.h"
#include "bn_sprite_items_lavabubble.h"
#include "bn_log.h"
#include "hud.h"
#include "bn_string.h"
#include "bn_sprite_palette_item.h"
#include "bn_sprite_palette_ptr.h"
#include "bn_color.h"
#include "bn_span.h"

void dog::start_polvere_anim(bn::fixed x, bn::fixed y, bool groundPound, bool vertical_flip, int rotation_angle) {
    groundPound ? ground_pound_anim->reset() : polvere_anim->reset();
    polvere_sprite->set_visible(true);
    polvere_sprite->set_z_order(-2);
    polvere_sprite->set_x(x);
    polvere_sprite->set_y(y);
    polvere_sprite->set_vertical_flip(vertical_flip);
    polvere_sprite->set_rotation_angle_safe(rotation_angle);
}

dog::dog()
{
    spriteItems = bn::sprite_items::fox1632;
    chr_y = 32;
    sprite = spriteItems->create_sprite(chr_x - HALF_SCREEN_W, chr_y - HALF_SCREEN_H, 0);

    sprite->set_bg_priority(1);

    actionStand = bn::create_sprite_animate_action_forever(
        *sprite, 8, spriteItems->tiles_item(), 0, 1, 2, 3, 4);
    actionWalk = bn::create_sprite_animate_action_forever(
        *sprite, 2, spriteItems->tiles_item(), 5, 6, 7, 8, 9, 10, 11, 12);

    actionIdle = bn::create_sprite_animate_action_forever(
        *sprite, 5, spriteItems->tiles_item(), 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55);   // idle con testa che si muove

    g_dog->sprite->set_camera(g_camera);
    g_dog->sprite->set_bg_priority(2);

    polvere_sprite = bn::sprite_items::fox1632.create_sprite(0, 0, 0);
    polvere_sprite->set_camera(g_camera);
    polvere_sprite->set_bg_priority(3);
    polvere_sprite->set_visible(false);
    polvere_anim = bn::create_sprite_animate_action_once(
        *polvere_sprite, 2, bn::sprite_items::fox1632.tiles_item(), 56, 57, 58, 59);   // 4 frame di sbuffo
    ground_pound_anim = bn::create_sprite_animate_action_once(
        *polvere_sprite, 2, bn::sprite_items::fox1632.tiles_item(), 63, 64, 65, 66);

    box_dim = bn::fixed(8);
    box_halfdim = bn::fixed(4);

    actionWallJump = bn::create_sprite_animate_action_once(
        *sprite, 3, spriteItems->tiles_item(), 20, 21, 22);

    actionColpito = bn::create_sprite_animate_action_once(
        *sprite, 3, spriteItems->tiles_item(), 13, 14, 15, 16, 17);

    actionAtterraggio = bn::create_sprite_animate_action_once(
        *sprite, 3, spriteItems->tiles_item(), 24, 25, 26, 27);

    actionStacco = bn::create_sprite_animate_action_once(
        *sprite, 3, spriteItems->tiles_item(), 21, 21);

    spriteAusiliario = bn::sprite_items::lavabubble.create_sprite(0, 0, 0);
    spriteAusiliario->set_camera(g_camera);
    spriteAusiliario->set_bg_priority(1);
    spriteAusiliario->set_z_order(-1);          // davanti al cane
    spriteAusiliario->set_visible(false);
    carica_anim = bn::create_sprite_animate_action_forever(
        *spriteAusiliario, 1, bn::sprite_items::lavabubble.tiles_item(), 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22);
    
    puo_scalare = true;

        actionScalaSu = bn::create_sprite_animate_action_forever(
        *sprite, 2, spriteItems->tiles_item(), 5, 6, 7, 8, 9, 10, 11, 12);   // placeholder: frame di salita

    actionScalaGiu = bn::create_sprite_animate_action_forever(
        *sprite, 2, spriteItems->tiles_item(), 5, 6, 7, 8, 9, 10, 11, 12);   // placeholder: frame di discesa
}

void dog::update_animations() {


    // posizione sprite finale del frame, dopo piattaforme mobili e collisioni
    sprite->set_x(chr_x - HALF_SCREEN_W);
    sprite->set_y(chr_y - HALF_SCREEN_H);
    sprite->set_horizontal_flip(dir == DIR_LEFT);

    if (chr_vx == 0 && chr_vy == 0)
        idle_ticks++;
    else {
        idle_ticks = 0;
        actionIdle->reset();
    }

    text_ticks--;
    if (text_ticks > 0)
        // il numero sale piano; lampeggia solo negli ultimi frame, per restare leggibile
        for (bn::sprite_ptr& s : numero_sprites)
        {
            s.set_y(s.y() - text_vy);
            s.set_visible(text_ticks > 10 || text_ticks % 2);
        }
    else
        numero_sprites.clear();

    if (!onGround || chr_vx == 0)
        actionWalk->reset();

    // appena agganciato: riparte dal primo frame
    if (su_scala() && !era_su_scala) {
        actionScalaSu->reset();
        actionScalaGiu->reset();
    }
    era_su_scala = su_scala();


    if (colpito_anim_ticks > 0)
    {
        colpito_anim_ticks--;
        if (!actionColpito->done())
            actionColpito->update();
    }
    else if (su_scala())
    {
        // fermo sulla scala: l'animazione si congela sul frame corrente
        if (intent_scala < 0)
            actionScalaSu->update();
        else if (intent_scala > 0)
            actionScalaGiu->update();
    }
        else if (dash_ticks > 0)
    {
        sprite->set_tiles(spriteItems->tiles_item(), 25);
    }
    else if (atterraggio_anim_ticks > 0)
    {
        atterraggio_anim_ticks--;
        if (!actionAtterraggio->done())
            actionAtterraggio->update();
    }
    else if (stacco_anim_ticks > 0)
    {
        stacco_anim_ticks--;
        if (!actionStacco->done())
            actionStacco->update();
    }
    else if (idle_ticks > 120) {
        if (!actionIdle->done())
            actionIdle->update();
        if (idle_ticks > 360) {
            idle_ticks = 0;
            actionIdle->reset();
        }
    }
    else if (wall_jump_anim_ticks > 0)
    {
        wall_jump_anim_ticks--;
        if (!actionWallJump->done())
            actionWallJump->update();
    }
    else if (wall_sliding)
        sprite->set_tiles(spriteItems->tiles_item(), 27);
    else if (!onGround)
        sprite->set_tiles(spriteItems->tiles_item(), (chr_vy > 0 ? 23 : 21));
    else if (onGround && chr_vx != 0)
        actionWalk->update();
    else
        actionStand->update();

    if (invulnerability)
        sprite->set_visible(invulnerability % 2);

    bool carica_piena = charge_ticks >= BAU_CHARGE_MAX && bn::keypad::a_held();

    if (carica_piena)
    {
        if (!carica_attiva)
        {
            carica_attiva = true;          // frame in cui la carica raggiunge il massimo
            carica_anim->reset();
            spriteAusiliario->set_visible(true);
        }
        carica_anim->update();
        spriteAusiliario->set_x(sprite->x());
        spriteAusiliario->set_y(sprite->y());
        spriteAusiliario->set_horizontal_flip(dir == DIR_LEFT);
    }
    else if (carica_attiva)
    {
        carica_attiva = false;
        spriteAusiliario->set_visible(false);
    }

}

void dog::dog_damage(int amount)
{
    if (life > 0) {
        actionColpito->reset();
        colpito_anim_ticks = COLPITO_ANIM_TICKS;
        life -= amount;
        show_nr(-amount);
        if (life <= 0) {
            life = 0;
        }
    }
}

void dog::applica_powerup(uint16_t attributo)
{
    if (attributo & POWERUP_CORSA) {
        run_acquisito = true;
    }
    if (attributo & POWERUP_SALTO) {
        if (bonus_salto == bn::fixed(0))
            bonus_salto = bn::fixed(0.8);
    }
    if (attributo & POWERUP_BAU) {
        if (bonus_bau == bn::fixed(0))
            bonus_bau = bn::fixed(1.0);
    }
    if (attributo & POWERUP_RESISTENZA) {
        if (bonus_resistenza == bn::fixed(0))
            bonus_resistenza += 30;
    }
    if (attributo & POWERUP_DOPPIO_SALTO) {
        doppio_salto_acquisito = true;
    }
    if (attributo & POWERUP_WALL_JUMP) {
        wall_jump_acquisito = true;
    }
    if (attributo & POWERUP_DASH) {
        dash_acquisito = true;
    }
    if (attributo & POWERUP_AIR_DASH) {
        air_dash_acquisito = true;
    }
    if (attributo & POWERUP_GROUND_POUND) {
        ground_pound_acquisito = true;
    }
}


void dog::show_nr(int n) {
    if (bn::sprite_text_generator* gen = hud_text_generator())
    {

        bn::string<4> txt = bn::to_string<4>(n);
        numero_sprites.clear();
        text_ticks = 30;
        text_vy = bn::fixed(0.5);
        gen->set_center_alignment();
        gen->generate(
            chr_x - HALF_SCREEN_W, chr_y - HALF_SCREEN_H - 8,
            txt, numero_sprites);
        gen->set_left_alignment();
        for (bn::sprite_ptr& s : numero_sprites) {
            s.set_camera(g_camera);   // il testo segue la camera come gli altri sprite del mondo
            s.set_visible(true);
        }
    }
}

void dog::show_str(bn::string<32> txt) {
    if (bn::sprite_text_generator* gen = hud_text_generator())
    {
        numero_sprites.clear();
        
        text_ticks = 180;
        text_vy = bn::fixed(0.1);
        gen->set_center_alignment();
        gen->generate(chr_x - HALF_SCREEN_W, chr_y - HALF_SCREEN_H - 8,
            txt, numero_sprites);
        gen->set_left_alignment();
        for (bn::sprite_ptr& s : numero_sprites) {
            s.set_camera(g_camera);   // il testo segue la camera come gli altri sprite del mondo
            s.set_visible(true);
        }
    }
}



 void dog::update()
{
    //Dato che in main.cpp g_dog->update() gira prima di lava_zona::update() nello stesso frame (verificato nel tuo game_step),
    //  resettare qui a ogni frame e lasciare che un'eventuale zona di lava la rialzi più tardi nello stesso giro 
    // produce il comportamento corretto
    sprite->set_bg_priority(1);




    // --- Timer di tolleranza ---
    if (onGround)
        coyote_ticks = COYOTE_TICKS;
    else if (coyote_ticks > 0)
        coyote_ticks--;

    if (jump_buffer_ticks > 0)
        jump_buffer_ticks--;

    //bool in_arrampicata = agganciato_cornice && ledge_climb_ticks > 0;
    bool b_premuto = bn::keypad::b_pressed() /* && !in_arrampicata*/;

    // intenzione di scalare (la porta ha priorità: se dog è davanti a una porta, up apre la porta)
    intent_scala = bn::keypad::up_held() ? -1 : (bn::keypad::down_held() ? 1 : 0);

    if (su_scala())
    {
        // B = salta via dalla scala nella direzione premuta
        if (b_premuto) {
            int d = bn::keypad::right_held() ? DIR_RIGHT : (bn::keypad::left_held() ? DIR_LEFT : 0);
            stacca_da_scala(SMALL_VX_R.multiplication(bn::fixed(d)), SMALL_JUMP_VY);
        }
        // altrimenti salta accelerazione orizzontale, wall slide, apply_map e apply_gravity:
        // la posizione la gestisce scala::muovi()
    }

    // all'inizio di update(), dopo aver calcolato intent_scala:
    if (su_scala())
        idle_ticks = 0;   // altrimenti, scesi dalla scala, parte subito l'idle lungo (> 120 tick)

    if (b_premuto)
    {
        jump_buffer_ticks = JUMP_BUFFER_TICKS;   // il comando viene sempre registrato
        // if (agganciato_cornice)
        //     rilascia_cornice();                  // salto da appeso: si stacca e prosegue coi rami sotto
    }

    bool puo_saltare_da_terra = onGround || (coyote_ticks > 0 && chr_vy >= 0);

    if (jump_buffer_ticks > 0 && puo_saltare_da_terra)
    {
        // salto da terra: immediato, oppure ritardato (coyote time) o anticipato (buffer)
        jump_buffer_ticks = 0;
        coyote_ticks = 0;
        salto_in_corso = true;
        salto_tenuto = true;
        salto_pieno = false;    // si sapra' solo all'apice (o al rilascio del tasto) se e' stato intero
        onGround = false;
        chr_vy = BIG_JUMP_VY - bonus_salto;
        doppio_salto_disponibile = doppio_salto_acquisito;

        actionStacco->reset();
        stacco_anim_ticks = STACCO_ANIM_TICKS;
    }
    else if (b_premuto)
    {
        if (wall_jump_acquisito && muro_lato != 0)
        {
            int direzione_spinta = (muro_lato == DIR_RIGHT) ? DIR_LEFT : DIR_RIGHT;
            jump_buffer_ticks = 0;
            salto_tenuto = false;
            salto_pieno = true;
            salto_in_corso = true;
            chr_vx = WALL_JUMP_VX.multiplication(bn::fixed(direzione_spinta));
            chr_vy = WALL_JUMP_VY;
            dir = direzione_spinta;

            actionWallJump->reset();
            wall_jump_anim_ticks = WALL_JUMP_ANIM_TICKS;

            start_polvere_anim(chr_x - HALF_SCREEN_W, chr_y - HALF_SCREEN_H, false, false, -90 * dir);

            muro_lato = 0;
            doppio_salto_disponibile = doppio_salto_acquisito;
        }
        else if (doppio_salto_disponibile && bn::abs(chr_vy) <= bn::fixed(2))
        {
            jump_buffer_ticks = 0;
            salto_tenuto = false;
            salto_pieno = true;
            chr_vy = MEDIUM_JUMP_VY - bonus_salto;
            doppio_salto_disponibile = false;
            actionWallJump->reset();
            wall_jump_anim_ticks = WALL_JUMP_ANIM_TICKS;
            salto_in_corso = true;
            start_polvere_anim(chr_x - HALF_SCREEN_W, chr_y - HALF_SCREEN_H + box_dim, false, true, 0);
        }
        // altrimenti il comando resta in buffer per JUMP_BUFFER_TICKS frame
    }

    // --- Salto variabile: rilasciare il tasto in salita taglia la spinta ---
    if (salto_tenuto)
    {
        if (chr_vy >= -3.0) {  // apice perlopiù raggiunto: il salto è pieno, ai fini del gorund_pund
            salto_pieno = true;
        }
        if (chr_vy >= 0) {
            salto_tenuto = false;                    // apice superato: niente da tagliare

        }
        else if (!bn::keypad::b_held())
        {
            if (chr_vy < SALTO_TAGLIO_VY) // questo è il salto minimo
                chr_vy = SALTO_TAGLIO_VY;
            salto_tenuto = false;
        }
    }

    if (ground_pound_acquisito && salto_in_corso && salto_pieno && !onGround && chr_vy > 0 && bn::keypad::down_held())
    {
        ground_pound_engaged = true;
        chr_vy = MAX_FALL;
    }

    bn::fixed runningExtraSpeed = (run_acquisito && bn::keypad::l_held()) ? RUN_EXTRA_SPEED : ZERO;

    if (dash_ticks == 0) {
        if (bn::keypad::right_held()) {
            dir = DIR_RIGHT;
            chr_vx += (onGround ? GROUND_ACCEL : AIR_ACCEL);
        }
        else if (bn::keypad::left_held()) {
            dir = DIR_LEFT;
            chr_vx -= (onGround ? GROUND_ACCEL : AIR_ACCEL);
        }
    }


    if (bn::keypad::a_pressed())
    {
        if (g_bau->ticks == 0)
            g_bau->do_spawn(10);      // primo bau: parte subito, potenza minima
        charge_ticks = 1;             // la carica comincia subito, anche con il primo bau ancora in volo
    }
    else if (bn::keypad::a_held())
    {
        if (charge_ticks > 0 && charge_ticks < BAU_CHARGE_MAX)
            charge_ticks++;
    }

    if (bn::keypad::a_released())
    {
        if (charge_ticks >= BAU_CHARGE_MIN_RILASCIO && g_bau_carico->ticks == 0)
            g_bau_carico->do_spawn(charge_ticks);
        charge_ticks = 0;
    }


    for (item* _item : *g_items)
    {
        if (!_item->active) continue;

        bool hit = check_collision_16(*_item->sprite);

        if (_item->tipo == ITEM_TIPO_INTERRUTTORE)
        {
            _item->contatto_interruttore(hit);
            continue;
        }

        if (_item->ticks2action) continue;
        if (hit)
            _item->raccogli();
    }

    if (invulnerability > 0)
        invulnerability--;

    // --- Dog vs nemici ---
    if (!invulnerability)
    {
        for (enemy* enem : *g_enemies)
        {
            if (enem->invulnerability == 0 && !enem->is_dead()) {


                if (ground_pound_engaged && enem->currentAction != ACTION_ATTACK) {
                    bool hit = check_collision_16(*enem);
                    if (hit)
                    {
                        enem->beHitByDog();
                        enem->takeDamage(10);
                        enem->invulnerability = 60;
                        chr_vy = SMALL_JUMP_VY;
                        chr_vx = (enem->chr_x < chr_x) ? SMALL_VX_R : SMALL_VX_L;
                        continue;
                    }
                }

                bool hit;
                bool hitByMelee = false;
                if (enem->melee_damage && enem->currentAction == ACTION_ATTACK) {
                    hit = check_collision_melee(*enem);
                    hitByMelee = true;
                }
                else
                    hit = check_collision_16(*enem);

                if (hit)
                {
                    bn::fixed new_vx = (chr_x < enem->chr_x) ? bn::fixed(-2.0) : bn::fixed(2.0);
                    chr_vy = SMALL_JUMP_VY;
                    invulnerability = 60;

                    if (hitByMelee) {
                        dog_damage(enem->melee_damage);
                    }
                    else {
                        enem->beHitByDog();
                        dog_damage(enem->contact_damage);
                    }

                    chr_vx = new_vx;
                }
            }


        }



        if (g_boss.has_value() && g_boss->invulnerability == 0)
        {
            bool hit = false;
            bool hitByMelee = false;
            if (g_boss->melee_damage && g_boss->currentAction == ACTION_ATTACK && g_boss->ticks_attacco_melee <= 16) {
                hit = check_collision_melee_boss(*g_boss);
                hitByMelee = true;
            }
            // else  hit = check_collision_16_boss(*g_boss);

            if (hit)
            {
                bn::fixed new_vx = (chr_x < g_boss->chr_x) ? SMALL_VX_L : SMALL_VX_R;
                chr_vy = SMALL_JUMP_VY;
                invulnerability = 60;
                dog_damage(hitByMelee ? g_boss->melee_damage : g_boss->contact_damage);
                chr_vx = new_vx;
            }
            else if (g_boss->weaponSprite && g_boss->weaponTicks > 0) {
                hit = check_collision_16(*g_boss->weaponSprite);
                if (hit)
                {
                    chr_vx = (chr_x < g_boss->wx_base) ? SMALL_VX_L : SMALL_VX_R;
                    chr_vy = SMALL_JUMP_VY;
                    invulnerability = 60;
                    g_boss->weaponTicks = 0;
                    g_boss->weaponSprite->set_visible(false);

                    dog_damage(3);
                }
            }
        }
    }

    if (atterrato_ora && onGround) {
        doppio_salto_disponibile = doppio_salto_acquisito;
    }

    if (atterrato_ora && velocita_atterraggio > VELOCITA_VY_CADUTA_DURA)
    {

        actionAtterraggio->reset();
        atterraggio_anim_ticks = ATTERRAGGIO_ANIM_TICKS;

        stordito_ticks = STORDIMENTO_CADUTA_TICKS;

        if (ground_pound_engaged) {
            start_polvere_anim(chr_x - HALF_SCREEN_W + dir * box_dim, chr_y - HALF_SCREEN_H, true, false, 0);
            screen_shake(GROUND_POUND_SHAKE_TICKS, GROUND_POUND_SHAKE_AMP);
            for (enemy* enem : *g_enemies)
            {
                if (enem->distrutto || enem->invulnerability > 0) continue;

                if (bn::abs(enem->chr_x - chr_x) < GROUND_POUND_ONDA_RAGGIO_X &&
                    bn::abs(enem->chr_y - chr_y) < GROUND_POUND_ONDA_RAGGIO_Y)
                {
                    int allontana = (enem->chr_x < chr_x) ? DIR_LEFT : DIR_RIGHT;
                    enem->beHitByShockwave(allontana);
                }
            }
            bn::sound_items::bomb.play();
        }
        else {
            bn::sound_items::thud.play();
            start_polvere_anim(chr_x - HALF_SCREEN_W, chr_y - HALF_SCREEN_H, false, false, 0);
        }

        ground_pound_engaged = false;
    }
    else if (testata_ora)
    {
        stordito_ticks = STORDIMENTO_CADUTA_TICKS;
        start_polvere_anim(chr_x - HALF_SCREEN_W, chr_y + 4 - HALF_SCREEN_H, false, true, 0);

    }

    if (wall_jump_anim_ticks == 0 && dash_ticks == 0) {
        chr_vx = cap(chr_vx, max_vx + runningExtraSpeed + bonus_corsa);

        if (spinta_ticks > 0) {
            chr_vx = cap(chr_vx, tetto_vx_spinta);
            if (polvere_anim->done())
                start_polvere_anim(chr_x - HALF_SCREEN_W + dir * 8, chr_y + 4 - HALF_SCREEN_H, false, false, 0);
        }
    }
    if (spinta_ticks > 0)
        spinta_ticks--;




    if (!polvere_anim->done()) {
        polvere_anim->update();
    }
    else if (!ground_pound_anim->done())
    {
        ground_pound_anim->update();
    }
    else
        polvere_sprite->set_visible(false);

    if (!su_scala()) {
        chr_x += chr_vx;
        apply_map();
    }



    if (!su_scala() && (((dash_acquisito && onGround) || (air_dash_acquisito && !onGround)) &&
        dash_ticks == 0 && bn::keypad::r_pressed())) {
        dash_ticks = DASH_TICKS;
        polvere_sprite->set_visible(true);
        polvere_sprite->set_vertical_flip(false);
        polvere_sprite->set_rotation_angle_safe(0);
        ground_pound_engaged = false;
        if (wall_sliding) {
            if (muro_lato == DIR_RIGHT)
                dir = DIR_LEFT;
            else if (muro_lato == DIR_LEFT)
                dir = DIR_RIGHT;
        }
    }
    if (dash_ticks > 0)
    {
        chr_vx = DASH_SPEED * dir;
        if (polvere_anim->done())
            polvere_anim->reset();
        polvere_sprite->set_x(chr_x - HALF_SCREEN_W - dir * box_dim);
        polvere_sprite->set_y(chr_y - HALF_SCREEN_H);
        dash_ticks--;
    }
    else apply_friction();

    wall_sliding = wall_jump_acquisito && !onGround && muro_lato != 0 && chr_vy > 0
        && !ground_pound_engaged && dash_ticks == 0 && (
            (muro_lato == DIR_RIGHT && bn::keypad::right_held()) || (muro_lato == DIR_LEFT && bn::keypad::left_held())
            ) && !su_scala();
    if (wall_sliding) {
        if (polvere_anim->done())
            polvere_anim->reset();
        polvere_sprite->set_visible(true);
        polvere_sprite->set_vertical_flip(false);
        polvere_sprite->set_rotation_angle_safe(90 * dir);
        polvere_sprite->set_x(chr_x - HALF_SCREEN_W + (dir > 0 ? (-(box_halfdim >> 1)) : 0));
        polvere_sprite->set_y(chr_y - HALF_SCREEN_H);
    }


    if (wall_sliding && chr_vy > WALL_SLIDE_VY - GRAVITY)
        chr_vy = WALL_SLIDE_VY - GRAVITY;

    if (!su_scala())
        apply_gravity();

}
