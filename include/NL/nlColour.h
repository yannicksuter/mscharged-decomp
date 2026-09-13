#ifndef _NLCOLOUR_H_
#define _NLCOLOUR_H_

#include "types.h"

struct nlColour
{
    unsigned char c[4];

    unsigned char& operator[](int index)
    {
        return c[index];
    }

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

inline void nlFloatColourSet(nlFloatColour& c0, float _r, float _g, float _b, float _a)
{
    c0.c[0] = _r;
    c0.c[1] = _g;
    c0.c[2] = _b;
    c0.c[3] = _a;
}

inline void nlColourSet(nlColour& c0, u8 _r, u8 _g, u8 _b, u8 _a)
{
    c0.c[0] = _r;
    c0.c[1] = _g;
    c0.c[2] = _b;
    c0.c[3] = _a;
}

inline void ConvertColour(nlColour& out, const nlFloatColour& in)
{
    float red, green, blue, alpha;
    red = in.c[0];
    green = in.c[1];
    blue = in.c[2];
    alpha = in.c[3];
    out.c[0] = (s32)(red * 255.0f);
    out.c[1] = (s32)(green * 255.0f);
    out.c[2] = (s32)(blue * 255.0f);
    out.c[3] = (s32)(alpha * 255.0f);
}

#endif // _NLCOLOUR_H_
