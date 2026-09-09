#ifndef GAME_RENDER_HOME_BUTTON_FADE_H
#define GAME_RENDER_HOME_BUTTON_FADE_H

#include "NL/gl/glDraw2.h"

struct HomeButtonFade
{
    HomeButtonFade()
        : mRate(0.0f)
        , mPos(0.0f)
        , mTo(0.0f)
        , mEnabled(false)
    {
    }

    static HomeButtonFade* Instance();
    void FadeOut();
    void FadeIn();
    void Update(float deltaTime);
    void Render();

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

#endif // GAME_RENDER_HOME_BUTTON_FADE_H
