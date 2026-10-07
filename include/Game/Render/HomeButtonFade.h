#ifndef GAME_RENDER_HOME_BUTTON_FADE_H
#define GAME_RENDER_HOME_BUTTON_FADE_H

#include "NL/gl/glDraw2.h"

struct HomeButtonFade
{
    HomeButtonFade()
        : mFadeRate(0.0f)
        , mOpacity(0.0f)
        , mTargetOpacity(0.0f)
        , mFading(false)
    {
    }

    static HomeButtonFade* Instance();
    void FadeOut();
    void FadeIn();
    void Update(float deltaTime);
    void Render();

    /* 0x00 */ float mFadeRate;
    /* 0x04 */ float mOpacity;
    /* 0x08 */ float mTargetOpacity;
    /* 0x0C */ bool mFading;

private:
    void Fade(float rate, float to)
    {
        mFadeRate = rate;
        mTargetOpacity = to;
        mOpacity = 1.0f - mTargetOpacity;
        mFading = true;
    }

    static void SetPolyColour(glPoly2& poly, u8 r, u8 g, u8 b, u8 a)
    {
        nlColour color;
        nlColourSet(color, r, g, b, a);
        poly.SetColour(color);
    }
}; // size: 0x10

#endif // GAME_RENDER_HOME_BUTTON_FADE_H
