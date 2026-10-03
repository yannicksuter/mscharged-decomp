#include "Game/Render/ShootToScoreArrow.h"

#include "Game/Render/RLView.h"

#include "NL/gl/glDraw2.h"
#include "NL/gl/glState.h"
#include "NL/gl/glView.h"

#include "Game/Replay.h"

#include <math.h>
#include <string.h>

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

void WorldDarkening::Update(float deltaTime)
{
    if (mPos < mTo)
    {
        mPos += mRate * deltaTime;
    }
    else
    {
        mPos -= mRate * deltaTime;
    }

    float distance = (float)fabs(mPos - mTo);
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

void WorldDarkening::Render()
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
    float depth = -1.0f;
    poly.SetupRectangle(0.0f, 0.0f, 640.0f, 480.0f, depth);

    if (useSubtractiveDarkening)
    {
        SetPolyColour(poly, darkenAmount, darkenAmount, darkenAmount, 255);
    }
    else
    {
        SetPolyColour(poly, 0, 0, 0, darkenAmount);
    }
    poly.Attach(GetLayerView(eCLV_BigBlackPolygon), 0, 0);
    glSetDefaultState(false);
}

void WorldDarkening::Replay(SaveFrame& frame)
{
    Replayable<0>(frame, mActive);
    if (mActive)
    {
        Replayable<0>(frame, mPos);
    }
}

void WorldDarkening::Replay(LoadFrame& frame)
{
    Replayable<0>(frame, mActive);
    if (mActive)
    {
        Replayable<0>(frame, mPos);
        float deltaTime = frame.mNonBlendableAheadOfFrame;
        Update(deltaTime);
    }
}
