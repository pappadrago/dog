// lampo.h
#pragma once
#include "bn_regular_bg_ptr.h"

void lampo_init(bn::regular_bg_ptr& bg0, bn::regular_bg_ptr& bg1);  // a ogni caricamento schema
void lampo_start();    // avvia il flash bianco
void lampo_update();   // ogni frame
void lampo_reset();    // prima che bg0/bg1 vengano distrutti