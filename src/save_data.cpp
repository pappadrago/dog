// src/save_data.cpp
#include "save_data.h"
#include "bn_sram.h"

save_data carica_salvataggio()
{
    save_data dati;
    bn::sram::read(dati);

    if (dati.magic != SAVE_MAGIC)   // prima esecuzione, o salvataggio di un formato precedente
    {
        dati.magic = SAVE_MAGIC;
        for (int i = 0; i < NUM_MONDI; i++)
            dati.mondo_sconfitto[i] = false;
        bn::sram::write(dati);
    }

    return dati;
}

void segna_mondo_sconfitto(int mondo)
{
    save_data dati = carica_salvataggio();
    if (mondo >= 0 && mondo < NUM_MONDI)
        dati.mondo_sconfitto[mondo] = true;
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