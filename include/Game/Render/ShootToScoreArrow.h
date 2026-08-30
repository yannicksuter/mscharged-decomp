#ifndef GAME_RENDER_SHOOT_TO_SCORE_ARROW_H
#define GAME_RENDER_SHOOT_TO_SCORE_ARROW_H

#include "NL/gl/glDraw2.h"
#include "NL/nlColour.h"
#include "types.h"

struct LoadFrame;
struct SaveFrame;

class WorldDarkening
{
public:
    WorldDarkening()
        : mRate(0.0f)
        , mPos(0.0f)
        , mTo(0.0f)
        , mActive(false)
    {
    }

    static WorldDarkening& Instance();
    void fn_801AF550();
    void Fade(float rate, float to);
    void fn_801AF570(float deltaTime);
    void fn_801AF5F4();
    void fn_801AF734(SaveFrame& frame);
    void fn_801AF7A8(LoadFrame& frame);

    /* 0x00 */ float mRate;
    /* 0x04 */ float mPos;
    /* 0x08 */ float mTo;
    /* 0x0C */ bool mActive;

private:
    static inline void SetPolyColour(glPoly2& poly, u8 r, u8 g, u8 b, u8 a)
    {
        nlColour color;
        nlColourSet(color, r, g, b, a);
        poly.SetColour(color);
    }
}; // size: 0x10

#endif // GAME_RENDER_SHOOT_TO_SCORE_ARROW_H
