#include "Game/RenderSnapshot.h"

#include "Game/Camera/CameraMan.h"
#include "Game/CharacterTemplate.h"
#include "Game/GameInfo.h"
#include "Game/GL/GLInventory.h"
#include "Game/GL/GLTextureAnim.h"
#include "Game/Goalie.h"
#include "Game/Physics/PhysicsNet.h"
#include "Game/Physics/PhysicsPatch.h"
#include "Game/Player.h"
#include "Game/Render/ChainChomp.h"
#include "Game/Render/DiddyBanana.h"
#include "Game/Render/FlyingCamera.h"
#include "Game/Render/NetMesh.h"
#include "Game/Render/NPCManager.h"
#include "Game/Render/StadiumLoading.h"
#include "Game/Render/tu_801B43F8.h"
#include "Game/Task/FixedUpdateTask.h"
#include "NL/gl/glMemory.h"
#include "NL/gl/glState.h"
#include "NL/nlMemory.h"
#include "NL/nlTask.h"

#include "Game/UnidentifiedStaticStorage.h"

float g_AllActorsHidden;
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

void RenderSnapshot::Grab()
{
    unsigned int i;
    for (i = 0; i < 10; i++)
    {
        mCharacters[i].Grab(*g_pCharacters[i]);
    }
    for (i = 0; i < 150; i++)
    {
        mPowerups[i].Grab(i);
    }
    if (_2714.bits.hammers)
    {
        _1DA0 = gNPCManager->GetNumHammers() != 0;
        if (_1DA0)
        {
            for (unsigned int i = 0; i < 15; i++)
                _1DA4[i].Grab(gNPCManager->GetHammer(i));
        }
    }
    if (_2714.bits.thwomps)
    {
        for (unsigned int i = 0; i < 8; i++)
            _2194[i].Grab(gNPCManager->GetThwomp(i));
    }
    if (_2714.bits.flyingCameras)
    {
        _2294 = gFlyingCameraCount;
        for (i = 0; i < _2294; i++)
            _2298[i].Grab();
        for (; i < 10; i++)
            _2298[i].mVisible = false;
    }
    if (_2714.bits.bulletBills)
    {
        _1CC4 = gNPCManager->GetNumBulletBills();
        for (i = 0; i < _1CC4; i++)
            _1CC8[i].Grab(gNPCManager->GetBulletBill(i));
    }
    mChainChomp.Grab(*gNPCManager->GetChainChomp());
    if (_2714.bits.yoshiEgg)
        _1BA0.Grab(gNPCManager->mpYoshiEgg);
    if (_2714.bits.birdoEgg)
        _1BC4.Grab(gNPCManager->mpBirdoEgg);
    if (_2714.bits.koopaShell)
        _1BE8.Grab(gNPCManager->mpKoopaShell);
    if (_2714.bits.daisyFists)
    {
        _1C00 = gNPCManager->mNumVisibleDaisyFists;
        for (i = 0; i < 8; i++)
            mDaisyFists[i].Grab(gNPCManager->GetDaisyFist(i));
    }
    if (_2714.bits.diddyBanana)
    {
        if (gNPCManager->mpDiddyBanana != 0)
            mBowser.Grab(*gNPCManager->mpDiddyBanana);
        else
            mBowser.visible = false;
    }
    if (_2714.bits.windDebris)
    {
        for (i = 0; i < 3; i++)
        {
            if (gNPCManager->fn_801A9DE0(i) != 0)
                _1FC0[i].Grab(*gNPCManager->fn_801A9DE0(i));
        }
    }
    mBall.Grab();
    mGoalLight = lbl_806E1960;
    if (NetMesh::s_bAnimatedNetMeshEnabled)
    {
        if (mpNetMeshPositiveX != 0)
            mpNetMeshPositiveX->Grab(*PhysicsNet::spPhysNetPositiveX->mpNetMesh);
        if (mpNetMeshNegativeX != 0)
            mpNetMeshNegativeX->Grab(*PhysicsNet::spPhysNetNegativeX->mpNetMesh);
        _2430 = Goalie::mbPosGoalieNetCheck;
        _2431 = Goalie::mbNegGoalieNetCheck;
    }
    mCameraUp = g_CameraWorldUpVector;
    if (lbl_806E12C8 != 0)
    {
        for (int i = 0; i < 60; i++)
        {
            PhysicsPatch* patch = lbl_806E12C8->fn_801745B8(i);
            if (patch != 0)
                _2440[i] = patch->GetPosition();
            else
                _2440[i].z = -10000.0f;
        }
    }
    _2718 = GetFixedUpdateTask()->mSimulationTime;
    mValid = true;
}

DrawableBulletBill& GetSnapshotBulletBill(RenderSnapshot* snapshot, unsigned int index)
{
    return snapshot->_1CC8[index];
}

int RenderSnapshot::NumDrawableObjects() const
{
    int count = 11;
    if (mChainChomp.visible)
    {
        count = 12;
    }

    if (_2714.bits.diddyBanana && mBowser.visible)
    {
        count++;
    }

    if (_2714.bits.windDebris)
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

    if (_2714.bits.flyingCameras)
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

    if (_2714.bits.diddyBanana)
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

    if (_2714.bits.windDebris)
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

    if (_2714.bits.flyingCameras && index < (s32)_2294)
    {
        return &_2298[index].mPosition;
    }

    return &lbl_80570C58;
}

void RenderSnapshot::Invalidate()
{
    mValid = false;
}

void RenderSnapshot::Render(float deltaTime)
{
    unsigned int i;
    bool allActorsHidden = false;
    if (g_AllActorsHidden > 0.0f)
    {
        g_AllActorsHidden -= deltaTime;
        allActorsHidden = true;
    }
    if (!mValid)
        return;
    if (!allActorsHidden)
    {
        mChainChomp.Render(*gNPCManager->GetChainChomp());
        if (_2714.bits.yoshiEgg)
            _1BA0.Render(gNPCManager->mpYoshiEgg);
        if (_2714.bits.birdoEgg)
            _1BC4.Render(gNPCManager->mpBirdoEgg);
        if (_2714.bits.koopaShell)
            _1BE8.Render(gNPCManager->mpKoopaShell);
        if (_2714.bits.daisyFists && _1C00 != 0)
        {
            for (i = 0; i < 8; i++)
                mDaisyFists[i].Render(gNPCManager->GetDaisyFist(i));
        }
        if (_2714.bits.diddyBanana && gNPCManager->mpDiddyBanana != 0)
            mBowser.Render(*gNPCManager->mpDiddyBanana);
        if (_2714.bits.bulletBills)
        {
            for (i = 0; i < _1CC4; i++)
                _1CC8[i].Render(gNPCManager->GetBulletBill(i));
        }
        if (_2714.bits.windDebris)
        {
            for (i = 0; i < 3; i++)
            {
                if (_1FC0[i].visible == true)
                    _1FC0[i].Render(*gNPCManager->fn_801A9DE0(i));
            }
        }
        for (i = 0; i < 10; i++)
            mCharacters[i].Render(*g_pCharacters[i]);
        for (i = 0; i < 150; i++)
            mPowerups[i].Render(i);
        if (_2714.bits.hammers && _1DA0)
        {
            for (unsigned int i = 0; i < 15; i++)
                _1DA4[i].Render(gNPCManager->GetHammer(i));
        }
        if (_2714.bits.thwomps)
        {
            for (unsigned int i = 0; i < 8; i++)
                _2194[i].Render(gNPCManager->GetThwomp(i));
        }
        if (_2714.bits.flyingCameras)
        {
            for (i = 0; i < _2294; i++)
                _2298[i].Render();
        }
        mBall.Render();
    }
    mpNetMeshPositiveX->Render();
    mpNetMeshNegativeX->Render();
    if (nlTaskManager::m_pInstance->mCurrentState == 2)
        cCameraManager::m_UpVectorStack[cCameraManager::m_UpVectorStackSize] = mCameraUp;
    static unsigned long goalLightTexture = glGetTexture("wario_stadium/goallight.ifl");
    GLTextureAnim* animation = glGetCurrentResourcePool()->m_inventory->GetTextureAnim(goalLightTexture);
    if (animation != 0)
        animation->m_bPaused = !mGoalLight;
}

void RenderSnapshot::RenderDebugInfo(
    const RenderSnapshot&, const RenderSnapshot&, float) const
{
}

void RenderSnapshot::Blend(const float* blendFactors, RenderSnapshot& lhs, RenderSnapshot& rhs)
{
    mFrameBlendPercent = blendFactors[0];
    mValid = true;
    for (int i = 0; i < 10; i++)
        mCharacters[i].Blend(blendFactors, lhs.mCharacters[i], rhs.mCharacters[i]);
    for (int i = 0; i < 150; i++)
        mPowerups[i].Blend(blendFactors, lhs.mPowerups[i], rhs.mPowerups[i]);
    if (_2714.bits.hammers)
    {
        _1DA0 = lhs._1DA0 && rhs._1DA0;
        if (_1DA0)
        {
            for (unsigned int i = 0; i < 15; i++)
                _1DA4[i].Blend(blendFactors, lhs._1DA4[i], rhs._1DA4[i]);
        }
    }
    if (_2714.bits.thwomps)
    {
        for (unsigned int i = 0; i < 8; i++)
            _2194[i].Blend(blendFactors, lhs._2194[i], rhs._2194[i]);
    }
    if (_2714.bits.flyingCameras)
    {
        _2294 = lhs._2294 <= rhs._2294 ? lhs._2294 : rhs._2294;
        for (unsigned int i = 0; i < _2294; i++)
            _2298[i].Blend(blendFactors, lhs._2298[i], rhs._2298[i]);
    }
    mChainChomp.Blend(blendFactors, lhs.mChainChomp, rhs.mChainChomp);
    if (_2714.bits.yoshiEgg)
        _1BA0.Blend(blendFactors, lhs._1BA0, rhs._1BA0);
    if (_2714.bits.birdoEgg)
        _1BC4.Blend(blendFactors, lhs._1BC4, rhs._1BC4);
    if (_2714.bits.koopaShell)
        _1BE8.Blend(blendFactors, lhs._1BE8, rhs._1BE8);
    if (_2714.bits.daisyFists)
    {
        _1C00 = lhs._1C00 >= rhs._1C00 ? lhs._1C00 : rhs._1C00;
        for (unsigned int i = 0; i < 8 && _1C00 != 0; i++)
            mDaisyFists[i].Blend(blendFactors, lhs.mDaisyFists[i], rhs.mDaisyFists[i]);
    }
    if (_2714.bits.diddyBanana && gNPCManager->mpDiddyBanana != 0)
        mBowser.Blend(blendFactors, lhs.mBowser, rhs.mBowser);
    if (_2714.bits.bulletBills)
    {
        _1CC4 = lhs._1CC4 <= rhs._1CC4 ? lhs._1CC4 : rhs._1CC4;
        for (unsigned int i = 0; i < _1CC4; i++)
            _1CC8[i].Blend(blendFactors, lhs._1CC8[i], rhs._1CC8[i]);
    }
    if (_2714.bits.windDebris)
    {
        for (int i = 0; i < 3; i++)
        {
            if (gNPCManager->fn_801A9DE0(i) != 0)
                _1FC0[i].Blend(blendFactors, lhs._1FC0[i], rhs._1FC0[i]);
        }
    }
    mBall.Blend(blendFactors, lhs.mBall, rhs.mBall);
    mpNetMeshPositiveX->Blend(blendFactors[0], *lhs.mpNetMeshPositiveX, *rhs.mpNetMeshPositiveX);
    mpNetMeshNegativeX->Blend(blendFactors[0], *lhs.mpNetMeshNegativeX, *rhs.mpNetMeshNegativeX);
    nlVecLerp(mCameraUp, lhs.mCameraUp, rhs.mCameraUp, blendFactors[0]);
    nlVec3Normalize(mCameraUp, mCameraUp);
    if (lbl_806E12C8 != 0 && (nlTaskManager::m_pInstance->mCurrentState & 0x20018) == 0)
    {
        const RenderSnapshot& previous = lhs;
        const RenderSnapshot& current = rhs;
        for (int i = 0; i < 60; i++)
        {
            PhysicsPatch* patch = lbl_806E12C8->fn_801745B8(i);
            if (patch != 0)
            {
                if (previous._2440[i].z > -100.0f && current._2440[i].z > -100.0f)
                {
                    nlVector3 position;
                    nlVecLerp(position, previous._2440[i], current._2440[i], blendFactors[0]);
                    patch->m_SpawnPosition = position;
                }
                else
                    patch->m_SpawnPosition = patch->GetPosition();
            }
        }
    }
    static float sPreviousStadiumTime = -1.0f;
    _2718 = (1.0f - blendFactors[0]) * lhs._2718 + blendFactors[0] * rhs._2718;
    if (sPreviousStadiumTime > 0.0f)
    {
        float deltaTime = _2718 - sPreviousStadiumTime;
        if (deltaTime > 0.0f)
            UpdateStadium(deltaTime);
    }
    mGoalLight = rhs.mGoalLight;
    _2430 = rhs._2430;
    sPreviousStadiumTime = _2718;
    _2431 = rhs._2431;
}

RenderSnapshot& RenderSnapshot::GetMutable()
{
    mValid = true;
    return *this;
}
