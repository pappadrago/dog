// include/save_data.h
#pragma once
#include <cstdint>

#define SAVE_MAGIC 0x444F4731   // valore arbitrario; cambialo se cambi il formato, per invalidare salvataggi vecchi
#define NUM_MONDI  4

struct save_data
{
    uint32_t magic;
    bool     mondo_sconfitto[NUM_MONDI];
};

save_data carica_salvataggio();
void segna_mondo_sconfitto(int mondo);
int  primo_mondo_da_giocare();   // il primo non ancora sconfitto: da qui riparte la partita