#include "Game/DB/CharacterInfo.h"
#include "Game/Character.h"
#include "Game/CharacterTemplate.h"
#include "Game/UnidentifiedStaticStorage.h"
#include "Game/Render/MegastrikeBackgroundOverlay.h"

#include "Game/Render/RLView.h"

#include "NL/gl/glDraw2.h"
#include "NL/gl/glState.h"
#include "NL/gl/glTexture.h"
#include "NL/nlPrint.h"

static int sBackgroundIntensity = 104;

static char sWhiteTexture[] = "global/white";
static char sGameplayBackgroundFormat[] = "%s/mega_gameplay_bg";
static char sBlackTexture[] = "global/black";

MegastrikeBackgroundOverlay gMegastrikeBackgroundOverlay;

void MegastrikeBackgroundOverlay::Start(
    float rate, float target, int mode)
{
    mFadeRate = rate;
    mTargetAlpha = target;
    mTeamIndex = mode;
}

void MegastrikeBackgroundOverlay::UpdateAndRender(float deltaTime)
{
    mAlpha += mFadeRate * deltaTime;
    if (mFadeRate == 0.0f)
    {
        mAlpha = mTargetAlpha;
    }
    else if (mFadeRate > 0.0f)
    {
        if (mAlpha > mTargetAlpha)
        {
            mAlpha = mTargetAlpha;
        }
    }
    else if (mFadeRate < 0.0f && mAlpha < mTargetAlpha)
    {
        mAlpha = mTargetAlpha;
    }

    if (mAlpha <= 0.0f)
    {
        mActive = false;
        return;
    }

    mActive = true;

    glPoly2 poly;
    nlColour colour;
    glSetDefaultState(false);
    glSetCurrentTexture(glGetTexture(sWhiteTexture), GLTT_Diffuse);

    u8 shade = (int)(mAlpha * (float)sBackgroundIntensity);
    nlColourSet(colour, shade, shade, shade, 0xFF);
    poly.FullCoverage(colour, -1.0f);
    poly.Attach(GetLayerView(eCLV_MegastrikeBackground), 0, 0);

    glSetDefaultState(false);
    glSetRasterState(GLS_AlphaBlend, 1);
    glSetCurrentRasterState(glHandleizeRasterState());

    int playerIndex = mTeamIndex == 0 ? 0 : 4;
    char textureName[64];
    nlSNPrintf(textureName, sizeof(textureName), sGameplayBackgroundFormat, g_pCharacters[playerIndex]->mUnidentified11C->mName);

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

    nlColourSet(colour, 0xFF, 0xFF, 0xFF, (int)(255.0f * mAlpha));
    poly.FullCoverage(colour, 0.0f);
    poly.Attach(GetLayerView(eCLV_MegastrikeBackground), 0, 0);
}
