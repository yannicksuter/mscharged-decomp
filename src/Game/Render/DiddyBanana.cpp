#include "Game/Render/DiddyBanana.h"

#include "Game/Render/RLView.h"
#include "Game/UnidentifiedStaticStorage.h"

static RLView* sDiddyBananaShadowView;
static const nlVector3 sHiddenPosition = { 0.0f, 0.0f, -15.0f };

DiddyBanana::DiddyBanana(
    cSHierarchy& hierarchy, int modelID, cInventory<cSAnim>& animInventory,
    void* resource)
    : SkinAnimatedNPC(hierarchy, modelID, resource)
{
    sDiddyBananaShadowView = GetUnshadowedView();
    mpZip = animInventory.Find((char*)"zip");
    mpUnzip = animInventory.Find((char*)"unzip");
    mTimer.SetSeconds(0.0f);
    mbIsVisible = false;
    if (mpZip != 0)
    {
        SetAnimState(*mpZip, 0.0f, PM_HOLD);
    }
    mpCharacter = 0;
    mUnidentified80 = false;
}

DiddyBanana::~DiddyBanana()
{
}

void DiddyBanana::Update(float dt)
{
    if (mbIsVisible)
    {
        if (mpCharacter != 0)
        {
            maFacingDirection = mpCharacter->mUnidentified024.m_aActualFacingDirection;
            SetPosition(mpCharacter->mUnidentified024.m_v3Position);
        }

        if (mTimer.m_uPackedTime != 0)
        {
            mTimer.Countdown(dt, 0.0f);
        }
        else
        {
            SkinAnimatedNPC::Update(dt);
        }
    }
}

void DiddyBanana::Hide()
{
    SetPosition(sHiddenPosition);
    mbIsVisible = false;
}

void DiddyBanana::DrawShadow(
    const cPoseAccumulator&, const nlMatrix4&)
{
    if (mbIsVisible)
    {
        SkinAnimatedNPC::DrawShadow(mpLastModel, mWorldMatrix);
    }
}
