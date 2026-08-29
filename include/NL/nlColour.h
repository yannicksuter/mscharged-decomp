#ifndef _NLCOLOUR_H_
#define _NLCOLOUR_H_

#include "types.h"

struct nlColour
{
    unsigned char c[4];

    bool operator==(const nlColour& other) const
    {
        return *(u32*)&c[0] == *(u32*)&other.c[0];
    }

    bool operator!=(const nlColour& other) const
    {
        return !(*this == other);
    }
};

struct nlFloatColour
{
    float c[4];
};

inline void nlColourSet(nlColour& c0, u8 _r, u8 _g, u8 _b, u8 _a)
{
    c0.c[0] = _r;
    c0.c[1] = _g;
    c0.c[2] = _b;
    c0.c[3] = _a;
}

#endif // _NLCOLOUR_H_
