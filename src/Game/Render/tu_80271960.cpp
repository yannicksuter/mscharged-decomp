#include "Game/Render/tu_80271960.h"

#include "Game/Render/RLView.h"
#include "NL/gl/glState.h"

float lbl_806DEE20 = 0.75f;
float lbl_806DEE24 = 2.0f;

UnidentifiedHBMDisplayState* fn_80271960()
{
    static UnidentifiedHBMDisplayState instance;
    return &instance;
}

void UnidentifiedHBMDisplayState::fn_802719A0()
{
    fn_80271960()->Fade(lbl_806DEE24, 1.0f);
}

void UnidentifiedHBMDisplayState::fn_80271A00()
{
    fn_80271960()->Fade(lbl_806DEE20, 0.0f);
}

void UnidentifiedHBMDisplayState::fn_80271A64(float deltaTime)
{
    if (mEnabled)
    {
        if (mPos < mTo)
        {
            mPos += mRate * deltaTime;
        }
        else
        {
            mPos -= mRate * deltaTime;
        }

        if (mPos <= 0.0f)
        {
            mPos = 0.0f;
            mEnabled = false;
        }
        else if (mPos >= 1.0f)
        {
            mPos = 1.0f;
            mEnabled = false;
        }
    }
    else
    {
        mPos = mTo;
    }
}

void UnidentifiedHBMDisplayState::fn_80271AEC()
{
    const int darkenAmount = (int)(255.0f * mPos);
    if ((u8)darkenAmount == 0)
    {
        return;
    }

    glPoly2 poly;
    glSetDefaultState(true);
    glSetRasterState(GLS_DepthTest, 0);
    glSetRasterState(GLS_AlphaBlend, 1);
    glSetCurrentTexture(glGetTexture("global/white"), GLTT_Diffuse);
    glSetCurrentTextureState(glHandleizeTextureState());
    glSetRasterState(GLS_DepthTest, 0);
    glSetCurrentRasterState(glHandleizeRasterState());
    poly.SetupRectangle(0.0f, 0.0f, 640.0f, 480.0f, -1.0f);
    SetPolyColour(poly, 0, 0, 0, darkenAmount);
    poly.Attach(GetLayerView(eCLV_HomeButtonFadeOut), 0, 0);
    glSetDefaultState(false);
}
