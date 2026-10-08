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
#include "Game/Render/WindDebris.h"
#include "Game/Task/FixedUpdateTask.h"
#include "NL/gl/glMemory.h"
#include "NL/gl/glState.h"
#include "NL/nlMemory.h"
#include "NL/nlTask.h"

#include "Game/UnidentifiedStaticStorage.h"

float g_AllActorsHidden;
static nlVector3 sInvalidDrawablePosition = {};

RenderSnapshot::RenderSnapshot()
    : mEvents(0)
    , mValid(false)
    , mGoalLight(false)
    , mBall(this)
    , mNumBulletBills(0)
    , mHasHammers(false)
    , mNumFlyingCameras(0)
    , mpNetMeshPositiveX(0)
    , mpNetMeshNegativeX(0)
    , mFrameBlendPercent(0.0f)
    , mFlags(RenderSnapshotFlags())
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
        camera->mCameraIndex = index++;
    }

    mNumFlyingCameras = 0;
    mWindDebris[0].visible = false;
    mWindDebris[1].visible = false;
    mWindDebris[2].visible = false;

    for (int i = 0; i < 10; ++i)
    {
        int value = g_pCharacters[i]->mUnidentified024.m_eCharacterClass;
        if (value == 8)
        {
            mFlags.raw |= 0x80000000;
        }
        else if (value == 5)
        {
            mFlags.raw |= 0x40000000;
        }
        else if (value == 13)
        {
            mFlags.raw |= 0x20000000;
        }
        else if (value == 19)
        {
            mFlags.raw |= 0x10000000;
        }
        else if (value == 14)
        {
            mFlags.raw |= 0x08000000;
        }
        else if (value == 12)
        {
            mFlags.raw |= 0x04000000;
        }
        else if (value == 2)
        {
            mFlags.raw |= 0x02000000;
        }
    }

    if (GameInfoManager::Instance()->GetStadium() == 0x0B)
    {
        mFlags.raw |= 0x00800000;
    }
    if (GameInfoManager::Instance()->GetStadium() == 0x0F)
    {
        mFlags.raw |= 0x00400000;
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
    mDiddyBanana.Free();

    for (int i = 0; i < 3; ++i)
    {
        mWindDebris[i].Free();
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
    if (mFlags.bits.hammers)
    {
        mHasHammers = gNPCManager->GetNumHammers() != 0;
        if (mHasHammers)
        {
            for (unsigned int i = 0; i < 15; i++)
                mHammers[i].Grab(gNPCManager->GetHammer(i));
        }
    }
    if (mFlags.bits.thwomps)
    {
        for (unsigned int i = 0; i < 8; i++)
            mThwomps[i].Grab(gNPCManager->GetThwomp(i));
    }
    if (mFlags.bits.flyingCameras)
    {
        mNumFlyingCameras = gFlyingCameraCount;
        for (i = 0; i < mNumFlyingCameras; i++)
            _2298[i].Grab();
        for (; i < 10; i++)
            _2298[i].mVisible = false;
    }
    if (mFlags.bits.bulletBills)
    {
        mNumBulletBills = gNPCManager->GetNumBulletBills();
        for (i = 0; i < mNumBulletBills; i++)
            mBulletBills[i].Grab(gNPCManager->GetBulletBill(i));
    }
    mChainChomp.Grab(*gNPCManager->GetChainChomp());
    if (mFlags.bits.yoshiEgg)
        mYoshiEgg.Grab(gNPCManager->mpYoshiEgg);
    if (mFlags.bits.birdoEgg)
        mBirdoEgg.Grab(gNPCManager->mpBirdoEgg);
    if (mFlags.bits.koopaShell)
        mKoopaShell.Grab(gNPCManager->mpKoopaShell);
    if (mFlags.bits.daisyFists)
    {
        mNumVisibleDaisyFists = gNPCManager->mNumVisibleDaisyFists;
        for (i = 0; i < 8; i++)
            mDaisyFists[i].Grab(gNPCManager->GetDaisyFist(i));
    }
    if (mFlags.bits.diddyBanana)
    {
        if (gNPCManager->mpDiddyBanana != 0)
            mDiddyBanana.Grab(*gNPCManager->mpDiddyBanana);
        else
            mDiddyBanana.visible = false;
    }
    if (mFlags.bits.windDebris)
    {
        for (i = 0; i < 3; i++)
        {
            if (gNPCManager->fn_801A9DE0(i) != 0)
                mWindDebris[i].Grab(*gNPCManager->fn_801A9DE0(i));
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
        mPositiveGoalieNetCheck = Goalie::mbPosGoalieNetCheck;
        mNegativeGoalieNetCheck = Goalie::mbNegGoalieNetCheck;
    }
    mCameraUp = g_CameraWorldUpVector;
    if (lbl_806E12C8 != 0)
    {
        for (int i = 0; i < 60; i++)
        {
            PhysicsPatch* patch = lbl_806E12C8->fn_801745B8(i);
            if (patch != 0)
                mPatchPositions[i] = patch->GetPosition();
            else
                mPatchPositions[i].z = -10000.0f;
        }
    }
    mSimulationTime = GetFixedUpdateTask()->mSimulationTime;
    mValid = true;
}

DrawableBulletBill& GetSnapshotBulletBill(RenderSnapshot* snapshot, unsigned int index)
{
    return snapshot->mBulletBills[index];
}

int RenderSnapshot::NumDrawableObjects() const
{
    int count = 11;
    if (mChainChomp.visible)
    {
        count = 12;
    }

    if (mFlags.bits.diddyBanana && mDiddyBanana.visible)
    {
        count++;
    }

    if (mFlags.bits.windDebris)
    {
        for (int i = 0; i < 3; i++)
        {
            if (this->mWindDebris[i].visible)
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

    if (mFlags.bits.flyingCameras)
    {
        count += this->mNumFlyingCameras;
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

    if (mFlags.bits.diddyBanana)
    {
        if (index == 0)
        {
            if (mDiddyBanana.visible)
            {
                return &mDiddyBanana.position;
            }
        }
        else
        {
            index--;
        }
    }

    if (mFlags.bits.windDebris)
    {
        for (int i = 0; i < 3; i++)
        {
            if (mWindDebris[i].visible == true)
            {
                if (index == 0)
                {
                    return &mWindDebris[i].position;
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

    if (mFlags.bits.flyingCameras && index < (s32)mNumFlyingCameras)
    {
        return &_2298[index].mPosition;
    }

    return &sInvalidDrawablePosition;
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
        if (mFlags.bits.yoshiEgg)
            mYoshiEgg.Render(gNPCManager->mpYoshiEgg);
        if (mFlags.bits.birdoEgg)
            mBirdoEgg.Render(gNPCManager->mpBirdoEgg);
        if (mFlags.bits.koopaShell)
            mKoopaShell.Render(gNPCManager->mpKoopaShell);
        if (mFlags.bits.daisyFists && mNumVisibleDaisyFists != 0)
        {
            for (i = 0; i < 8; i++)
                mDaisyFists[i].Render(gNPCManager->GetDaisyFist(i));
        }
        if (mFlags.bits.diddyBanana && gNPCManager->mpDiddyBanana != 0)
            mDiddyBanana.Render(*gNPCManager->mpDiddyBanana);
        if (mFlags.bits.bulletBills)
        {
            for (i = 0; i < mNumBulletBills; i++)
                mBulletBills[i].Render(gNPCManager->GetBulletBill(i));
        }
        if (mFlags.bits.windDebris)
        {
            for (i = 0; i < 3; i++)
            {
                if (mWindDebris[i].visible == true)
                    mWindDebris[i].Render(*gNPCManager->fn_801A9DE0(i));
            }
        }
        for (i = 0; i < 10; i++)
            mCharacters[i].Render(*g_pCharacters[i]);
        for (i = 0; i < 150; i++)
            mPowerups[i].Render(i);
        if (mFlags.bits.hammers && mHasHammers)
        {
            for (unsigned int i = 0; i < 15; i++)
                mHammers[i].Render(gNPCManager->GetHammer(i));
        }
        if (mFlags.bits.thwomps)
        {
            for (unsigned int i = 0; i < 8; i++)
                mThwomps[i].Render(gNPCManager->GetThwomp(i));
        }
        if (mFlags.bits.flyingCameras)
        {
            for (i = 0; i < mNumFlyingCameras; i++)
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
    if (mFlags.bits.hammers)
    {
        mHasHammers = lhs.mHasHammers && rhs.mHasHammers;
        if (mHasHammers)
        {
            for (unsigned int i = 0; i < 15; i++)
                mHammers[i].Blend(blendFactors, lhs.mHammers[i], rhs.mHammers[i]);
        }
    }
    if (mFlags.bits.thwomps)
    {
        for (unsigned int i = 0; i < 8; i++)
            mThwomps[i].Blend(blendFactors, lhs.mThwomps[i], rhs.mThwomps[i]);
    }
    if (mFlags.bits.flyingCameras)
    {
        mNumFlyingCameras = lhs.mNumFlyingCameras <= rhs.mNumFlyingCameras ? lhs.mNumFlyingCameras : rhs.mNumFlyingCameras;
        for (unsigned int i = 0; i < mNumFlyingCameras; i++)
            _2298[i].Blend(blendFactors, lhs._2298[i], rhs._2298[i]);
    }
    mChainChomp.Blend(blendFactors, lhs.mChainChomp, rhs.mChainChomp);
    if (mFlags.bits.yoshiEgg)
        mYoshiEgg.Blend(blendFactors, lhs.mYoshiEgg, rhs.mYoshiEgg);
    if (mFlags.bits.birdoEgg)
        mBirdoEgg.Blend(blendFactors, lhs.mBirdoEgg, rhs.mBirdoEgg);
    if (mFlags.bits.koopaShell)
        mKoopaShell.Blend(blendFactors, lhs.mKoopaShell, rhs.mKoopaShell);
    if (mFlags.bits.daisyFists)
    {
        mNumVisibleDaisyFists = lhs.mNumVisibleDaisyFists >= rhs.mNumVisibleDaisyFists ? lhs.mNumVisibleDaisyFists : rhs.mNumVisibleDaisyFists;
        for (unsigned int i = 0; i < 8 && mNumVisibleDaisyFists != 0; i++)
            mDaisyFists[i].Blend(blendFactors, lhs.mDaisyFists[i], rhs.mDaisyFists[i]);
    }
    if (mFlags.bits.diddyBanana && gNPCManager->mpDiddyBanana != 0)
        mDiddyBanana.Blend(blendFactors, lhs.mDiddyBanana, rhs.mDiddyBanana);
    if (mFlags.bits.bulletBills)
    {
        mNumBulletBills = lhs.mNumBulletBills <= rhs.mNumBulletBills ? lhs.mNumBulletBills : rhs.mNumBulletBills;
        for (unsigned int i = 0; i < mNumBulletBills; i++)
            mBulletBills[i].Blend(blendFactors, lhs.mBulletBills[i], rhs.mBulletBills[i]);
    }
    if (mFlags.bits.windDebris)
    {
        for (int i = 0; i < 3; i++)
        {
            if (gNPCManager->fn_801A9DE0(i) != 0)
                mWindDebris[i].Blend(blendFactors, lhs.mWindDebris[i], rhs.mWindDebris[i]);
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
                if (previous.mPatchPositions[i].z > -100.0f && current.mPatchPositions[i].z > -100.0f)
                {
                    nlVector3 position;
                    nlVecLerp(position, previous.mPatchPositions[i], current.mPatchPositions[i], blendFactors[0]);
                    patch->m_SpawnPosition = position;
                }
                else
                    patch->m_SpawnPosition = patch->GetPosition();
            }
        }
    }
    static float sPreviousStadiumTime = -1.0f;
    mSimulationTime = (1.0f - blendFactors[0]) * lhs.mSimulationTime + blendFactors[0] * rhs.mSimulationTime;
    if (sPreviousStadiumTime > 0.0f)
    {
        float deltaTime = mSimulationTime - sPreviousStadiumTime;
        if (deltaTime > 0.0f)
            UpdateStadium(deltaTime);
    }
    mGoalLight = rhs.mGoalLight;
    mPositiveGoalieNetCheck = rhs.mPositiveGoalieNetCheck;
    sPreviousStadiumTime = mSimulationTime;
    mNegativeGoalieNetCheck = rhs.mNegativeGoalieNetCheck;
}

RenderSnapshot& RenderSnapshot::GetMutable()
{
    mValid = true;
    return *this;
}
