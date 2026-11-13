// include/save_data.h
#pragma once
#include <cstdint>

#define SAVE_MAGIC_V1 0x444F4731
#define SAVE_MAGIC 0x444F4732
#define NUM_MONDI  4

struct save_data_v1
{
    uint32_t magic;
    bool     mondo_sconfitto[NUM_MONDI];
};

struct save_data
{
    uint32_t magic;
    bool     mondo_sconfitto[NUM_MONDI];
    bool     musica_attiva;
};

save_data carica_salvataggio();
void segna_mondo_sconfitto(int mondo);
void salva_musica(bool attiva);
int  primo_mondo_da_giocare();   // il primo non ancora sconfitto: da qui riparte la partita
