#include "pioggia.h"
#include "globals.h"

pioggia::pioggia(const pioggia_def& d)
{
    chr_x = bn::fixed(d.x);
    chr_y = bn::fixed(d.y);
    _intensita = d.intensita;
}

void pioggia::update()
{
    // animazione pioggia placeholder
}
