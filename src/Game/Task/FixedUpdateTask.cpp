#include "Game/Task/FixedUpdateTask.h"
#include "Game/EventDispatcher.inl"

#include "Game/FE/feManager.h"

#include "Game/Physics/PhysicsCharacter.h"

#include "unclassified/tu_80284A58.h"

#include "Game/Render/PeachPhoto.h"

#include "Game/AI/AiUtil.h"
#include "Game/Ball.h"
#include "Game/CharacterTemplate.h"
#include "Game/DebugWriteCache.h"
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
#include "NL/platpad.h"
#include "NL/nlMain.h"
#include "NL/nlPrint.h"
#include "NL/nlString.h"
#include "types.h"
#include "Game/DetInput.h"
#include "Game/InputRouter.h"
#include "Game/NetworkInput.h"
#include "Game/NetworkSync.h"

#include <math.h>

extern u16 m_aJoystickRemap__14cCameraManager;
extern u16 lbl_806DF740;

float g_fFixedUpdateTick = 0.02f;
bool g_bRunSimAndRenderInLockStep;

static u16 sSimulationTimeType = 0xFFFF;
static u16 sTimeScaleType = 0xFFFF;

void fn_80111654()
{
}

void fn_80111658(bool)
{
}

void fn_8011165C()
{
}

void fn_80111660()
{
}

bool fn_80111664()
{
    return false;
}

static FixedUpdateTask fixedUpdateTask;
float g_fSimulationTick = g_fFixedUpdateTick;

FixedUpdateTask* GetFixedUpdateTask()
{
    return &fixedUpdateTask;
}

EventDispatcher* GetFixedUpdateEventDispatcher()
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
        g_pTeams[i]->fn_800A8DE8(&checksum);
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
    DebugWriteCache* cache = gNetworkSyncState->GetWriteCache();
    if (cache == 0)
    {
        return 0;
    }

    cache->BeginFrame(GetFrame());

    RunningChecksum checksum;
    cache->WriteFloat(&sSimulationTimeType, "simulationTime", &checksum,
        fixedUpdateTask.mSimulationTime);
    cache->WriteFloat(&sTimeScaleType, "timeScale", &checksum,
        fixedUpdateTask.mTargetTimeScale);

    int numGroups = g_pNetworkSessionBase->GetNumMachines();
    for (int groupIndex = 0; groupIndex < numGroups; groupIndex++)
    {
        NetworkPeer* group = g_pNetworkSessionBase->GetPeer((s8)groupIndex);
        int numControllers = group->mPlayerCount;
        for (int controllerIndex = 0; controllerIndex < numControllers; controllerIndex++)
        {
            DetInput* pad = (group->GetNetworkPeerChannel(controllerIndex))->GetNetworkPeerChannelInput();
            if (lbl_806DF740 == 0xFFFF)
            {
                lbl_806DF740 = cache->BeginType(sDetInputName);
                cache->AddField(17, gDebugFieldTypes[17].size, 0, sAnalogLeftXName);
                cache->AddField(17, gDebugFieldTypes[17].size, PAD_FIELD_OFFSET(pad, m_AnalogLeftY), sAnalogLeftYName);
                cache->AddField(17, gDebugFieldTypes[17].size, PAD_FIELD_OFFSET(pad, m_AnalogRightX), sAnalogRightXName);
                cache->AddField(17, gDebugFieldTypes[17].size, PAD_FIELD_OFFSET(pad, m_AnalogRightY), sAnalogRightYName);
                cache->AddField(0, gDebugFieldTypes[0].size, PAD_FIELD_OFFSET(pad, m_nConnected), sConnectedName);
                cache->AddField(1, gDebugFieldTypes[1].size, PAD_FIELD_OFFSET(pad, m_ButtonBitfield), sButtonBitfieldName);
                cache->AddField(0, gDebugFieldTypes[0].size, PAD_FIELD_OFFSET(pad, m_LeftTrigger), sLeftTriggerName);
                cache->AddField(0, gDebugFieldTypes[0].size, PAD_FIELD_OFFSET(pad, m_RightTrigger), sRightTriggerName);
                cache->AddField(22, gDebugFieldTypes[22].size, PAD_FIELD_OFFSET(pad, m_v3RevRemoteAccel), sRevRemoteAccelName);
                cache->AddField(22, gDebugFieldTypes[22].size, PAD_FIELD_OFFSET(pad, m_v3RevFreeStyleAccel), sRevFreeStyleAccelName);
                cache->AddField(0, gDebugFieldTypes[0].size, PAD_FIELD_OFFSET(pad, m_nRevDPDNumTargets), sRevDPDNumTargetsName);
                cache->AddField(21, gDebugFieldTypes[21].size, PAD_FIELD_OFFSET(pad, m_v2RevDPDCoord), sRevDPDCoordName);
                cache->AddField(15, gDebugFieldTypes[15].size, PAD_FIELD_OFFSET(pad, m_pPrevInput), sPrevInputName);
                cache->AddField(15, gDebugFieldTypes[15].size, PAD_FIELD_OFFSET(pad, m_pMyUser), sMyUserName);
                cache->AddField(19, gDebugFieldTypes[19].size, PAD_FIELD_OFFSET(pad, m_PolarAnalogLeft.a), sPolarAnalogLeftAName);
                cache->AddField(17, gDebugFieldTypes[17].size, PAD_FIELD_OFFSET(pad, m_PolarAnalogLeft.r), sPolarAnalogLeftRName);
                cache->AddField(19, gDebugFieldTypes[19].size, PAD_FIELD_OFFSET(pad, m_PolarAnalogLeft.a), sPolarAnalogRightAName);
                cache->AddField(17, gDebugFieldTypes[17].size, PAD_FIELD_OFFSET(pad, m_PolarAnalogLeft.r), sPolarAnalogRightRName);
                cache->AddArrayField(8, gDebugFieldTypes[8].size, 13, PAD_FIELD_OFFSET(pad, m_buttonStateTicks), sButtonStateTicksName);
                cache->AddField(19, gDebugFieldTypes[19].size, PAD_FIELD_OFFSET(pad, m_aRemapAngle), sRemapAngleName);
                cache->EndType();
            }

            DetInput* copy =
                (DetInput*)cache->WriteData(lbl_806DF740, pad, sizeof(DetInput));
            if (copy != 0)
            {
                copy->m_pPrevInput = 0;
                copy->m_pMyUser = (void*)pad->GetPadID();
                cache->ChecksumData(lbl_806DF740, copy, &checksum);
            }
        }
    }

    g_pGame->fn_8005B840(&checksum, cache);
    g_pBall->SyncLog(&checksum, cache);
    for (int i = 0; i < 2; i++)
    {
        g_pTeams[i]->fn_800A8900(&checksum, cache);
    }
    g_PhysicsWorld->SyncLog(&checksum, cache);

    u32 crc = ~checksum.m_nChecksum;
    char buffer[0x100];
    nlSNPrintf(buffer, sizeof(buffer),
        "------------------------ END Frame:%d CRC:%x -------------------------\n\n",
        GetFrame(), crc);
    cache->WriteText(buffer);
    return crc;
}

#undef PAD_FIELD_OFFSET

void FixedUpdateTask::OnSyncError()
{
    if (g_pNetworkSession->GetSessionMode())
    {
        NetworkStatsManager::Instance()->CalculateAndReportGameResult(2);
    }
    g_pNetworkSession->PopupNetworkError(1);
}

void FixedUpdateTask::OnInputQueueOverflow()
{
    NetworkStatsManager::Instance()->CalculateAndReportGameResult(4);
    g_pNetworkSession->PopupNetworkError(2);
}

u16 FixedUpdateTask::GetInputRemapAngle()
{
    return m_aJoystickRemap__14cCameraManager - 0x4000;
}

bool FixedUpdateTask::IsInPauseMenu()
{
    return FrontEnd::m_bInPauseMenuState;
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
    if (fn_80287AB0(GetPresentation()))
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
        && !g_pNetworkSession->GetPausedMachineMask())
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
            g_pPadManager->SetActivePadSet(1);
            simulationTick = g_fFixedUpdateTick;
            UpdatePlatPad(g_pPlatPadManager);
            g_pPadManager->Update(simulationTick);
            FlickDetection::Update();

            mAccumulatedDeltaT -= g_fFixedUpdateTick;
            if (g_bRunSimAndRenderInLockStep)
            {
                mAccumulatedDeltaT = 0.0f;
            }

            int updateCount = gInputManager->GetUpdateCount();
            gInputManager->CaptureInputs();

            bool updated = false;
            for (int i = 0; i < updateCount; ++i)
            {
                if (gInputManager->PrepareUpdate())
                {
                    CallFixedUpdateTasks();
                    updated = true;
                }
                if (fn_80287AB0(GetPresentation()))
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

            if (fn_80287AB0(GetPresentation()))
            {
                break;
            }
            if (g_bRunSimAndRenderInLockStep)
            {
                break;
            }
        }
    }

    g_pPadManager->SetActivePadSet(0);
    UpdatePlatPad(g_pPlatPadManager);
    g_pPadManager->Update(dt);
    FlickDetection::Update();
}

static void AIUpdateTask(float fDeltaT)
{
    g_pGame->PreUpdate(fDeltaT);
    g_pGame->fn_8005A8FC(fDeltaT);
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
    GetInputRouter();
    DispatchDetermDataEvents();

    AIUpdateTask(g_fSimulationTick);
    fn_80142A1C();
    gNPCManager->UpdateAINPCs(g_fSimulationTick);
    PrePhysicsAITask(g_fSimulationTick);
    PhysicsUpdate(g_PhysicsWorld, GetPhysicsUpdateTick());
    PostPhysicsAITask(g_fSimulationTick);

    if (NetMesh::s_bAnimatedNetMeshEnabled)
    {
        bool i = true;
        float goalieX = (float)fabs(g_pTeams[0]->GetGoalie()->mUnidentified024.m_v3Position.x);
        if (goalieX > cField::GetGoalLineX(1U))
        {
        }
        else
        {
            goalieX = (float)fabs(g_pTeams[1]->GetGoalie()->mUnidentified024.m_v3Position.x);
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
    UpdatePeachPhoto(&gPeachPhotoState, g_fSimulationTick, lbl_806E2130--);
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

