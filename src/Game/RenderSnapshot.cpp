#include "Game/RenderSnapshot.h"

#include "Game/GameInfo.h"
#include "Game/Player.h"
#include "NL/nlMemory.h"

extern cPlayer* lbl_8056B800[10];

RenderSnapshot::RenderSnapshot()
    : mEvents(0)
    , mValid(false)
    , mGoalLight(false)
    , mBall(this)
    , _1CC4(0)
    , _1DA0(false)
    , _2294(0)
    , mpNetMeshPositiveX(0)
    , mpNetMeshNegativeX(0)
    , mFrameBlendPercent(0.0f)
    , _2714(0)
{
    mCameraUp.x = 0.0f;
    mCameraUp.y = 0.0f;
    mCameraUp.z = 1.0f;
}

void RenderSnapshot::Initialize()
{
    DrawableNetMesh* pNetMesh = new (nlMalloc(sizeof(DrawableNetMesh), 8, false)) DrawableNetMesh(true);
    mpNetMeshPositiveX = pNetMesh;

    pNetMesh = new (nlMalloc(sizeof(DrawableNetMesh), 8, false)) DrawableNetMesh(false);
    mpNetMeshNegativeX = pNetMesh;

    for (int i = 0; i < 10; ++i)
    {
        _2298[i].mIndex = i;
    }

    _2294 = 0;
    _1FC0[0].visible = false;
    _1FC0[1].visible = false;
    _1FC0[2].visible = false;

    for (int i = 0; i < 10; ++i)
    {
        int value = lbl_8056B800[i]->m_eCharacterClass;
        if (value == 8)
        {
            _2714 |= 0x80000000;
        }
        else if (value == 5)
        {
            _2714 |= 0x40000000;
        }
        else if (value == 13)
        {
            _2714 |= 0x20000000;
        }
        else if (value == 19)
        {
            _2714 |= 0x10000000;
        }
        else if (value == 14)
        {
            _2714 |= 0x08000000;
        }
        else if (value == 12)
        {
            _2714 |= 0x04000000;
        }
        else if (value == 2)
        {
            _2714 |= 0x02000000;
        }
    }

    if (GameInfoManager::Instance()->GetStadium() == 0x0B)
    {
        _2714 |= 0x00800000;
    }
    if (GameInfoManager::Instance()->GetStadium() == 0x0F)
    {
        _2714 |= 0x00400000;
    }

    mValid = false;
}

void RenderSnapshot::Free()
{
    for (int i = 0; i < 10; ++i)
    {
        mCharacters[i].Free();
    }

    mChainChomp.Free();
    mBowser.Free();

    for (int i = 0; i < 3; ++i)
    {
        _1FC0[i].Free();
    }

    delete mpNetMeshPositiveX;
    delete mpNetMeshNegativeX;
    mpNetMeshPositiveX = 0;
    mpNetMeshNegativeX = 0;
}

void RenderSnapshot::Invalidate()
{
    mValid = false;
}

void RenderSnapshot::RenderDebugInfo(
    const RenderSnapshot&, const RenderSnapshot&, float) const
{
}

RenderSnapshot& RenderSnapshot::GetMutable()
{
    mValid = true;
    return *this;
}
