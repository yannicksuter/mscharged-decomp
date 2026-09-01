#include "unclassified/tu_80199880.h"

extern "C" void* fn_8027262C();

static void* sUnidentifiedView;
static const nlVector3 sHiddenPosition = { 0.0f, 0.0f, -15.0f };

UnidentifiedSkinAnimatedNPC_80199880::UnidentifiedSkinAnimatedNPC_80199880(
    cSHierarchy& hierarchy, int modelID, cInventory<cSAnim>& animInventory,
    void* resource)
    : SkinAnimatedNPC(hierarchy, modelID, resource)
{
    sUnidentifiedView = fn_8027262C();
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

UnidentifiedSkinAnimatedNPC_80199880::~UnidentifiedSkinAnimatedNPC_80199880()
{
}

void UnidentifiedSkinAnimatedNPC_80199880::Update(float dt)
{
    if (mbIsVisible)
    {
        if (mpCharacter != 0)
        {
            maFacingDirection = mpCharacter->m_aActualFacingDirection;
            SetPosition(mpCharacter->m_v3Position);
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

void UnidentifiedSkinAnimatedNPC_80199880::fn_80199A88()
{
    SetPosition(sHiddenPosition);
    mbIsVisible = false;
}

void UnidentifiedSkinAnimatedNPC_80199880::DrawShadow(
    const cPoseAccumulator&, const nlMatrix4&)
{
    if (mbIsVisible)
    {
        SkinAnimatedNPC::DrawShadow(mpLastModel, mWorldMatrix);
    }
}
