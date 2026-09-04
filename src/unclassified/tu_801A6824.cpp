#include "unclassified/tu_801A6824.h"

#include "Game/Render/RLView.h"

#include "NL/gl/glDraw2.h"
#include "NL/gl/glState.h"
#include "NL/gl/glTexture.h"
#include "NL/nlPrint.h"

struct PlayerPresentationName_801A6834
{
    char _000[4];
    const char* name;
};

struct Player_801A6834
{
    char _000[0x11C];
    PlayerPresentationName_801A6834* presentationName;
};

extern "C"
{
    extern Player_801A6834* lbl_8056B800[10];
    extern int lbl_806DCFB0;
}

static char sWhiteTexture[] = "global/white";
static char sGameplayBackgroundFormat[] = "%s/mega_gameplay_bg";
static char sBlackTexture[] = "global/black";

ScreenOverlay_801A6824 gScreenOverlay_801A6824;

void ScreenOverlay_801A6824::Start(
    float rate, float target, int mode)
{
    mRate = rate;
    mTarget = target;
    mMode = mode;
}

void ScreenOverlay_801A6824::UpdateAndRender(float deltaTime)
{
    mValue += mRate * deltaTime;
    if (mRate == 0.0f)
    {
        mValue = mTarget;
    }
    else if (mRate > 0.0f)
    {
        if (mValue > mTarget)
        {
            mValue = mTarget;
        }
    }
    else if (mRate < 0.0f && mValue < mTarget)
    {
        mValue = mTarget;
    }

    if (mValue <= 0.0f)
    {
        mActive = false;
        return;
    }

    mActive = true;

    glPoly2 poly;
    nlColour colour;
    glSetDefaultState(false);
    glSetCurrentTexture(glGetTexture(sWhiteTexture), GLTT_Diffuse);

    colour.c[3] = 0xFF;
    u8 shade = (int)(mValue * (float)lbl_806DCFB0);
    colour.c[0] = shade;
    colour.c[1] = shade;
    colour.c[2] = shade;
    poly.FullCoverage(colour, -1.0f);
    poly.Attach(GetLayerView(eCLV_MegastrikeBackground), 0, 0);

    glSetDefaultState(false);
    glSetRasterState(GLS_AlphaBlend, 1);
    glSetCurrentRasterState(glHandleizeRasterState());

    int playerIndex = mMode == 0 ? 0 : 4;
    char textureName[64];
    nlSNPrintf(textureName, sizeof(textureName), sGameplayBackgroundFormat, lbl_8056B800[playerIndex]->presentationName->name);

    u32 texture = glGetTexture(textureName);
    u32 selectedTexture;
    if (glTextureLoad(texture))
    {
        selectedTexture = texture;
    }
    else
    {
        selectedTexture = glGetTexture(sBlackTexture);
    }
    glSetCurrentTexture(selectedTexture, GLTT_Diffuse);

    nlColourSet(colour, 0xFF, 0xFF, 0xFF, (int)(255.0f * mValue));
    poly.FullCoverage(colour, 0.0f);
    poly.Attach(GetLayerView(eCLV_MegastrikeBackground), 0, 0);
}
