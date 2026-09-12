#include "NL/plat/PlatPadManager.h"
#include "NL/plat/WiiRemotePad.h"
#include "NL/plat/WiiFreestylePad.h"

#include "Game/ReplayManager.h"
#include "Game/Character.h"

#include "Game/Camera/CameraMan.h"
#include "Game/Camera/DebugCam.h"
#include "Game/Event.h"
#include "Game/ExcitementSystem.h"
#include "Game/GameEventQueue.h"
#include "Game/ReplayChoreo.h"
#include "Game/Task/FixedUpdateTask.h"
#include "Game/Task/ProfilerTask.h"
#include "Game/Task/TweakerTask.h"
#include "NL/nlAVLTree.h"
#include "NL/nlBind.h"
#include "NL/nlFunction.inl"
#include "NL/nlConfig.h"
#include "NL/globalpad.h"
#include "NL/nlMemory.h"
#include "NL/nlTask.h"
#include "Game/InputManager.h"

#include "Game/UnidentifiedStaticStorage.h"

extern float g_fSimulationTick;
extern float g_fFixedUpdateTick;
extern bool lbl_806E14B8;
extern float lbl_806E14CC;
extern bool lbl_806E14D1;

extern "C"
{
    float fn_80189870();
}

typedef nlAVLTree<unsigned int, UnidentifiedEventBase*,
    DefaultKeyCompare<unsigned int> >
    UnidentifiedEventRegistry;

extern "C" UnidentifiedEventRegistry* g_pEventRegistry;

extern "C" bool fn_8019464C(cCharacter* character)
{
    return character->m_eClassType == FIELDER;
}

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

template <typename EventData>
static inline BindExp2<void,
    Detail::MemFunImpl<void, void (ReplayManager::*)(EventData*)>,
    ReplayManager*, Placeholder<0> >
UnidentifiedMakeReplayBinding(
    void (ReplayManager::*callback)(EventData*), ReplayManager* manager)
{
    typedef Detail::MemFunImpl<void,
        void (ReplayManager::*)(EventData*)>
        CallbackMemFun;
    typedef BindExp2<void, CallbackMemFun, ReplayManager*, Placeholder<0> >
        CallbackBind;
    CallbackMemFun function(callback);
    return CallbackBind(function, manager, placeholder0);
}

static inline BindExp1<void,
    Detail::MemFunImpl<void, void (ReplayManager::*)()>, ReplayManager*>
UnidentifiedMakeReplayBinding(
    void (ReplayManager::*callback)(), ReplayManager* manager)
{
    typedef Detail::MemFunImpl<void, void (ReplayManager::*)()>
        CallbackMemFun;
    typedef BindExp1<void, CallbackMemFun, ReplayManager*> CallbackBind;
    CallbackMemFun function(callback);
    return CallbackBind(function, manager);
}

extern "C" bool fn_80194674(cCharacter* character)
{
    return character->mUnidentified024.m_eCharacterClass == 10;
}

void ReplayManager::DoPotentialDebugReplay(float& deltaTime)
{
    static bool debugReplay
        = GetConfigBool(Config::Global(), "debug_replay_in_release", false);

    cGlobalPad* unidentifiedPad = g_pPadManager->GetPad(0);
    if (debugReplay && !g_bTweaking && !IsProfiling()
        && !IsNetworkOrRecordedGame()
        && unidentifiedPad->PlatJustPressed(4, true))
    {
        if (nlTaskManager::m_pInstance->mCurrentState == 0x20000)
        {
            nlTaskManager::SetNextState(2);
            if (mUnidentified7604 != 0)
            {
                cCameraManager::PopCamera();
                delete mUnidentified7604;
                mUnidentified7604 = 0;
            }
            return;
        }
        else if (nlTaskManager::m_pInstance->mCurrentState == 2)
        {
            mTime = mReplay->EndTime();
            nlTaskManager::SetNextState(0x20000);
        }
        return;
    }

    if (nlTaskManager::m_pInstance->mCurrentState == 0x20000)
    {
        if (cCameraManager::PeekCamera()->GetType() != eCameraType_Debug)
        {
            mUnidentified7604
                = new (nlMalloc(0xA0, 8, false)) cDebugCamera(true);
            cCameraManager::PushCamera(mUnidentified7604);
        }

        mDeltaTime = 0.0f;
        if (lbl_806E14B8 == 1)
        {
            mDeltaTime = 0.02f * unidentifiedPad->GetPressure(5, true);
            if (unidentifiedPad->GetPressure(5, true))
            {
                mDeltaTime = 0.02f;
            }
            else if (unidentifiedPad->GetPressure(6, true))
            {
                mDeltaTime = 0.02f * 5.0f;
            }
        }
        else
        {
            mDeltaTime
                -= 0.02f * unidentifiedPad->GetPressure(5, true);
            mDeltaTime
                += 0.02f * unidentifiedPad->GetPressure(6, true);
        }

        int unidentifiedClassID
            = unidentifiedPad->mBackend->GetClassID();
        if (g_pPlatPadManager->type[0] == 2
            && (unidentifiedClassID == gWiiRemotePadClassID
                || unidentifiedClassID == gWiiFreestylePadClassID))
        {
            mDeltaTime *= fn_80189870();
        }

        float time = mTime + mDeltaTime;
        if (time < mReplay->BeginTime())
        {
            time = mReplay->BeginTime();
        }
        if (time > mReplay->EndTime())
        {
            time = mReplay->EndTime();
        }

        mDeltaTime = time - mTime;
        mTime = time;
        mReplay->Play<RenderSnapshot>(mTime, *mPrevious, *mCurrent, mBlend);
    }
}

void ReplayManager::fn_80188D88()
{
    {
        Function<ReceiveBallData*> callback(
            UnidentifiedMakeReplayBinding(
                &ReplayManager::fn_801895C0, this));
        UnidentifiedEventBase** foundEvent;
        unsigned int hash;
        hash = HashEventName("ReceiveBall", -1);
        foundEvent = 0;
        g_pEventRegistry->Find(hash, &foundEvent, 0);
        UnidentifiedEventBase* event = foundEvent != 0 ? *foundEvent : 0;
        ((UnidentifiedTypedEvent<ReceiveBallData>*)event)
            ->Add(callback, 0, -1);
    }
    {
        Function<UnidentifiedEventData_80066590*> callback(
            UnidentifiedMakeReplayBinding(
                &ReplayManager::fn_801895D0, this));
        UnidentifiedEventBase** foundEvent;
        unsigned int hash;
        hash = HashEventName("ShotAtGoal", -1);
        foundEvent = 0;
        g_pEventRegistry->Find(hash, &foundEvent, 0);
        UnidentifiedEventBase* event = foundEvent != 0 ? *foundEvent : 0;
        ((UnidentifiedTypedEvent<UnidentifiedEventData_80066590>*)event)
            ->Add(callback, 0, -1);
    }
    {
        Function<UnidentifiedEventData_800663A8*> callback(
            UnidentifiedMakeReplayBinding(
                &ReplayManager::fn_801895E0, this));
        UnidentifiedEventBase** foundEvent;
        unsigned int hash;
        hash = HashEventName("PassBall", -1);
        foundEvent = 0;
        g_pEventRegistry->Find(hash, &foundEvent, 0);
        UnidentifiedEventBase* event = foundEvent != 0 ? *foundEvent : 0;
        ((UnidentifiedTypedEvent<UnidentifiedEventData_800663A8>*)event)
            ->Add(callback, 0, -1);
    }
    {
        Function<GoalScoredData*> callback(
            UnidentifiedMakeReplayBinding(
                &ReplayManager::fn_801895F0, this));
        UnidentifiedEventBase** foundEvent;
        unsigned int hash;
        hash = HashEventName("GoalScored", -1);
        foundEvent = 0;
        g_pEventRegistry->Find(hash, &foundEvent, 0);
        UnidentifiedEventBase* event = foundEvent != 0 ? *foundEvent : 0;
        ((UnidentifiedTypedEvent<GoalScoredData>*)event)
            ->Add(callback, 0, -1);
    }
    {
        Function<UnidentifiedEventData_8006649C*> callback(
            UnidentifiedMakeReplayBinding(
                &ReplayManager::fn_80189610, this));
        UnidentifiedEventBase** foundEvent;
        unsigned int hash;
        hash = HashEventName("GoalieSave", -1);
        foundEvent = 0;
        g_pEventRegistry->Find(hash, &foundEvent, 0);
        UnidentifiedEventBase* event = foundEvent != 0 ? *foundEvent : 0;
        ((UnidentifiedTypedEvent<UnidentifiedEventData_8006649C>*)event)
            ->Add(callback, 0, -1);
    }
    {
        Function<FnVoidVoid> callback(UnidentifiedMakeReplayBinding(
            &ReplayManager::fn_80189620, this));
        UnidentifiedEventBase** foundEvent;
        unsigned int hash;
        hash = HashEventName("Kickoff", -1);
        foundEvent = 0;
        g_pEventRegistry->Find(hash, &foundEvent, 0);
        UnidentifiedEventBase* event = foundEvent != 0 ? *foundEvent : 0;
        ((UnidentifiedTypedEvent<UnidentifiedEventNoData>*)event)
            ->Add(callback, 0, -1);
    }
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

void ReplayManager::fn_801895C0(ReceiveBallData* event)
{
    mEvents |= 4;
}

void ReplayManager::fn_801895D0(UnidentifiedEventData_80066590* event)
{
    mEvents |= 2;
}

void ReplayManager::fn_801895E0(UnidentifiedEventData_800663A8* event)
{
    mEvents |= 8;
}

void ReplayManager::fn_801895F0(GoalScoredData* event)
{
    if (event->uGoalType != 6)
    {
        mEvents |= 1;
    }
}

void ReplayManager::fn_80189610(UnidentifiedEventData_8006649C* event)
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
        mBlend[i] = GetFixedUpdateTask()->mUnidentified28 / g_fFixedUpdateTick;
    }
    mDeltaTime = GetFixedUpdateTask()->mUnidentified28;

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

extern "C" bool fn_80194660(cCharacter* character)
{
    return character->mUnidentified024.m_eCharacterClass == 13;
}
