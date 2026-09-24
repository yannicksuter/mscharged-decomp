#include "Game/Render/HomeButtonFade.h"

#include "Game/Render/RLView.h"
#include "Game/UnidentifiedStaticStorage.h"
#include "NL/gl/glState.h"

const unsigned long LightTexture = glGetTexture("global/lightramp");
const unsigned long BlackTexture = glGetTexture("global/black");
const unsigned long WhiteTexture = glGetTexture("global/white");

float gHomeButtonFadeInRate = 0.75f;
float gHomeButtonFadeOutRate = 2.0f;

HomeButtonFade* HomeButtonFade::Instance()
{
    static HomeButtonFade instance;
    return &instance;
}

void HomeButtonFade::FadeOut()
{
    HomeButtonFade::Instance()->Fade(gHomeButtonFadeOutRate, 1.0f);
}

void HomeButtonFade::FadeIn()
{
    HomeButtonFade::Instance()->Fade(gHomeButtonFadeInRate, 0.0f);
}

void HomeButtonFade::Update(float deltaTime)
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

void HomeButtonFade::Render()
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
