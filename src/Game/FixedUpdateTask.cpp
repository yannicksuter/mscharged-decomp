#include "Game/FixedUpdateTask.h"

#include "Game/AI/AiUtil.h"
#include "Game/Ball.h"
#include "Game/CharacterTemplate.h"
#include "Game/Field.h"
#include "Game/Game.h"
#include "Game/Goalie.h"
#include "Game/NetworkSession.h"
#include "Game/NetworkStatsManager.h"
#include "Game/Pad/FlickDetection.h"
#include "Game/Physics/Physics.h"
#include "Game/Physics/PhysicsAIBall.h"
#include "Game/Render/NPCManager.h"
#include "Game/Render/NetMesh.h"
#include "Game/ReplayManager.h"
#include "Game/Sys/clock.h"
#include "Game/Team.h"
#include "Game/UnidentifiedStaticStorage.h"
#include "NL/globalpad.h"
#include "NL/nlMain.h"
#include "NL/nlPrint.h"
#include "NL/nlString.h"
#include "types.h"

#include <math.h>

class DebugWriteCache;
struct UnidentifiedPeachPhotoState;

struct DebugFieldType
{
    unsigned short size;
    unsigned short unknown;
    void* writer;
};

struct UnclassifiedControllerGroup
{
    void* mUnidentified000;
    int mNumControllers;
};

// Deterministic input record logged for every controller.
struct DetInput
{
    /* 0x00 */ float m_AnalogLeftX;
    /* 0x04 */ float m_AnalogLeftY;
    /* 0x08 */ float m_AnalogRightX;
    /* 0x0C */ float m_AnalogRightY;
    /* 0x10 */ u8 m_nConnected;
    /* 0x12 */ u16 m_ButtonBitfield;
    /* 0x14 */ u8 m_LeftTrigger;
    /* 0x15 */ u8 m_RightTrigger;
    /* 0x18 */ nlVector3 m_v3RevRemoteAccel;
    /* 0x24 */ nlVector3 m_v3RevFreeStyleAccel;
    /* 0x30 */ int m_nRevDPDNumTargets;
    /* 0x34 */ nlVector2 m_v2RevDPDCoord;
    /* 0x3C */ void* m_pPrevInput;
    /* 0x40 */ void* m_pMyUser;
    /* 0x44 */ nlPolar m_PolarAnalogLeft;
    /* 0x4C */ nlPolar m_PolarAnalogRight;
    /* 0x54 */ int m_buttonStateTicks[13];
    /* 0x88 */ u16 m_aRemapAngle;
}; // size: 0x8C

extern u16 m_aJoystickRemap__14cCameraManager;
extern "C" bool lbl_806E180D;
extern "C" void* fn_80284A58();
extern "C" bool fn_80287AB0(void*);
extern "C" void fn_802C084C(void*, int);
extern "C" void fn_802C07AC(void*, float);
extern "C" void fn_8037537C(void*);
extern "C" int fn_803328B4(void*);
extern "C" void fn_803328FC(void*);
extern "C" bool fn_80332A00(void*);
extern "C" void* fn_803330AC();
extern "C" void fn_80333A18();
extern "C" void fn_8005A8FC(cGame*, float);
extern "C" void fn_80142A1C();
extern "C" void fn_801AD7E4(UnidentifiedPeachPhotoState*, float, int);
extern "C" void fn_800A8DE8(cTeam*, RunningChecksum*);
extern "C" void fn_8005B840(cGame*, void*, DebugWriteCache*);
extern "C" void fn_800A8900(cTeam*, void*, DebugWriteCache*);
extern "C" DebugWriteCache* fn_80338950(void*);
extern "C" void fn_80339544(DebugWriteCache*, u32 frame);
extern "C" void fn_80338D04(DebugWriteCache*, u16* type, const char* name, RunningChecksum*, float value);
extern "C" UnclassifiedControllerGroup* fn_80338BF8(void*, s8);
extern "C" void* fn_80336B6C(UnclassifiedControllerGroup*, int);
extern "C" cGlobalPad* fn_80336D68(void*);
extern "C" unsigned short fn_80338EBC(DebugWriteCache*, const char*);
extern "C" void fn_80338F78(DebugWriteCache*);
extern "C" void fn_80338F88(DebugWriteCache*, int, unsigned short, unsigned int, const char*);
extern "C" void fn_80339090(DebugWriteCache*, int, unsigned short, unsigned int, unsigned int, const char*);
extern "C" void* fn_8033930C(DebugWriteCache*, unsigned short, void*, unsigned int);
extern "C" void fn_80339450(DebugWriteCache*, unsigned short, void*, void*);
extern "C" void fn_8033919C(DebugWriteCache*, const char*);
extern "C" DebugFieldType lbl_80533C98[];
extern "C" u16 lbl_806DF740;
extern "C" int lbl_806E2130;
extern "C" UnidentifiedPeachPhotoState lbl_80573CA8;

extern void* lbl_806E1E28;
extern void* lbl_806E2478;
extern void* lbl_806E2138;
extern void* lbl_806E2168;

float g_fFixedUpdateTick = 0.02f;
bool g_bRunSimAndRenderInLockStep;

static u16 sSimulationTimeType = 0xFFFF;
static u16 sTimeScaleType = 0xFFFF;

extern "C" void fn_80111654()
{
}

extern "C" void fn_80111658()
{
}

extern "C" void fn_8011165C()
{
}

extern "C" void fn_80111660()
{
}

extern "C" bool fn_80111664()
{
    return false;
}

static FixedUpdateTask fixedUpdateTask;
float g_fSimulationTick = g_fFixedUpdateTick;

extern "C" FixedUpdateTask* fn_8011166C()
{
    return &fixedUpdateTask;
}

extern "C" EventDispatcher* fn_80111678()
{
    return &fixedUpdateTask.mEventDispatcher;
}

void FixedUpdateTask::Reset()
{
    mUnidentified28 = mAccumulatedDeltaT = g_fFixedUpdateTick;
    mSimulationTime = 0.0f;
    mTimeScale = 1.0f;
    mfFrameLockTime = 0.0f;
    mFrame = 0;
    mUnidentified38 = false;

    mEventDispatcher.Clear();
    BasicSlotPool<DLListEntry<EventCallback> >* pool = &mEventDispatcher.callbacks.m_Allocator;
    pool->FreeBlocks();
}

float FixedUpdateTask::GetFixedUpdateMilliseconds()
{
    return 1000.0f * g_fFixedUpdateTick;
}

u32 FixedUpdateTask::CalculateChecksum()
{
    RunningChecksum checksum;
    float simulationTime = fixedUpdateTask.mSimulationTime;
    checksum.ChecksumData(&simulationTime, sizeof(simulationTime));
    g_pGame->fn_8005BF50(&checksum);
    g_pBall->fn_8001A898(&checksum);
    for (int i = 0; i < 2; i++)
    {
        fn_800A8DE8(g_pTeams[i], &checksum);
    }
    return ~checksum.m_nChecksum;
}

static char sDetInputName[] = "DetInput";
static char sAnalogLeftXName[] = "m_AnalogLeftX";
static char sAnalogLeftYName[] = "m_AnalogLeftY";
static char sAnalogRightXName[] = "m_AnalogRightX";
static char sAnalogRightYName[] = "m_AnalogRightY";
static char sConnectedName[] = "m_nConnected";
static char sButtonBitfieldName[] = "m_ButtonBitfield";
static char sLeftTriggerName[] = "m_LeftTrigger";
static char sRightTriggerName[] = "m_RightTrigger";
static char sRevRemoteAccelName[] = "m_v3RevRemoteAccel";
static char sRevFreeStyleAccelName[] = "m_v3RevFreeStyleAccel";
static char sRevDPDNumTargetsName[] = "m_nRevDPDNumTargets";
static char sRevDPDCoordName[] = "m_v2RevDPDCoord";
static char sPrevInputName[] = "m_pPrevInput";
static char sMyUserName[] = "m_pMyUser";
static char sPolarAnalogLeftAName[] = "m_PolarAnalogLeft.a";
static char sPolarAnalogLeftRName[] = "m_PolarAnalogLeft.r";
static char sPolarAnalogRightAName[] = "m_PolarAnalogLeft.a";
static char sPolarAnalogRightRName[] = "m_PolarAnalogLeft.r";
static char sButtonStateTicksName[] = "m_buttonStateTicks";
static char sRemapAngleName[] = "m_aRemapAngle";

#define PAD_FIELD_OFFSET(pad, field) \
    ((unsigned char*)&(pad)->field - (unsigned char*)(pad))

u32 FixedUpdateTask::WriteSyncLog()
{
    DebugWriteCache* cache = fn_80338950(lbl_806E2168);
    if (cache == 0)
    {
        return 0;
    }

    fn_80339544(cache, GetFrame());

    RunningChecksum checksum;
    fn_80338D04(cache, &sSimulationTimeType, "simulationTime", &checksum,
        fixedUpdateTask.mSimulationTime);
    fn_80338D04(cache, &sTimeScaleType, "timeScale", &checksum,
        fixedUpdateTask.mTargetTimeScale);

    int numGroups = fn_80338BF0(lbl_806E20D8);
    for (int groupIndex = 0; groupIndex < numGroups; groupIndex++)
    {
        UnclassifiedControllerGroup* group = fn_80338BF8(lbl_806E20D8, (s8)groupIndex);
        int numControllers = group->mNumControllers;
        for (int controllerIndex = 0; controllerIndex < numControllers; controllerIndex++)
        {
            DetInput* pad =
                (DetInput*)fn_80336D68(fn_80336B6C(group, controllerIndex));
            if (lbl_806DF740 == 0xFFFF)
            {
                lbl_806DF740 = fn_80338EBC(cache, sDetInputName);
                fn_80338F88(cache, 17, lbl_80533C98[17].size, 0, sAnalogLeftXName);
                fn_80338F88(cache, 17, lbl_80533C98[17].size, PAD_FIELD_OFFSET(pad, m_AnalogLeftY), sAnalogLeftYName);
                fn_80338F88(cache, 17, lbl_80533C98[17].size, PAD_FIELD_OFFSET(pad, m_AnalogRightX), sAnalogRightXName);
                fn_80338F88(cache, 17, lbl_80533C98[17].size, PAD_FIELD_OFFSET(pad, m_AnalogRightY), sAnalogRightYName);
                fn_80338F88(cache, 0, lbl_80533C98[0].size, PAD_FIELD_OFFSET(pad, m_nConnected), sConnectedName);
                fn_80338F88(cache, 1, lbl_80533C98[1].size, PAD_FIELD_OFFSET(pad, m_ButtonBitfield), sButtonBitfieldName);
                fn_80338F88(cache, 0, lbl_80533C98[0].size, PAD_FIELD_OFFSET(pad, m_LeftTrigger), sLeftTriggerName);
                fn_80338F88(cache, 0, lbl_80533C98[0].size, PAD_FIELD_OFFSET(pad, m_RightTrigger), sRightTriggerName);
                fn_80338F88(cache, 22, lbl_80533C98[22].size, PAD_FIELD_OFFSET(pad, m_v3RevRemoteAccel), sRevRemoteAccelName);
                fn_80338F88(cache, 22, lbl_80533C98[22].size, PAD_FIELD_OFFSET(pad, m_v3RevFreeStyleAccel), sRevFreeStyleAccelName);
                fn_80338F88(cache, 0, lbl_80533C98[0].size, PAD_FIELD_OFFSET(pad, m_nRevDPDNumTargets), sRevDPDNumTargetsName);
                fn_80338F88(cache, 21, lbl_80533C98[21].size, PAD_FIELD_OFFSET(pad, m_v2RevDPDCoord), sRevDPDCoordName);
                fn_80338F88(cache, 15, lbl_80533C98[15].size, PAD_FIELD_OFFSET(pad, m_pPrevInput), sPrevInputName);
                fn_80338F88(cache, 15, lbl_80533C98[15].size, PAD_FIELD_OFFSET(pad, m_pMyUser), sMyUserName);
                fn_80338F88(cache, 19, lbl_80533C98[19].size, PAD_FIELD_OFFSET(pad, m_PolarAnalogLeft.a), sPolarAnalogLeftAName);
                fn_80338F88(cache, 17, lbl_80533C98[17].size, PAD_FIELD_OFFSET(pad, m_PolarAnalogLeft.r), sPolarAnalogLeftRName);
                fn_80338F88(cache, 19, lbl_80533C98[19].size, PAD_FIELD_OFFSET(pad, m_PolarAnalogLeft.a), sPolarAnalogRightAName);
                fn_80338F88(cache, 17, lbl_80533C98[17].size, PAD_FIELD_OFFSET(pad, m_PolarAnalogLeft.r), sPolarAnalogRightRName);
                fn_80339090(cache, 8, lbl_80533C98[8].size, 13, PAD_FIELD_OFFSET(pad, m_buttonStateTicks), sButtonStateTicksName);
                fn_80338F88(cache, 19, lbl_80533C98[19].size, PAD_FIELD_OFFSET(pad, m_aRemapAngle), sRemapAngleName);
                fn_80338F78(cache);
            }

            DetInput* copy =
                (DetInput*)fn_8033930C(cache, lbl_806DF740, pad, sizeof(DetInput));
            if (copy != 0)
            {
                copy->m_pPrevInput = 0;
                copy->m_pMyUser = (void*)((cGlobalPad*)pad)->fn_80332748();
                fn_80339450(cache, lbl_806DF740, copy, &checksum);
            }
        }
    }

    fn_8005B840(g_pGame, &checksum, cache);
    g_pBall->SyncLog(&checksum, cache);
    for (int i = 0; i < 2; i++)
    {
        fn_800A8900(g_pTeams[i], &checksum, cache);
    }
    g_PhysicsWorld->SyncLog(&checksum, cache);

    u32 crc = ~checksum.m_nChecksum;
    char buffer[0x100];
    nlSNPrintf(buffer, sizeof(buffer),
        "------------------------ END Frame:%d CRC:%x -------------------------\n\n",
        GetFrame(), crc);
    fn_8033919C(cache, buffer);
    return crc;
}

#undef PAD_FIELD_OFFSET

void FixedUpdateTask::UnidentifiedVirtual10()
{
    if (lbl_806E10EC->OnlineVirtual0C())
    {
        NetworkStatsManager_8012F378::Instance()->CalculateAndReportGameResult(2);
    }
    lbl_806E10EC->fn_80123FBC(1);
}

void FixedUpdateTask::UnidentifiedVirtual14()
{
    NetworkStatsManager_8012F378::Instance()->CalculateAndReportGameResult(4);
    lbl_806E10EC->fn_80123FBC(2);
}

u16 FixedUpdateTask::UnidentifiedVirtual18()
{
    return m_aJoystickRemap__14cCameraManager - 0x4000;
}

bool FixedUpdateTask::UnidentifiedVirtual1C()
{
    return lbl_806E180D;
}

float FixedUpdateTask::GetPhysicsUpdateTick()
{
    return g_fSimulationTick;
}

void FixedUpdateTask::SetTimeScale(float timeScale)
{
    fixedUpdateTask.mTimeScale = timeScale;
    fixedUpdateTask.mTargetTimeScale = timeScale;
}

float FixedUpdateTask::GetTargetTimeScale()
{
    return fixedUpdateTask.mTargetTimeScale;
}

void FixedUpdateTask::SetTimeScale(float timeScale, float transitionTime)
{
    fixedUpdateTask.mTimeScaleTransitionTime = transitionTime;
    fixedUpdateTask.mTimeScaleTransitionRemaining = transitionTime;
    fixedUpdateTask.mTimeScaleTransitionStart = fixedUpdateTask.mTimeScale;
    fixedUpdateTask.mTargetTimeScale = timeScale;
}

float FixedUpdateTask::GetTimeScale()
{
    return fixedUpdateTask.mTimeScale;
}

void FixedUpdateTask::SetFrameLock(float frameLockTime)
{
    fixedUpdateTask.mfFrameLockTime = frameLockTime;
    nlTaskManager::SetNextState(1);
}

void FixedUpdateTask::DecrementFrameLock(float fDeltaT)
{
    fixedUpdateTask.mfFrameLockTime -= fDeltaT;
    if (fixedUpdateTask.mfFrameLockTime < 0.0f)
    {
        if (nlTaskManager::m_pInstance->mCurrentState == 1
            && nlTaskManager::m_pInstance->mPendingState != 16)
        {
            nlTaskManager::SetNextState(2);
        }
        fixedUpdateTask.mfFrameLockTime = 0.0f;
    }
}

void FixedUpdateTask::Run(float dt)
{
    bool runFixedUpdate = true;
    if (fn_80287AB0(fn_80284A58()))
    {
        runFixedUpdate = false;
    }
    if (!mUnidentified38)
    {
        runFixedUpdate = false;
    }
    if (nlTaskManager::m_pInstance->mPendingState == 16)
    {
        runFixedUpdate = false;
    }

    if (runFixedUpdate
        && nlTaskManager::m_pInstance->mCurrentState == 2
        && !lbl_806E10EC->fn_80123A00())
    {
        float simulationTick;

        if (mTimeScaleTransitionRemaining != 0.0f)
        {
            mTimeScaleTransitionRemaining -= dt * mTimeScale;
            if (mTimeScaleTransitionRemaining < 0.0f)
            {
                mTimeScaleTransitionRemaining = 0.0f;
            }

            float percent = 1.0f
                - mTimeScaleTransitionRemaining / mTimeScaleTransitionTime;
            mTimeScale = Interpolate(
                mTimeScaleTransitionStart, mTargetTimeScale, percent);
        }

        mAccumulatedDeltaT += dt * mTimeScale;
        mUnidentified28 = mAccumulatedDeltaT;

        while (g_bRunSimAndRenderInLockStep
            || mAccumulatedDeltaT >= g_fFixedUpdateTick)
        {
            fn_802C084C(lbl_806E1E28, 1);
            simulationTick = g_fFixedUpdateTick;
            fn_8037537C(lbl_806E2478);
            fn_802C07AC(lbl_806E1E28, simulationTick);
            FlickDetection::Update();

            mAccumulatedDeltaT -= g_fFixedUpdateTick;
            if (g_bRunSimAndRenderInLockStep)
            {
                mAccumulatedDeltaT = 0.0f;
            }

            int updateCount = fn_803328B4(lbl_806E2138);
            fn_803328FC(lbl_806E2138);

            bool updated = false;
            for (int i = 0; i < updateCount; ++i)
            {
                if (fn_80332A00(lbl_806E2138))
                {
                    CallFixedUpdateTasks();
                    updated = true;
                }
                if (fn_80287AB0(fn_80284A58()))
                {
                    break;
                }
            }

            if (updated)
            {
                mUnidentified28 = mAccumulatedDeltaT;
            }
            else
            {
                mUnidentified28 = g_fFixedUpdateTick;
            }

            if (fn_80287AB0(fn_80284A58()))
            {
                break;
            }
            if (g_bRunSimAndRenderInLockStep)
            {
                break;
            }
        }
    }

    fn_802C084C(lbl_806E1E28, 0);
    fn_8037537C(lbl_806E2478);
    fn_802C07AC(lbl_806E1E28, dt);
    FlickDetection::Update();
}

static void AIUpdateTask(float fDeltaT)
{
    g_pGame->PreUpdate(fDeltaT);
    fn_8005A8FC(g_pGame, fDeltaT);
}

static void PrePhysicsAITask(float fDeltaT)
{
    int i;
    for (i = 0; i < 10; i++)
    {
        g_pCharacters[i]->Unknown7(fDeltaT);
    }
}

static void PostPhysicsAITask(float fDeltaT)
{
    int i;
    for (i = 0; i < 10; i++)
    {
        g_pCharacters[i]->PrePhysicsUpdate();
    }
    g_pBall->PostPhysicsUpdate(fDeltaT);
}

void FixedUpdateTask::CallFixedUpdateTasks()
{
    ++lbl_806E2130;
    mFrame++;
    mSimulationTime += g_fSimulationTick;

    ClockManager::Update(g_fSimulationTick);
    fn_803330AC();
    fn_80333A18();

    AIUpdateTask(g_fSimulationTick);
    fn_80142A1C();
    lbl_806E1608->UpdateAINPCs(g_fSimulationTick);
    PrePhysicsAITask(g_fSimulationTick);
    PhysicsUpdate(g_PhysicsWorld, GetPhysicsUpdateTick());
    PostPhysicsAITask(g_fSimulationTick);

    if (NetMesh::s_bAnimatedNetMeshEnabled)
    {
        bool i = true;
        float goalieX = (float)fabs(g_pTeams[0]->GetGoalie()->m_v3Position.x);
        if (goalieX > cField::GetGoalLineX(1U))
        {
        }
        else
        {
            goalieX = (float)fabs(g_pTeams[1]->GetGoalie()->m_v3Position.x);
            if (goalieX > cField::GetGoalLineX(1U))
            {
            }
            else
            {
                i = false;
            }
        }

        NetMesh::spPositiveXNetMesh->Update(g_fSimulationTick, g_pBall->m_v3Position, g_pBall->m_v3PrevPosition, i, g_pBall->m_pPhysicsBall);
        NetMesh::spNegativeXNetMesh->Update(g_fSimulationTick, g_pBall->m_v3Position, g_pBall->m_v3PrevPosition, i, g_pBall->m_pPhysicsBall);
    }

    mEventDispatcher.Dispatch(true);
    fn_801AD7E4(&lbl_80573CA8, g_fSimulationTick, lbl_806E2130--);
    ReplayManager::Instance()->GrabSnapshot();
}

EventDispatcher::~EventDispatcher()
{
    BasicSlotPool<DLListEntry<EventCallback> >* pool = &callbacks.m_Allocator;
    pool->FreeBlocks();
}

void EventDispatcherBase::Dispatch(bool flag)
{
    state.fields.dispatching = 1;
    int count;
    do
    {
        count = state.fields.callbackCount;
        state.fields.callbackCount = 0;
        while (count != 0 && !state.fields.stopDispatch)
        {
            (*callbacks.Begin())(true);
            callbacks.DeleteEntry(nlDLRingRemoveStart(&callbacks.m_Head));
            count--;
        }
    } while (!flag && state.fields.callbackCount != 0);

    while (count != 0)
    {
        callbacks.DeleteEntry(nlDLRingRemoveStart(&callbacks.m_Head));
        state.fields.callbackCount--;
    }

    state.fields.dispatching = 0;
    state.fields.stopDispatch = 0;
}

void EventDispatcherBase::Clear()
{
    if (!state.fields.dispatching)
    {
        nlDLListIterator<EventCallback> iterator = callbacks.Begin();
        while (iterator.hasNext())
        {
            (*iterator)(false);
            iterator.next();
        }

        callbacks.Clear();
        state.fields.callbackCount = 0;
    }
}

void EventDispatcherBase::Add(const EventCallback& callback)
{
    callbacks.AddEnd(callback);
    state.fields.callbackCount++;
}

template struct UnidentifiedStaticStorage<UnidentifiedStaticTag>;
