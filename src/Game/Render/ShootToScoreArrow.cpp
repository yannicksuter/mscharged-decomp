#include "Game/Render/ShootToScoreArrow.h"

#include "NL/gl/glDraw2.h"
#include "NL/gl/glState.h"
#include "NL/gl/glView.h"
#include "NL/nlMath.h"

#include <string.h>

struct LoadFrame
{
    char _000[8];
    u8* position;
    char _00C[4];
    float deltaTime;
};

struct SaveFrame
{
    char _000[8];
    u8* position;
};

extern "C" GLView* fn_8027267C(int index);

const unsigned long LightTexture = glGetTexture("global/lightramp");
const unsigned long BlackTexture = glGetTexture("global/black");
const unsigned long WhiteTexture = glGetTexture("global/white");

static bool useSubtractiveDarkening = false;

WorldDarkening& WorldDarkening::Instance()
{
    static WorldDarkening instance;
    return instance;
}

void WorldDarkening::fn_801AF550()
{
    mRate = 0.0f;
    mTo = 0.0f;
    mPos = 0.0f;
}

void WorldDarkening::Fade(float rate, float to)
{
    mRate = rate;
    mTo = to;
}

void WorldDarkening::fn_801AF570(float deltaTime)
{
    if (mPos < mTo)
    {
        mPos += mRate * deltaTime;
    }
    else
    {
        mPos -= mRate * deltaTime;
    }

    float distance = nlAbs(mPos - mTo);
    if (distance <= 2.0f * (mRate * deltaTime))
    {
        mPos = mTo;
    }

    mActive = true;
    if (mPos == mTo)
    {
        mActive = false;
    }
}

void WorldDarkening::fn_801AF5F4()
{
    const int darkenAmount = (int)(255.0f * mPos);
    if ((u8)darkenAmount == 0)
    {
        return;
    }

    glPoly2 poly;
    glSetDefaultState(true);
    glSetRasterState(GLS_DepthTest, 0);
    if (useSubtractiveDarkening)
    {
        glSetRasterState(GLS_AlphaBlend, 7);
    }
    else
    {
        glSetRasterState(GLS_AlphaBlend, 1);
    }

    glSetCurrentTexture(glGetTexture("global/white"), GLTT_Diffuse);
    glSetCurrentTextureState(glHandleizeTextureState());
    glSetRasterState(GLS_DepthTest, 0);
    glSetCurrentRasterState(glHandleizeRasterState());
    poly.SetupRectangle(0.0f, 0.0f, 640.0f, 480.0f, -1.0f);

    if (useSubtractiveDarkening)
    {
        SetPolyColour(poly, darkenAmount, darkenAmount, darkenAmount, 255);
    }
    else
    {
        SetPolyColour(poly, 0, 0, 0, darkenAmount);
    }
    poly.Attach(fn_8027267C(19), 0, 0);
    glSetDefaultState(false);
}

void WorldDarkening::fn_801AF734(SaveFrame& frame)
{
    memcpy(frame.position, &mActive, sizeof(mActive));
    frame.position += sizeof(mActive);
    if (mActive)
    {
        memcpy(frame.position, &mPos, sizeof(mPos));
        frame.position += sizeof(mPos);
    }
}

void WorldDarkening::fn_801AF7A8(LoadFrame& frame)
{
    memcpy(&mActive, frame.position, sizeof(mActive));
    frame.position += sizeof(mActive);
    if (mActive)
    {
        memcpy(&mPos, frame.position, sizeof(mPos));
        frame.position += sizeof(mPos);
        fn_801AF570(frame.deltaTime);
    }
}
