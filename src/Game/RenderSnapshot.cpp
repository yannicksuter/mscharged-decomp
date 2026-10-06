#include "Game/RenderSnapshot.h"

#include "Game/GameInfo.h"
#include "Game/CharacterTemplate.h"
#include "Game/Player.h"
#include "NL/nlMemory.h"

#include "Game/UnidentifiedStaticStorage.h"

static nlVector3 lbl_80570C58 = {};

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
    , _2714(RenderSnapshotFlags())
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

    int index = 0;
    for (DrawableFlyingCamera* camera = _2298; index < 10; ++camera)
    {
        camera->mIndex = index++;
    }

    _2294 = 0;
    _1FC0[0].visible = false;
    _1FC0[1].visible = false;
    _1FC0[2].visible = false;

    for (int i = 0; i < 10; ++i)
    {
        int value = g_pCharacters[i]->mUnidentified024.m_eCharacterClass;
        if (value == 8)
        {
            _2714.raw |= 0x80000000;
        }
        else if (value == 5)
        {
            _2714.raw |= 0x40000000;
        }
        else if (value == 13)
        {
            _2714.raw |= 0x20000000;
        }
        else if (value == 19)
        {
            _2714.raw |= 0x10000000;
        }
        else if (value == 14)
        {
            _2714.raw |= 0x08000000;
        }
        else if (value == 12)
        {
            _2714.raw |= 0x04000000;
        }
        else if (value == 2)
        {
            _2714.raw |= 0x02000000;
        }
    }

    if (GameInfoManager::Instance()->GetStadium() == 0x0B)
    {
        _2714.raw |= 0x00800000;
    }
    if (GameInfoManager::Instance()->GetStadium() == 0x0F)
    {
        _2714.raw |= 0x00400000;
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

int RenderSnapshot::NumDrawableObjects() const
{
    int count = 11;
    if (mChainChomp.visible)
    {
        count = 12;
    }

    if (_2714.bits._b7 && mBowser.visible)
    {
        count++;
    }

    if (_2714.bits._b8)
    {
        for (int i = 0; i < 3; i++)
        {
            if (this->_1FC0[i].visible)
            {
                count++;
            }
        }
    }

    for (int i = 0; i < 150; i++)
    {
        if (mPowerups[i].mVisible)
        {
            count++;
        }
    }

    if (_2714.bits._b1)
    {
        count += this->_2294;
    }

    return count;
}

const nlVector3* RenderSnapshot::GetPositionForDrawableObject(int index) const
{
    if (index == 0)
    {
        return &mBall.mPosition;
    }

    index--;
    if (index < 10)
    {
        return &mCharacters[index].position;
    }
    index -= 10;

    if (index == 0)
    {
        if (mChainChomp.visible)
        {
            return &mChainChomp.position;
        }
    }
    else
    {
        index--;
    }

    if (_2714.bits._b7)
    {
        if (index == 0)
        {
            if (mBowser.visible)
            {
                return &mBowser.position;
            }
        }
        else
        {
            index--;
        }
    }

    if (_2714.bits._b8)
    {
        for (int i = 0; i < 3; i++)
        {
            if (_1FC0[i].visible == true)
            {
                if (index == 0)
                {
                    return &_1FC0[i].position;
                }
                index--;
            }
        }
    }

    for (int i = 0; i < 150; i++)
    {
        if (mPowerups[i].mVisible)
        {
            if (index == 0)
            {
                return &mPowerups[i].mPosition;
            }
            index--;
        }
    }

    if (_2714.bits._b1 && index < (s32)_2294)
    {
        return &_2298[index].mPosition;
    }

    return &lbl_80570C58;
}
