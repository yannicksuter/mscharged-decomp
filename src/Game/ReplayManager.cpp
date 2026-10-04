#include "NL/plat/PlatPadManager.h"
#include "NL/plat/WiiRemotePad.h"
#include "NL/plat/WiiFreestylePad.h"

#include "Game/Render/NPCManager.h"
#include "Game/Render/CrowdManager.h"
#include "Game/Render/NetMesh.h"
#include "Game/Render/ShootToScoreArrow.h"
#include "Game/Effects/EmissionManager.h"
#include "Game/Physics/PhysicsNet.h"

#include "Game/ReplayManager.h"
#include "Game/PoseNode.h"
#include "Game/SAnim/pnBlender.h"
#include "Game/SAnim/pnFeather.h"
#include "Game/SAnim/pnSAnimController.h"
#include "Game/SAnim/pnSingleAxisBlender.h"
#include "Game/SAnim/pnScaleBlender.h"
#include "Game/DB/SaveLoad.h"
#include "Game/Render/Presentation.h"
#include "NL/plat/nlFlash.h"
#include "NL/nlPrint.h"
#include "Game/Character.h"
#include "Game/CharacterQueries.h"
#include "Game/MathHelpers.h"
#include "Game/Sys/tweak.h"
#include "Game/TweakValue.h"

#include "Game/Camera/CameraMan.h"
#include "Game/Camera/DebugCam.h"
#include "Game/Event.h"
#include "Game/EventRegistry.h"
#include "Game/ExcitementSystem.h"
#include "Game/GameEventQueue.h"
#include "Game/ReplayChoreo.h"
#include "Game/Task/FixedUpdateTask.h"
#include "Game/Task/ProfilerTask.h"
#include "Game/Task/TweakerTask.h"
#include "NL/nlBind.h"
#include "NL/nlFunction.inl"
#include "NL/nlConfig.h"
#include "NL/globalpad.h"
#include "NL/nlMemory.h"
#include "NL/nlTask.h"
#include "Game/InputManager.h"

#include "Game/UnidentifiedStaticStorage.h"


float GetRemoteReplaySpeed();

bool gbDebugReplayFixedStepMode;
bool gbReplayFlashError;
extern float lbl_806E14CC;
extern bool gbSavingReplay;
extern bool gbLoadingReplay;

ReplayManager::ReplayManager()
    : mCurrent(mSnapshots)
    , mPrevious(mSnapshots + 1)
    , mRender(0)
    , mDebugCamera(cFollowCamera::FOLLOW_SELECTABLE)
    , mReplayDebugCamera(0)
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

bool gbNetMeshReplayResync;
float gfLastAheadOfFrameTime;
nlVector3 gLastReplayBallPosition;

template <typename T>
void RenderSnapshot::Replay(T& frame)
{
    for (int i = 0; i < 10; i++)
        Replayable<0>(frame, mCharacters[i]);
    frame.fn_80191504();
    for (int i = 0; i < 150; i++)
        Replayable<0>(frame, mPowerups[i]);
    frame.fn_80191504();
    frame.fn_80191504();
    frame.fn_80191504();
    if ((_2714 >> 30) & 1)
    {
        Replayable<0>(frame, _2294);
        for (unsigned int i = 0; i < _2294; i++)
            Replayable<0>(frame, _2298[i]);
    }
    frame.fn_80191504();
    if ((_2714 >> 29) & 1)
    {
        Replayable<0>(frame, _1DA0);
        if (_1DA0)
            for (unsigned int i = 0; i < 15; i++)
                Replayable<0>(frame, _1DA4[i]);
        frame.fn_80191504();
    }
    if ((_2714 >> 22) & 1)
        for (unsigned int i = 0; i < 8; i++)
            Replayable<0>(frame, _2194[i]);
    if ((_2714 >> 28) & 1)
    {
        Replayable<0>(frame, _1CC4);
        for (unsigned int i = 0; i < _1CC4; i++)
            Replayable<0>(frame, _1CC8[i]);
        frame.fn_80191504();
    }
    Replayable<0>(frame, mChainChomp);
    if ((_2714 >> 24) & 1)
    {
        if (NPCManager::fn_801948A0()->fn_801919A4() != 0)
            Replayable<0>(frame, mBowser);
    }
    if ((_2714 >> 25) & 1)
    {
        Replayable<0>(frame, _1C00);
        for (unsigned int i = 0; i < _1C00; i++)
            Replayable<0>(frame, mDaisyFists[i]);
    }
    if ((_2714 >> 31) & 1)
        Replayable<0>(frame, _1BA0);
    if ((_2714 >> 26) & 1)
        Replayable<0>(frame, _1BC4);
    if ((_2714 >> 27) & 1)
        Replayable<0>(frame, _1BE8);
    Replayable<0>(frame, mBall);
    Replayable<1>(frame, mCameraUp);
    Replayable<1>(frame, mGoalLight);
    Replayable<1>(frame, CrowdManager::fn_801919AC());
    Replayable<0>(frame, WorldDarkening::Instance());
    if ((_2714 >> 23) & 1)
    {
        for (int i = 0; i < 3; i++)
        {
            if (NPCManager::fn_801948A0()->fn_801A9DE0(i) != 0)
                Replayable<0>(frame, _1FC0[i]);
        }
        frame.fn_80191504();
    }
    frame.fn_80191504();
    if (NetMesh::IsAnimatedNetMeshEnabled())
    {
        if (ReplayFrameTraits<T>::IsLoadFrame
            && ((LoadFrame&)frame).GetInterval() == 1)
        {
            if (gbNetMeshReplayResync)
            {
                gbNetMeshReplayResync = false;
                NetMesh::GetPositiveXNetMesh()->Update(g_fFixedUpdateTick,
                    mBall.fn_801925BC(),
                    gLastReplayBallPosition,
                    _2430,
                    0);
                NetMesh::GetNegativeXNetMesh()->Update(g_fFixedUpdateTick,
                    mBall.fn_801925BC(),
                    gLastReplayBallPosition,
                    _2431,
                    0);
                gLastReplayBallPosition = mBall.fn_801925BC();
                mpNetMeshPositiveX->Grab(*PhysicsNet::fn_801949D4()->fn_801949CC());
                mpNetMeshNegativeX->Grab(*PhysicsNet::fn_801949DC()->fn_801949CC());
            }
            if (((LoadFrame&)frame).fn_801948B0() > 0.0f)
            {
                if (((LoadFrame&)frame).fn_801948B0() < gfLastAheadOfFrameTime)
                {
                    mpNetMeshPositiveX->Grab(*PhysicsNet::fn_801949D4()->fn_801949CC());
                    mpNetMeshNegativeX->Grab(*PhysicsNet::fn_801949DC()->fn_801949CC());
                    gbNetMeshReplayResync = true;
                }
                gfLastAheadOfFrameTime = ((LoadFrame&)frame).fn_801948B0();
            }
        }
        Replayable<1>(frame, *mpNetMeshPositiveX);
        Replayable<1>(frame, *mpNetMeshNegativeX);
    }
    Replayable<1>(frame, _2718);
    frame.fn_80191504();
    if (frame.fn_801919D0())
    {
        Replayable<3>(frame, EmissionManager::Instance()->InstanceForReplayOnly());
    }
    mValid = true;
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

void ReplayManager::RegisterEventHandlers()
{
    UnidentifiedFindEvent<ReceiveBallData>("ReceiveBall", -1)->Add(Function<ReceiveBallData*>(UnidentifiedMakeReplayBinding(&ReplayManager::OnReceiveBall, this)), 0, -1);
    UnidentifiedFindEvent<UnidentifiedEventData_80066590>("ShotAtGoal", -1)->Add(Function<UnidentifiedEventData_80066590*>(UnidentifiedMakeReplayBinding(&ReplayManager::OnShotAtGoal, this)), 0, -1);
    UnidentifiedFindEvent<UnidentifiedEventData_800663A8>("PassBall", -1)->Add(Function<UnidentifiedEventData_800663A8*>(UnidentifiedMakeReplayBinding(&ReplayManager::OnPassBall, this)), 0, -1);
    UnidentifiedFindEvent<GoalScoredData>("GoalScored", -1)->Add(Function<GoalScoredData*>(UnidentifiedMakeReplayBinding(&ReplayManager::OnGoalScored, this)), 0, -1);
    UnidentifiedFindEvent<GoalieSaveData>("GoalieSave", -1)->Add(Function<GoalieSaveData*>(UnidentifiedMakeReplayBinding(&ReplayManager::OnGoalieSave, this)), 0, -1);
    UnidentifiedFindEvent<UnidentifiedEventNoData>("Kickoff", -1)->Add(Function<FnVoidVoid>(UnidentifiedMakeReplayBinding(&ReplayManager::OnKickoff, this)), 0, -1);
}

void ReplayManager::InitializeSnapshots()
{
    for (int i = 0; i < 3; i++)
    {
        mSnapshots[i].Initialize();
    }
}

void ReplayManager::OnMegaStrikeResult()
{
    mEvents |= 0x40;
}

void ReplayManager::OnReceiveBall(ReceiveBallData* event)
{
    mEvents |= 4;
}

void ReplayManager::OnShotAtGoal(UnidentifiedEventData_80066590* event)
{
    mEvents |= 2;
}

void ReplayManager::OnPassBall(UnidentifiedEventData_800663A8* event)
{
    mEvents |= 8;
}

void ReplayManager::OnGoalScored(GoalScoredData* event)
{
    if (event->uGoalType != 6)
    {
        mEvents |= 1;
    }
}

void ReplayManager::OnGoalieSave(GoalieSaveData* event)
{
    mEvents |= 0x11;
}

void ReplayManager::OnKickoff()
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
        unsigned int excitement = ExcitementSystem::Instance().mExcitement;
        excitement <<= 16;
        excitement += ExcitementSystem::Instance().mExcitementCount;
        mReplay->Record<RenderSnapshot>(mTime, *mCurrent, mEvents, excitement);

        ExcitementSystem& state = ExcitementSystem::Instance();
        state.mExcitement = 0;
        state.mExcitementCount = 0;
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

#include "Game/Render/NPCManager.inl"

void ReplayManager::DoPotentialAutoReplay(float deltaTime)
{
    if (nlTaskManager::m_pInstance->mCurrentState == 8 && !gbLoadingReplay)
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

float GetRemoteReplaySpeed()
{
    static TweakFloatBinding unidentifiedScale(
        "/Camera/Debug Cam/Revolution/Revolution Accelerometer Scale", 1.0f);

    float acceleration = g_pPlatPadManager->GetFreestyleStatus(0)->wpad.accX;
    float speed;
    if (gbDebugReplayFixedStepMode == 1)
    {
        speed = acceleration * unidentifiedScale;
        speed = nlMaxEquals(speed, -1.0f);
        speed = nlMinEquals(speed, 1.0f);
    }
    else
    {
        speed = acceleration * unidentifiedScale;
        speed = (1.0f + speed) / 2.0f;
        if (speed < 0.0f)
        {
            speed = 0.0f;
        }
        if (speed > 1.0f)
        {
            speed = 1.0f;
        }
    }

    nlScreenPrintf(0, 1, false, 0, "Replay Speed %0.1f", speed);
    return speed;
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
            if (mReplayDebugCamera != 0)
            {
                cCameraManager::PopCamera();
                delete mReplayDebugCamera;
                mReplayDebugCamera = 0;
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
            mReplayDebugCamera
                = new (nlMalloc(0xA0, 8, false)) cDebugCamera(true);
            cCameraManager::PushCamera(mReplayDebugCamera);
        }

        mDeltaTime = 0.0f;
        if (gbDebugReplayFixedStepMode == 1)
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
            mDeltaTime *= GetRemoteReplaySpeed();
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

float lbl_806E14CC;
bool gbSavingReplay;
bool gbLoadingReplay;

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
        mBlend[i] = GetFixedUpdateTask()->mInterpolationDeltaT / g_fFixedUpdateTick;
    }
    mDeltaTime = GetFixedUpdateTask()->mInterpolationDeltaT;

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

int ReplayManager::GetReplayExcitement(float time) const
{
    if (Instance()->IsSavingReplay() || Instance()->IsLoadingReplay())
    {
        return 0;
    }

    Replay::Frame* begin;
    unsigned int value = 0;
    unsigned int count = 0;
    if (mReplay != 0 && mReplay->mReelIdx == 0)
    {
        begin = mReplay->mReels[mReplay->mReelIdx].mBegin;
        float beginTime = nlMaxEquals(time + mReplay->EndTime(), 0.0f);
        float goalTime = mReplay->TimeOfLastOccurence(1);
        float kickoffTime = mReplay->TimeOfLastOccurence(0x20);
        if (goalTime > kickoffTime)
        {
            beginTime = nlMaxEquals(goalTime + time, 0.0f);
        }
        beginTime = nlMaxEquals(beginTime, kickoffTime);

        float lastTime = 0.0f;
        for (Replay::Frame* frame = begin->mNext;
             frame != 0 && frame != begin; frame = frame->mNext)
        {
            if (frame->mTime > beginTime && frame->mTime <= mReplay->EndTime()
                && frame->mTime != lastTime && frame->mUnidentifiedState != 0)
            {
                lastTime = frame->mTime;
                value += frame->mUnidentifiedState >> 16;
                count += frame->mUnidentifiedState & 0xFFFF;
            }
        }
    }
    else if (mReplay != 0)
    {
        value = (unsigned short)mReplay->mReels[mReplay->mReelIdx].mAge;
        count = 1;
    }

    return (unsigned short)count * (unsigned short)value;
}

static void OnReplayFileClosed(s32 result)
{
}

static void OnReplayWritten(s32 result)
{
    gbSavingReplay = false;
    if (result < 0)
    {
        gbReplayFlashError = true;
    }
    else
    {
        nlFlashClose(OnReplayFileClosed);
    }
}

static void OnReplayRead(s32 result)
{
    gbLoadingReplay = false;
    if (result < 0)
    {
        gbReplayFlashError = true;
        GetPresentation()->SendSkipNis();
    }
    else
    {
        nlFlashClose(0);
    }
}

bool ReplayManager::IsSavingReplay() const
{
    return gbSavingReplay;
}

bool ReplayManager::IsLoadingReplay() const
{
    return gbLoadingReplay;
}

bool ReplayManager::SaveReplay(int index)
{
    if (SaveError == 1 || gbReplayFlashError == 1)
    {
        return false;
    }

    char name[10];
    nlSNPrintf(name, 10, "REPLAY_%d", index);
    if (nlFlashChangeDirectory(2, 0) != 0)
    {
        return false;
    }

    s32 result = nlFlashOpen(name, 2, 0);
    if (result != 0)
    {
        if (nlFlashCreate(name, 0x30, 0) != 0)
        {
            return false;
        }
        result = nlFlashOpen(name, 2, 0);
    }
    if (result != 0)
    {
        return false;
    }

    gbSavingReplay = true;
    nlFlashWrite(mMemory, 0x100000, OnReplayWritten);
    return gbSavingReplay;
}

bool ReplayManager::LoadReplay(int index)
{
    if (SaveError == 1 || gbReplayFlashError == 1)
    {
        GetPresentation()->SendSkipNis();
        return false;
    }

    char name[128];
    nlSNPrintf(name, 10, "REPLAY_%d", index);
    if (nlFlashChangeDirectory(2, 0) != 0)
    {
        GetPresentation()->SendSkipNis();
        return false;
    }
    if (nlFlashOpen(name, 1, 0) != 0)
    {
        GetPresentation()->SendSkipNis();
        return false;
    }

    gbLoadingReplay = true;
    u32 size = 0x100000;
    if (nlFlashRead((void**)&mMemory, &size, OnReplayRead, true) != 0)
    {
        GetPresentation()->SendSkipNis();
        return false;
    }
    return gbLoadingReplay;
}
