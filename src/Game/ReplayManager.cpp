#include "Game/ReplayManager.h"

#include "Game/Camera/CameraMan.h"
#include "Game/ExcitementSystem.h"
#include "Game/FixedUpdateTask.h"
#include "NL/nlMemory.h"
#include "NL/nlTask.h"
#include "new.h"

extern float g_fSimulationTick;
extern float g_fFixedUpdateTick;
extern float lbl_806E14CC;
extern bool lbl_806E14D1;

extern "C" FixedUpdateTask* fn_8011166C();

ReplayManager::ReplayManager()
    : mCurrent(mSnapshots)
    , mPrevious(mSnapshots + 1)
    , mRender(0)
    , mDebugCamera(cFollowCamera::FOLLOW_SELECTABLE)
    , mUnidentified7604(0)
    , mEvents(0)
    , mSpeed(1.0f)
    , mSpeedUp(0.0f)
    , mDeltaTime(0.0f)
    , mTime(0.0f)
    , mReplay(0)
    , mMemory(0)
{
}

ReplayManager* ReplayManager::Instance()
{
    static ReplayManager* rm;
    if (rm == 0)
    {
        rm = new ReplayManager;
    }
    return rm;
}

void ReplayManager::Initialize()
{
    mMemory = (u8*)nlMalloc(0x100000, 0x20, false);
    mReplay = new (nlMalloc(0x48, 8, false)) Replay((char*)mMemory, 0x100000, 0x8000);
    mTime = 0.0f;
}

void ReplayManager::InitializeSnapshots()
{
    for (int i = 0; i < 3; i++)
    {
        mSnapshots[i].Initialize();
    }
}

void ReplayManager::fn_801895B0()
{
    mEvents |= 0x40;
}

void ReplayManager::fn_801895C0()
{
    mEvents |= 4;
}

void ReplayManager::fn_801895D0()
{
    mEvents |= 2;
}

void ReplayManager::fn_801895E0()
{
    mEvents |= 8;
}

void ReplayManager::fn_80189610()
{
    mEvents |= 0x11;
}

void ReplayManager::fn_80189620()
{
    mEvents |= 0x20;
}

void ReplayManager::Uninitialize()
{
    for (int i = 0; i < 3; i++)
    {
        mSnapshots[i].Free();
    }

    delete mReplay;
    mReplay = 0;

    nlFree(mMemory);
    mMemory = 0;
}

void ReplayManager::SwapPreviousAndCurrent()
{
    RenderSnapshot* tmp = mCurrent;
    mCurrent = mPrevious;
    mPrevious = tmp;
}

void ReplayManager::GrabSnapshot()
{
    SwapPreviousAndCurrent();

    mCurrent->Grab();

    if (nlTaskManager::m_pInstance->mCurrentState == 2)
    {
        mTime = mReplay->EndTime() + g_fSimulationTick;
        unsigned int unidentifiedState = ExcitementSystem::fn_80196644().mUnidentified02C;
        unidentifiedState <<= 16;
        unidentifiedState += ExcitementSystem::fn_80196644().mUnidentified02E;
        mReplay->Record<RenderSnapshot>(mTime, *mCurrent, mEvents, unidentifiedState);

        ExcitementSystem& state = ExcitementSystem::fn_80196644();
        state.mUnidentified02C = 0;
        state.mUnidentified02E = 0;
        mEvents = 0;
    }
}

RenderSnapshot& ReplayManager::GetMutableRenderSnapshot()
{
    mRender = mCurrent;
    return mRender->GetMutable();
}

void ReplayManager::Flush()
{
    delete mReplay;
    mReplay = new (nlMalloc(0x48, 8, false)) Replay((char*)mMemory, 0x100000, 0x8000);

    ResetSnapshots();
}

void ReplayManager::DoPotentialAutoReplay(float deltaTime)
{
    if (nlTaskManager::m_pInstance->mCurrentState == 8 && !lbl_806E14D1)
    {
        mSpeed = mSpeedUp * deltaTime + mSpeed;
        if (mSpeed < 0.1f)
        {
            mSpeed = 0.1f;
        }
        mDeltaTime = mSpeed * deltaTime;
        mTime = mTime + mDeltaTime;
        mReplay->Play<RenderSnapshot>(mTime, *mPrevious, *mCurrent, mBlend);
    }
}

void ReplayManager::ResetSnapshots()
{
    for (int i = 0; i < 3; i++)
    {
        mSnapshots[i].Invalidate();
    }

    GrabSnapshot();
}

void ReplayManager::PrepareForRecording()
{
    cCameraManager::Remove(mDebugCamera);
    mTime = mReplay->EndTime();
    mPrevious->Invalidate();
    mCurrent->Invalidate();
    mRender = 0;
}

void ReplayManager::SetCurrentTime(float time)
{
    mTime = time;

    if (mTime < mReplay->BeginTime())
    {
        mTime = mReplay->BeginTime();
    }

    if (mTime > mReplay->EndTime())
    {
        mTime = mReplay->EndTime();
    }
}

static bool NisOverridesReplayBuffer()
{
    return nlTaskManager::m_pInstance->mCurrentState == 0x10 || (nlTaskManager::m_pInstance->mPreviousState == 0x10 && nlTaskManager::m_pInstance->mCurrentState == 1);
}

void ReplayManager::RenderSnapshotAt(float deltaTime)
{
    for (int i = 0; i < 3; i++)
    {
        mBlend[i] = fn_8011166C()->mUnidentified28 / g_fFixedUpdateTick;
    }
    mDeltaTime = fn_8011166C()->mUnidentified28;

    DoPotentialDebugReplay(deltaTime);
    DoPotentialAutoReplay(deltaTime);

    mRender = mCurrent;

    bool transitioning = NisOverridesReplayBuffer();

    if (!transitioning && mPrevious->mValid)
    {
        mSnapshots[2].Blend(mBlend, *mPrevious, *mCurrent);
        mRender = &mSnapshots[2];
    }

    mRender->Render(deltaTime);
    lbl_806E14CC = mRender->_2718;

    if (nlTaskManager::m_pInstance->mCurrentState == 0x20000)
    {
        mSnapshots[2].RenderDebugInfo(*mPrevious, *mCurrent, mBlend[0]);
    }
}
