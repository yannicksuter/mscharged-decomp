#ifndef GAME_RENDER_TU_80271960_H
#define GAME_RENDER_TU_80271960_H

#include "NL/gl/glDraw2.h"

struct UnidentifiedHBMDisplayState
{
    UnidentifiedHBMDisplayState()
        : mRate(0.0f)
        , mPos(0.0f)
        , mTo(0.0f)
        , mEnabled(false)
    {
    }

    void fn_802719A0();
    void fn_80271A00();
    void fn_80271A64(float deltaTime);
    void fn_80271AEC();

    /* 0x00 */ float mRate;
    /* 0x04 */ float mPos;
    /* 0x08 */ float mTo;
    /* 0x0C */ bool mEnabled;

private:
    void Fade(float rate, float to)
    {
        mRate = rate;
        mTo = to;
        mPos = 1.0f - mTo;
        mEnabled = true;
    }

    static void SetPolyColour(glPoly2& poly, u8 r, u8 g, u8 b, u8 a)
    {
        nlColour color;
        nlColourSet(color, r, g, b, a);
        poly.SetColour(color);
    }
}; // size: 0x10

extern "C" UnidentifiedHBMDisplayState* fn_80271960();

#endif // GAME_RENDER_TU_80271960_H
