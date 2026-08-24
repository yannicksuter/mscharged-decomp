#include "NL/nlEndian.h"

void nlSwapEndian(unsigned short in, unsigned short* out)
{
    *out = 0;
    *out |= (in >> 8U) & 0xFF;
    *out |= (in << 8) & 0xFF00;
}
