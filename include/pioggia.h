#pragma once

#include "bn_fixed.h"
#include <cstdint>

struct pioggia_def
{
    int16_t x, y;
    int intensita;   // gocce per frame
};

class pioggia
{
public:
    explicit pioggia(const pioggia_def& d);

    void update();

private:
    bn::fixed chr_x, chr_y;
    int _intensita;
};
