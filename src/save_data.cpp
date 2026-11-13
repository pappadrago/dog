// src/save_data.cpp
#include "save_data.h"
#include "bn_sram.h"

save_data carica_salvataggio()
{
    save_data_v1 dati_v1{};
    bn::sram::read(dati_v1);

    if (dati_v1.magic == SAVE_MAGIC)
    {
        save_data dati{};
        bn::sram::read(dati);
        return dati;
    }

    save_data dati{};
    dati.magic = SAVE_MAGIC;
    dati.musica_attiva = true;

    if (dati_v1.magic == SAVE_MAGIC_V1)
    {
        for (int i = 0; i < NUM_MONDI; i++)
            dati.mondo_sconfitto[i] = dati_v1.mondo_sconfitto[i];
    }

    bn::sram::write(dati);
    return dati;
}

void segna_mondo_sconfitto(int mondo)
{
    save_data dati = carica_salvataggio();
    if (mondo >= 0 && mondo < NUM_MONDI)
        dati.mondo_sconfitto[mondo] = true;
    bn::sram::write(dati);
}

void salva_musica(bool attiva)
{
    save_data dati = carica_salvataggio();
    dati.musica_attiva = attiva;
    bn::sram::write(dati);
}

int primo_mondo_da_giocare()
{
    save_data dati = carica_salvataggio();
    for (int i = 0; i < NUM_MONDI; i++)
        if (!dati.mondo_sconfitto[i])
            return i;
    return NUM_MONDI - 1;   // tutto già sconfitto: rigioca l'ultimo mondo
}