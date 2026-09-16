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
#include "Game/SAnim/tu_8030E550.h"
#include "Game/DB/SaveLoad.h"
#include "Game/Render/Presentation.h"
#include "NL/plat/nlFlash.h"
#include "NL/nlPrint.h"
#include "Game/Character.h"
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

extern float g_fSimulationTick;
extern float g_fFixedUpdateTick;
extern bool lbl_806E14B8;
extern bool lbl_806E14B9;
extern float lbl_806E14CC;
extern bool lbl_806E14D1;
extern bool lbl_806E14D0;

extern "C"
{
    float fn_80189870();
}

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

void ReplayManager::fn_80188D88()
{
    UnidentifiedFindEvent<ReceiveBallData>("ReceiveBall", -1)->Add(Function<ReceiveBallData*>(UnidentifiedMakeReplayBinding(&ReplayManager::fn_801895C0, this)), 0, -1);
    UnidentifiedFindEvent<UnidentifiedEventData_80066590>("ShotAtGoal", -1)->Add(Function<UnidentifiedEventData_80066590*>(UnidentifiedMakeReplayBinding(&ReplayManager::fn_801895D0, this)), 0, -1);
    UnidentifiedFindEvent<UnidentifiedEventData_800663A8>("PassBall", -1)->Add(Function<UnidentifiedEventData_800663A8*>(UnidentifiedMakeReplayBinding(&ReplayManager::fn_801895E0, this)), 0, -1);
    UnidentifiedFindEvent<GoalScoredData>("GoalScored", -1)->Add(Function<GoalScoredData*>(UnidentifiedMakeReplayBinding(&ReplayManager::fn_801895F0, this)), 0, -1);
    UnidentifiedFindEvent<GoalieSaveData>("GoalieSave", -1)->Add(Function<GoalieSaveData*>(UnidentifiedMakeReplayBinding(&ReplayManager::fn_80189610, this)), 0, -1);
    UnidentifiedFindEvent<UnidentifiedEventNoData>("Kickoff", -1)->Add(Function<FnVoidVoid>(UnidentifiedMakeReplayBinding(&ReplayManager::fn_80189620, this)), 0, -1);
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

void ReplayManager::fn_80189610(GoalieSaveData* event)
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

extern "C" float fn_80189870()
{
    static TweakFloatBinding unidentifiedScale(
        "/Camera/Debug Cam/Revolution/Revolution Accelerometer Scale", 1.0f);

    float acceleration = g_pPlatPadManager->GetFreestyleStatus(0)->wpad.accX;
    float speed;
    if (lbl_806E14B8 == 1)
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

bool ReplayManager::fn_8018A4B4() const
{
    return lbl_806E14D0;
}

bool ReplayManager::fn_8018A4BC() const
{
    return lbl_806E14D1;
}

int ReplayManager::fn_8018A16C(float time) const
{
    if (Instance()->fn_8018A4B4() || Instance()->fn_8018A4BC())
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

extern "C" bool fn_80194660(cCharacter* character)
{
    return character->mUnidentified024.m_eCharacterClass == 13;
}

const nlVector3& DrawableBall::fn_801925BC() const
{
    return mPosition;
}

int SaveFrame::GetInterval() const
{
    return mInterval;
}

void cPoseAccumulator::fn_801949E4(float scale)
{
    m_Scale = scale;
}

int LoadFrame::GetInterval() const
{
    return mInterval;
}

float LoadFrame::fn_801948B0() const
{
    return mNonBlendableAheadOfFrame;
}

static void fn_8018A43C(s32 result)
{
}

static void fn_8018A440(s32 result)
{
    lbl_806E14D0 = false;
    if (result < 0)
    {
        lbl_806E14B9 = true;
    }
    else
    {
        nlFlashClose(fn_8018A43C);
    }
}

bool ReplayManager::fn_8018A4C4(int index)
{
    if (SaveError == 1 || lbl_806E14B9 == 1)
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

    lbl_806E14D0 = true;
    nlFlashWrite(mMemory, 0x100000, fn_8018A440);
    return lbl_806E14D0;
}

static void fn_8018A46C(s32 result)
{
    lbl_806E14D1 = false;
    if (result < 0)
    {
        lbl_806E14B9 = true;
        GetPresentation()->SendSkipNis();
    }
    else
    {
        nlFlashClose(0);
    }
}

bool ReplayManager::fn_8018A5BC(int index)
{
    if (SaveError == 1 || lbl_806E14B9 == 1)
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

    lbl_806E14D1 = true;
    u32 size = 0x100000;
    if (nlFlashRead((void**)&mMemory, &size, fn_8018A46C, true) != 0)
    {
        GetPresentation()->SendSkipNis();
        return false;
    }
    return lbl_806E14D1;
}

extern bool lbl_806E14C0;
extern float lbl_806E14C4;
extern nlVector3 lbl_80570CA0;

template <int N>
void Replayable(LoadFrame& frame, char typeId, cPoseNode*& poseNode)
{
    if (N == 0 || frame.mInterval == N)
    {
        if (typeId == 0)
        {
            cPN_Blender* blender = new cPN_Blender;
            blender->Replay(frame);
            poseNode = blender;
        }
        else if (typeId == 1)
        {
            cPN_Feather* feather = new cPN_Feather;
            feather->Replay(frame);
            poseNode = feather;
        }
        else if (typeId == 2)
        {
            cPN_SAnimController* controller = new cPN_SAnimController;
            controller->Replay(frame);
            poseNode = controller;
        }
        else if (typeId == 3)
        {
            cPN_SingleAxisBlender* singleAxis = new cPN_SingleAxisBlender;
            singleAxis->Replay(frame);
            poseNode = singleAxis;
        }
        else if (typeId == 4)
        {
            cPN_8030E550* node = new cPN_8030E550;
            node->Replay(frame);
            poseNode = node;
        }
    }
}

template <int N>
void Replayable(SaveFrame& frame, char typeId, cPoseNode*& poseNode)
{
    if (N == 0 || frame.mInterval == N)
    {
        if (typeId < 0 || typeId >= 5)
            nlBreak();

        if (typeId == 0)
        {
            cPN_Blender* pn = (cPN_Blender*)poseNode;
            pn->Replay(frame);
        }
        else if (typeId == 1)
        {
            cPN_Feather* pn = (cPN_Feather*)poseNode;
            pn->Replay(frame);
        }
        else if (typeId == 2)
        {
            cPN_SAnimController* pn = (cPN_SAnimController*)poseNode;
            pn->Replay(frame);
        }
        else if (typeId == 3)
        {
            cPN_SingleAxisBlender* pn = (cPN_SingleAxisBlender*)poseNode;
            pn->Replay(frame);
        }
        else if (typeId == 4)
        {
            cPN_8030E550* pn = (cPN_8030E550*)poseNode;
            pn->Replay(frame);
        }
    }
}

template <typename T>
void DrawableCharacter::Replay(T& frame)
{
    bool usePoseAccumulator = false;
    Replayable<1>(frame, visible);
    Replayable<1>(frame, facingDirection);
    Replayable<1>(frame, FloatCompressor<-128, 128, 8>(position.x));
    Replayable<1>(frame, FloatCompressor<-128, 128, 8>(position.y));
    Replayable<1>(frame, FloatCompressor<-128, 128, 8>(position.z));
    if (character != 0)
    {
        Replayable<1>(frame, useObject);
        usePoseAccumulator = useObject;
        Replayable<1>(frame, FloatCompressor<0, 1, 8>(damage1));
        Replayable<1>(frame, FloatCompressor<0, 1, 8>(damage2));
        Replayable<1>(frame, damageType);
        Replayable<1>(frame, FloatCompressor<-128, 128, 8>(bip01Position.x));
        Replayable<1>(frame, FloatCompressor<-128, 128, 8>(bip01Position.y));
        Replayable<1>(frame, FloatCompressor<-128, 128, 8>(bip01Position.z));
        Replayable<1>(frame, FloatCompressor<-128, 128, 8>(headPosition.x));
        Replayable<1>(frame, FloatCompressor<-128, 128, 8>(headPosition.y));
        Replayable<1>(frame, FloatCompressor<-128, 128, 8>(headPosition.z));
        Replayable<1>(frame, FloatCompressor<-512, 512, 8>(velocity.x));
        Replayable<1>(frame, FloatCompressor<-512, 512, 8>(velocity.y));
        Replayable<1>(frame, FloatCompressor<-512, 512, 8>(velocity.z));
        Replayable<1>(frame, (unsigned long&)effectsTexturing);
        Replayable<1>(frame, FloatCompressor<0, 7, 13>(scale));
        if (ReplayFrameTraits<T>::IsLoadFrame)
            poseAccumulator->fn_801949E4(scale);
        Replayable<1>(frame, FloatCompressor<0, 1, 7>(blendAmount));
        Replayable<1>(frame, FloatCompressor<0, 7, 5>(state40));
        Replayable<1>(frame, FloatCompressor<0, 1, 7>(shadowLevel));
        Replayable<1>(frame, typeIsOne);
        if (fn_8019464C(character))
        {
            cCharacter* current = character;
            if (fn_80194660(current))
            {
                Replayable<1>(frame, megaEnabled);
                if (megaEnabled)
                {
                    Replayable<1>(frame, FloatCompressor<-128, 128, 8>(megaTranslation.x));
                    Replayable<1>(frame, FloatCompressor<-128, 128, 8>(megaTranslation.y));
                    Replayable<1>(frame, FloatCompressor<-128, 128, 8>(megaTranslation.z));
                    Replayable<1>(frame, UnidentifiedQuaternionCompressor(megaBasis));
                    Replayable<1>(frame, FloatCompressor<0, 7, 5>(megaScale));
                }
                else
                {
                    Replayable<1>(frame, flag3);
                }
                Replayable<1>(frame, flag2);
            }
            else if (fn_80194674(current))
            {
                Replayable<1>(frame, flag5);
                Replayable<1>(frame, flag6);
            }
        }
        if (!usePoseAccumulator && frame.GetInterval() == 1)
        {
            unsigned short headAngles;
            if (!ReplayFrameTraits<T>::IsLoadFrame)
                headAngles = (headSpin >> 8) | (headTilt != 0);
            Replayable<1>(frame, headAngles);
            if (ReplayFrameTraits<T>::IsLoadFrame)
            {
                headSpin = (headAngles & 0xFF) << 8;
                headTilt = headAngles & 0xFF00;
            }
        }
    }
    if (!usePoseAccumulator)
    {
        ReplayablePolymorphic<1>(frame, object);
        if (ReplayFrameTraits<T>::IsLoadFrame && frame.GetInterval() == 1)
        {
            poseAccumulator->InitAccumulators();
            object->Evaluate(1.0f, poseAccumulator);
            BuildNodeMatrices(poseAccumulator);
            delete object;
            object = 0;
        }
    }
    else if (frame.GetInterval() == 1)
    {
        Replayable<1>(frame, *poseAccumulator);
        if (ReplayFrameTraits<T>::IsLoadFrame)
            BuildNpcMatrix();
    }
}

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
    if (NetMesh::fn_801919B8())
    {
        if (ReplayFrameTraits<T>::IsLoadFrame
            && ((LoadFrame&)frame).GetInterval() == 1)
        {
            if (lbl_806E14C0)
            {
                lbl_806E14C0 = false;
                NetMesh::fn_801919C0()->Update(g_fFixedUpdateTick,
                    mBall.fn_801925BC(),
                    lbl_80570CA0,
                    _2430,
                    0);
                NetMesh::fn_801919C8()->Update(g_fFixedUpdateTick,
                    mBall.fn_801925BC(),
                    lbl_80570CA0,
                    _2431,
                    0);
                lbl_80570CA0 = mBall.fn_801925BC();
                mpNetMeshPositiveX->Grab(*PhysicsNet::fn_801949D4()->fn_801949CC());
                mpNetMeshNegativeX->Grab(*PhysicsNet::fn_801949DC()->fn_801949CC());
            }
            if (((LoadFrame&)frame).fn_801948B0() > 0.0f)
            {
                if (((LoadFrame&)frame).fn_801948B0() < lbl_806E14C4)
                {
                    mpNetMeshPositiveX->Grab(*PhysicsNet::fn_801949D4()->fn_801949CC());
                    mpNetMeshNegativeX->Grab(*PhysicsNet::fn_801949DC()->fn_801949CC());
                    lbl_806E14C0 = true;
                }
                lbl_806E14C4 = ((LoadFrame&)frame).fn_801948B0();
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

void SaveFrame::fn_80191504()
{
}

bool SaveFrame::fn_801919D0() const
{
    return true;
}

void LoadFrame::fn_80191504()
{
}

bool LoadFrame::fn_801919D0() const
{
    return mReplayNonBlendables == REPLAY_NON_BLENDABLES;
}

NPCManager* NPCManager::fn_801948A0()
{
    return gNPCManager;
}

DiddyBanana* NPCManager::fn_801919A4() const
{
    return mpDiddyBanana;
}

CrowdManager& CrowdManager::fn_801919AC()
{
    return instance;
}

bool NetMesh::fn_801919B8()
{
    return s_bAnimatedNetMeshEnabled;
}


NetMesh* NetMesh::fn_801919C0()
{
    return spPositiveXNetMesh;
}

NetMesh* NetMesh::fn_801919C8()
{
    return spNegativeXNetMesh;
}

NetMesh* PhysicsNet::fn_801949CC() const
{
    return mpNetMesh;
}

PhysicsNet* PhysicsNet::fn_801949D4()
{
    return spPhysNetPositiveX;
}

PhysicsNet* PhysicsNet::fn_801949DC()
{
    return spPhysNetNegativeX;
}
