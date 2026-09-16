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

inline void nlFloatColourSet(nlFloatColour& output, const nlFloatColour& input)
{
    nlFloatColourSet(output, input.c[0], input.c[1], input.c[2], input.c[3]);
}

inline void nlColourSet(nlColour& c0, u8 _r, u8 _g, u8 _b, u8 _a)
{
    c0.c[0] = _r;
    c0.c[1] = _g;
    c0.c[2] = _b;
    c0.c[3] = _a;
}

inline void nlColourSeReversed(nlColour& c0, u8 _r, u8 _g, u8 _b, u8 _a)
{
    c0.c[3] = _a;
    c0.c[2] = _b;
    c0.c[1] = _g;
    c0.c[0] = _r;
}

inline u8 nlFloatColourToByte(float input)
{
    return (s32)(input * 255.0f);
}

inline void ConvertColour(nlColour& out, const nlFloatColour& in)
{
    out.c[0] = nlFloatColourToByte(in.c[0]);
    out.c[1] = nlFloatColourToByte(in.c[1]);
    out.c[2] = nlFloatColourToByte(in.c[2]);
    out.c[3] = nlFloatColourToByte(in.c[3]);
}

#endif // _NLCOLOUR_H_
