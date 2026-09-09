#include "Game/Sys/audio.h"
#include "Game/Render/StadiumLoading.h"
#include "Game/Font/fontmanager.h"
#include "Game/AsyncLoading.h"
#include "Game/DB/GameProgress.h"
#include "Game/GameInfo.h"
#include "Game/NetTournManager.h"
#include "Game/Render/NPCManager.h"
#include "Game/TrophyInfo.h"
#include "Game/GameTweaks.h"
#include "NL/nlFunctionMemory.h"
#include "Game/EventDataTypes.h"
#include "Game/Event.h"
#include "Game/Sys/debug.h"
#include "Game/FE/feMusic.h"
#include "Game/Render/CrowdManager.h"
#include "Game/Render/Jumbotron.h"
#include "Game/Render/RLView.h"
#include "Game/Task/DispatchEventsTask.h"
#include "Game/Task/SmokeTestUpdateTask.h"
#include "NL/globalpad.h"
#include "Game/Render/Warble.h"

#include "Game/Audio/AudioBundleManager.h"
#include "Game/Audio/AudioBankTable.h"
#include "Game/Audio/AudioSystem.h"
#include "Game/GameSceneManager.h"
#include "Game/Sys/movie.h"
#include "Game/Task/BeginFrameTask.h"
#include "Game/Task/FrontEndTask.h"
#include "Game/Camera/CameraMan.h"
#include "Game/DB/StatsTracker.h"
#include "Game/Debug/FrameCounter.h"
#include "Game/Debug/TimeRegions.h"
#include "Game/Drawable/DrawableModel.h"
#include "Game/FE/feManager.h"
#include "Game/FE/feSceneManager.h"
#include "Game/SH/SHPause.h"
#include "Game/Task/FixedUpdateTask.h"
#include "Game/Game.h"
#include "Game/NetworkSession.h"
#include "Game/NisPlayer.h"
#include "Game/Team.h"
#include "Game/PadActions.h"
#include "Game/Physics/PhysicsFakeBall.h"
#include "Game/Physics/PhysicsPatch.h"
#include "Game/Render/ElectricFence.h"
#include "Game/Render/FlareHandler.h"
#include "Game/ReplayChoreo.h"
#include "Game/ReplayManager.h"
#include "NL/gl/glMemory.h"
#include "Game/SAnim.h"
#include "Game/SAnim/pnBlender.h"
#include "Game/SAnim/pnFeather.h"
#include "Game/SAnim/pnSAnimController.h"
#include "Game/SAnim/pnSingleAxisBlender.h"
#include "Game/SAnim/tu_8030E550.h"
#include "Game/Task/ParticleUpdateTask.h"
#include "Game/Transitions/ScreenTransitionManager.h"
#include "Game/TweakValue.h"
#include "Game/TweakRegistry.h"
#include "Game/Task/TweakerTask.h"
#include "NL/gl/gl.h"
#include "NL/nlPrint.h"
#include "Game/InputRouter.h"
#include "NL/nlConfig.h"
#include "NL/nlDebug.h"
#include "NL/nlFile.h"
#include "NL/nlFileGC.h"
#include "NL/nlLocalization.h"
#include "NL/nlMemory.h"
#include "NL/nlSingleton.h"
#include "NL/nlSlotPool.h"
#include "NL/nlString.h"
#include "NL/nlTask.h"
#include "NL/nlTicker.h"
#include "NL/nlTime.h"
#include "types.h"
#include "unclassified/tu_80188884.h"
#include "Game/InputManager.h"
#include "Game/NetworkInput.h"
#include "Game/NetworkSync.h"
#include "NL/nlstring_tmpl.h"

#define OS_BUS_CLOCK_SPEED           (*(volatile u32*)0x800000F8)
#define OS_TIME_SPEED                (OS_BUS_CLOCK_SPEED / 4)
#define OSTicksToMilliseconds(ticks) ((ticks) / (OS_TIME_SPEED / 1000))

class FEResourceManager : public nlTask,
                          public nlSingleton<FEResourceManager>
{
public:
    void Run(float dt)
    {
        Update(dt);
    }

    virtual const char* GetName();
    void Cleanup();
    void UnloadPermanentResourceBundle();
    void Update(float dt);
};

class UnidentifiedDeletable
{
public:
    virtual ~UnidentifiedDeletable();
};

struct FrameTimingStat
{
    /* 0x00 */ u8 mUnidentified00[0xC];
    /* 0x0C */ float mSeconds;
    /* 0x10 */ int mCount;
};

extern "C" void fn_801CC114();
bool IsNetworkOrRecordedGame();
extern "C" u32 OSGetTick();
extern "C" void OSYieldThread();

extern "C" void fn_801B2770();
extern "C" void fn_8027ED18();
extern "C" void fn_8027E5D4();
extern "C" void fn_800AA3E8(void*, int);
extern "C" void fn_801AF97C(void*);
extern "C" void fn_80013660(void*, int);
extern "C" void fn_801A01F8();
extern "C" void fn_801440BC();
extern "C" void fn_8013DB18();
 void ShutdownWarbleRendering(void*);
extern "C" void fn_8013DDD4();
extern "C" void fn_802EC9D0(void*);

extern "C" void fn_801ACFC4();
 void FreeImpostorLighting();
extern "C" void fn_80183E4C();
extern "C" void fn_802DB9C4(void*);
extern "C" void fn_802BDA28();
extern "C" void fn_80143FD4();

void fn_80056EA8();
void DestroyCharacters();

extern FrameTimingStat* lbl_806E1698;
extern FrameTimingStat* lbl_806E169C;
extern FrameTimingStat* lbl_806E16A0;
extern cBall* g_pBall;
extern u8 lbl_80574148[];
extern u8 gCrowdModelCollection[];
extern SlotPool<cSAnimCallback> lbl_805840D8;
extern SlotPoolBase lbl_8057AB80;
extern bool g_e3_Build;

namespace Detail
{
extern SlotPoolBase sTempStringAllocatorPool;
}

bool g_VerboseAudio;
float g_fScriptBlockingWarningMS = 50.0f;
float g_fYieldScriptBlockingTimeMS = 45.0f;

static const char* lbl_806E103C;
static BaseSceneHandler* lbl_806E1040;
static u8 lbl_806E1044;
static u32 lbl_806E1048;
static float lbl_806E104C;
static UnidentifiedDeletable* lbl_806E1050;
static GLResourcePool* sPersistentResourcePool;
static int lbl_806E1058;
static float lbl_806E105C;
static void* lbl_806E1060;
static void* lbl_806E1064;
static bool lbl_806E1068;
static bool lbl_806E1069;
static bool lbl_806E106A;

static TweakBoolBinding lbl_8056E458(
    "g_VerboseAudio", "Audio", &g_VerboseAudio, true);
static TweakBoolBinding lbl_8056E478(
    "g_bDumpMemoryStatsOnLoad", "General/Memory",
    &g_e3_Build, true);
static TweakFloatBinding lbl_8056E498(
    "g_fScriptBlockingWarningMS", "Loading",
    &g_fScriptBlockingWarningMS);
static TweakFloatBinding lbl_8056E4B8(
    "g_fYieldScriptBlockingTimeMS", "Loading",
    &g_fYieldScriptBlockingTimeMS);
static AsyncLoadingManager sAsyncLoadingManager;

static inline void ReleaseUnidentifiedOwner(UnidentifiedOwnerHandle* handle)
{
    if (handle != 0 && handle->mOwner != 0
        && ((handle->mOwner->mFlags >> 30) & 1) != 0)
    {
        handle->mOwner->mTarget->Release(handle);
    }
}

static inline void FinishLoadingStep(AsyncLoadingManager* manager)
{
    if (nlGetTickerDifference(manager->mStageStartTick, nlGetTicker())
        > g_fYieldScriptBlockingTimeMS)
    {
        manager->StopWithoutUndo();
    }
}

extern "C" void fn_80116988(void*, const char* bankName)
{
    if (nlStrCmp<char>(bankName, "FE_GEN") == 0)
    {
        lbl_806E104C = nlGetTickerDifference(lbl_806E1048, nlGetTicker());
        tDebugPrintManager::Print(DC_LOADER, "Loaded in %f MS big bank FE_GEN\n", lbl_806E104C);
    }

    nlPrintf("Bank load complete \"%s\"\n", bankName);
    lbl_806E1058--;
    nlPrintf("Remaining loads %d\n", lbl_806E1058);
}

void AsyncLoadingManager::DoFunctionCall(unsigned int functionIndex)
{
    switch (functionIndex)
    {
    case 0:
        LoadTrophyTemplates();
        break;
    case 6:
        fn_8011B2E4(this);
        break;
    case 15:
        fn_8011B178(this);
        break;
    case 38:
        fn_8011A2E8(this);
        break;
    case 57:
        fn_8011926C(this);
        break;
    case 58:
        fn_80119B0C(this);
        break;
    case 59:
        fn_8011A9DC(this);
        break;
    case 60:
        fn_8011B02C(this);
        break;
    case 85:
        fn_80119454(this);
        break;
    case 86:
        fn_80119528(this);
        break;
    case 87:
        fn_80119EC0(this);
        break;
    case 88:
        fn_8011A0A8(this);
        break;
    case 90:
        fn_8011A800(this);
        break;
    case 92:
        fn_8011A570(this);
        break;
    case 98:
        fn_8011B6E8(this);
        break;
    default:
        if (functionIndex > 143)
        {
            nlBreak();
        }
        break;
    }
}

AsyncLoadingManager* AsyncLoadingManager::Instance()
{
    return &sAsyncLoadingManager;
}

AsyncLoadingManager::~AsyncLoadingManager()
{
    if (mByteCode != 0)
    {
        nlFree(mByteCode);
        mByteCode = 0;
    }
    ReleaseUnidentifiedOwner(&mLoadingHandle);
}

extern "C" void fn_80118B38(void* data, unsigned long, void*)
{
    AsyncLoadingManager* manager = &sAsyncLoadingManager;
    manager->mByteCode = data;
    manager->LoadByteCode(data);
}

extern "C" void fn_80118B50(AsyncLoadingManager*)
{
    nlLoadEntireFileAsync("art/Scripts/async_loading.byte_code", fn_80118B38,
        0, 0x20, AllocateStart, 0, 0, 0);
}

extern "C" u32 fn_80118B7C(AsyncLoadingManager* manager)
{
    if (manager->mByteCode == 0)
    {
        return ASYNC_LOADING_WAITING_FOR_BYTE_CODE;
    }

    u32 result = ASYNC_LOADING_NO_TRANSITION;
    bool completed = false;
    manager->mStageStartTick = nlGetTicker();
    manager->mLoadingComment = "No Loading Comment";

    switch (manager->mSequenceState)
    {
    case ASYNC_LOADING_BOOT_TO_FE_BEGIN:
        manager->CallFunction(nlStringHash("BootLoadingToFE"));
        manager->mSequenceState = ASYNC_LOADING_BOOT_TO_FE_RUN;
        result = ASYNC_LOADING_RUNNING;
        break;
    case ASYNC_LOADING_BOOT_TO_FE_RUN:
        manager->Run();
        if (manager->IsFinished())
        {
            manager->mSequenceState = ASYNC_LOADING_IDLE;
            result = ASYNC_LOADING_FE_READY;
            completed = true;
        }
        else
        {
            result = ASYNC_LOADING_RUNNING;
        }
        break;
    case ASYNC_LOADING_CLEAN_BOOT_BEGIN:
        manager->CallFunction(nlStringHash("CleanBootShutdown"));
        manager->mSequenceState = ASYNC_LOADING_CLEAN_BOOT_RUN;
        result = ASYNC_LOADING_RUNNING;
        break;
    case ASYNC_LOADING_CLEAN_BOOT_RUN:
        manager->Run();
        if (manager->IsFinished())
        {
            manager->mSequenceState = ASYNC_LOADING_IDLE;
            result = ASYNC_LOADING_CLEAN_BOOT_COMPLETE;
            completed = true;
        }
        else
        {
            result = ASYNC_LOADING_RUNNING;
        }
        break;
    case ASYNC_LOADING_FE_TO_GAME_BEGIN:
        manager->CallFunction(nlStringHash("FELoadingToGame"));
        manager->mSequenceState = ASYNC_LOADING_FE_TO_GAME_RUN;
        result = ASYNC_LOADING_RUNNING;
        break;
    case ASYNC_LOADING_FE_TO_GAME_RUN:
        manager->Run();
        if (manager->IsFinished())
        {
            manager->mSequenceState = ASYNC_LOADING_IDLE;
            result = ASYNC_LOADING_GAME_READY;
            completed = true;
        }
        else
        {
            result = ASYNC_LOADING_RUNNING;
        }
        break;
    case ASYNC_LOADING_GAME_TO_FE_BEGIN:
        manager->CallFunction(nlStringHash("GameLoadingToFE"));
        manager->mSequenceState = ASYNC_LOADING_GAME_TO_FE_RUN;
        result = ASYNC_LOADING_RUNNING;
        break;
    case ASYNC_LOADING_GAME_TO_FE_RUN:
        manager->Run();
        if (manager->IsFinished())
        {
            manager->mSequenceState = ASYNC_LOADING_IDLE;
            result = ASYNC_LOADING_RETURN_TO_FE;
            completed = true;
        }
        else
        {
            result = ASYNC_LOADING_RUNNING;
        }
        break;
    case ASYNC_LOADING_BOOT_TO_GAME_BEGIN:
        manager->CallFunction(nlStringHash("BootLoadingToGame"));
        manager->mSequenceState = ASYNC_LOADING_BOOT_TO_GAME_RUN;
        result = ASYNC_LOADING_RUNNING;
        break;
    case ASYNC_LOADING_BOOT_TO_GAME_RUN:
        manager->Run();
        if (manager->IsFinished())
        {
            manager->mSequenceState = ASYNC_LOADING_IDLE;
            result = ASYNC_LOADING_STADIUM_OR_GAME_READY;
            completed = true;
        }
        else
        {
            result = ASYNC_LOADING_RUNNING;
        }
        break;
    case ASYNC_LOADING_STADIUM_VIEWER_BEGIN:
        manager->CallFunction(nlStringHash("BootLoadingToStadiumViewer"));
        manager->mSequenceState = ASYNC_LOADING_STADIUM_VIEWER_RUN;
        result = ASYNC_LOADING_RUNNING;
        break;
    case ASYNC_LOADING_STADIUM_VIEWER_RUN:
        manager->Run();
        if (manager->IsFinished())
        {
            manager->mSequenceState = ASYNC_LOADING_IDLE;
            result = ASYNC_LOADING_STADIUM_OR_GAME_READY;
            completed = true;
        }
        else
        {
            result = ASYNC_LOADING_RUNNING;
        }
        break;
    }

    if (result == ASYNC_LOADING_NO_TRANSITION)
    {
        return result;
    }

    if (manager->mLoadingComment != lbl_806E103C)
    {
        u32 ticker = nlGetTicker();
        tDebugPrintManager::Print(DC_LOADER, "Processed %f, %s -> %s\n",
            nlGetTickerDifference(manager->mPreviousStageTick, ticker),
            lbl_806E103C, manager->mLoadingComment);
        lbl_806E103C = manager->mLoadingComment;
        manager->mPreviousStageTick = ticker;
    }

    if (nlGetTickerDifference(manager->mStageStartTick, nlGetTicker())
        > g_fScriptBlockingWarningMS)
    {
        tDebugPrintManager::Print(DC_LOADER, "Script function %s blocked for more than %f MS\n",
            manager->mLoadingComment, g_fScriptBlockingWarningMS);
    }

    if (completed)
    {
        lbl_806E105C = 0.0f;
    }
    return result;
}

static inline void BeginLoadingSequence(AsyncLoadingManager* manager, u32 state)
{
    manager->mSequenceState = state;
    lbl_806E103C = 0;
    manager->mPreviousStageTick = nlGetTicker();
    manager->mSequenceStartTime = nlGetTime();
}

extern "C" void fn_80119054(AsyncLoadingManager* manager)
{
    BeginLoadingSequence(manager, ASYNC_LOADING_BOOT_TO_FE_BEGIN);
}

extern "C" void fn_801190A0(AsyncLoadingManager* manager)
{
    BeginLoadingSequence(manager, ASYNC_LOADING_CLEAN_BOOT_BEGIN);
}

extern "C" void fn_801190EC(AsyncLoadingManager* manager)
{
    BeginLoadingSequence(manager, ASYNC_LOADING_FE_TO_GAME_BEGIN);
}

extern "C" void fn_80119138(AsyncLoadingManager* manager)
{
    BeginLoadingSequence(manager, ASYNC_LOADING_GAME_TO_FE_BEGIN);
}

extern "C" void fn_80119184(AsyncLoadingManager* manager)
{
    lbl_806E1044 = false;
    BeginLoadingSequence(manager, ASYNC_LOADING_GAME_TO_FE_BEGIN);
}

extern "C" void fn_801191D4(AsyncLoadingManager* manager)
{
    BeginLoadingSequence(manager, ASYNC_LOADING_BOOT_TO_GAME_BEGIN);
}

extern "C" void fn_80119220(AsyncLoadingManager* manager)
{
    BeginLoadingSequence(manager, ASYNC_LOADING_STADIUM_VIEWER_BEGIN);
}

extern "C" void fn_8011926C(AsyncLoadingManager* manager)
{
    manager->mLoadingState = 3;
    manager->mLoadingComment = "DestroyFEFast";
    manager->mLoadingState = 0;
    FinishLoadingStep(manager);
}

extern "C" void fn_80119454(AsyncLoadingManager* manager)
{
    manager->mLoadingState = 2;
    manager->mLoadingComment = "InitializeFEState1";
    FinishLoadingStep(manager);
}

extern "C" void fn_80119528(AsyncLoadingManager* manager)
{
    manager->mLoadingComment = "InitializeFEState2";
    manager->mLoadingState = 0;
    FinishLoadingStep(manager);
}

extern "C" void fn_80119B0C(AsyncLoadingManager* manager)
{
    manager->mLoadingState = 3;
    manager->mLoadingComment = "DestroyFEState";
    manager->mLoadingState = 0;
    FinishLoadingStep(manager);
}

extern "C" void fn_80119EC0(AsyncLoadingManager* manager)
{
    manager->mLoadingState = 2;
    manager->mLoadingComment = "InitializeGameState1";
    FinishLoadingStep(manager);
}

extern "C" void fn_8011A0A8(AsyncLoadingManager* manager)
{
    manager->mLoadingComment = "InitializeGameState2";
    FinishLoadingStep(manager);
}

extern "C" void fn_8011A2DC(void* value0, void* value1)
{
    lbl_806E1060 = value0;
    lbl_806E1064 = value1;
}

extern "C" void fn_8011A2E8(AsyncLoadingManager* manager)
{
    manager->mLoadingComment = "AsyncStartGameWorldLoading";
    FinishLoadingStep(manager);
}

extern "C" void fn_8011A570(AsyncLoadingManager* manager)
{
    manager->mLoadingComment = "InitializeGameStateInGameFE1";
    FinishLoadingStep(manager);
}

extern "C" void fn_8011A800(AsyncLoadingManager* manager)
{
    manager->mLoadingComment = "GameStateFinalize";

    PauseMenuScene::mLastSelectedIndex = 0;
    InitializeElectricFence(GetLayerView(eCLV_ElectricFence));
    BeginFrameTask::s_FramerateLocked = false;
    fn_801CC114();
    InitializeTimeRegions();
    UseDefaultFreestyleButtonRemap(IsNetworkOrRecordedGame());

    manager->mLoadingState = 1;
    lbl_806E1040->SetVisible(false);
    g_pNetworkSession->mUnidentified2472 = true;

    float mem1Free = (float)StandardAllocator.TotalFreeMemory();
    float mem2Free = (float)VirtualAllocator.TotalFreeMemory();
    float totalMemFree = mem1Free + mem2Free;

    tDebugPrintManager::Print(DC_MEMORY,
        "MEM1 Free at end of InitializeGameState: %f bytes, or %f KB, or %f MB\n",
        mem1Free, mem1Free / 1024.0f, mem1Free / 1048576.0f);
    tDebugPrintManager::Print(DC_MEMORY,
        "MEM2 Free at end of InitializeGameState: %f bytes, or %f KB, or %f MB\n",
        mem2Free, mem2Free / 1024.0f, mem2Free / 1048576.0f);
    tDebugPrintManager::Print(DC_MEMORY,
        "Total Mem Free at end of InitializeGameState: %f bytes, or %f KB, or %f MB\n",
        totalMemFree, totalMemFree / 1024.0f,
        totalMemFree / 1048576.0f);

    fn_802BD718(
        "MEM1 Free at end of InitializeGameState", "bytes", mem1Free);
    fn_802BD718(
        "MEM2 Free at end of InitializeGameState", "bytes", mem2Free);
    fn_802BD718(
        "Total Free Memory at end of InitializeGameState", "bytes",
        totalMemFree);

    FinishLoadingStep(manager);
}

extern "C" void fn_8011A9DC(AsyncLoadingManager* manager)
{
    manager->mLoadingComment = "DestroyGameState";

    nlPrintf("RL memory free: %dK\n", glGetCurrentResourcePool()->GetFreeMemory() >> 10);
    glGetCurrentResourcePool();

    manager->mLoadingState = 3;

    fn_80056EA8();

    if (fn_802BD63C())
    {
        fn_802BD718("FrameTime_GamePlay", "seconds",
            lbl_806E1698->mSeconds / (float)lbl_806E1698->mCount);
        fn_802BD718("FrameTime_NIS", "seconds",
            lbl_806E169C->mSeconds / (float)lbl_806E169C->mCount);
        fn_802BD718("FrameTime_AutoReplay", "seconds",
            lbl_806E16A0->mSeconds / (float)lbl_806E16A0->mCount);
        g_FrameCounter.fn_802B80C4();
    }

    fn_801B2770();
    FESceneManager::Instance()->ClearTopMostScene();
    g_pOverlayManager->PopEntireStack();
    if (g_pOverlayManager != 0)
    {
        delete g_pOverlayManager;
        g_pOverlayManager = 0;
    }
    FESceneManager::Instance()->ForceImmediateStackProcessing();

    FlareHandler::instance.Cleanup();
    NisPlayer::Instance();
    fn_8027ED18();
    NisPlayer::Instance()->Reset();
    NisPlayer::Instance();
    fn_8027E5D4();
    ReplayChoreo::Instance().Reset();
    ReplayManager::Instance()->Uninitialize();

    while (!FESceneManager::Instance()->AreAllScenesValid())
    {
        nlServiceFileSystem();
        FESceneManager::Instance()->Update(0.0f);
        FEResourceManager::Instance()->Run(0.0f);
    }

    DisconnectEventOwner(&manager->mLoadingHandle);
    FEMusic::StopStream();
    BeginFrameTask::s_FramerateLocked = false;
    OnInputSessionReset();
    gNetworkInputRecording->Reset(0);
    gNetworkSyncState->Reset(0);
    g_pNetworkSessionBase->BaseVirtual48(5);
    GetInputRouter()->Reset(0);
    gInputManager->Reset();
    g_pNetworkSessionBase->Initialize(false);

    g_pTeams[0]->StopGameplayEffectsAndSounds();
    g_pTeams[1]->StopGameplayEffectsAndSounds();
    DestroyPowerups();
    lbl_806E12C8->ResetEffects();
    DestroyCharacters();
    fn_800AA3E8(g_pGame->mUnidentified10DC, 1);
    fn_801AF97C(lbl_80574148);
    fn_80013660(g_pBall, 1);
    g_pBall = 0;
    FakeBallWorld::Destroy();
    cCameraManager::Shutdown();
    gNPCManager->DestroyNPCs();
    fn_801A01F8();
    gNPCManager->UnloadTransientNPCTemplates();
    ParticleUpdateTask::sInstance->Shutdown();
    DestroyStadium();
    fn_80115FB4();
    GetFixedUpdateTask()->Reset();
    fn_801440BC();
    fn_8013DB18();
    FrontEnd::Destroy();
    Jumbotron::instance.Uninitialize();
    CrowdManager::instance.Uninitialize();
    ShutdownWarble(&gWarble);
    ShutdownWarbleRendering(&gWarbleEnabled);

    if (nlSingleton<UnidentifiedManager_80188928>::s_pInstance != 0)
    {
        delete nlSingleton<UnidentifiedManager_80188928>::s_pInstance;
        nlSingleton<UnidentifiedManager_80188928>::s_pInstance = 0;
    }

    FreeElectricFence();
    DestroyGame();
    DestroyGameTweaks(&gGameTweaks);
    fn_8013DDD4();

    fn_802B467C(&Detail::sTempStringAllocatorPool);
    SlotPoolBase::BaseFreeBlocks(&Detail::sTempStringAllocatorPool, 0x40);

    FlushAudio(g_pAudioSystem, true, true);
    u32 startTick = OSGetTick();
    while (!g_pAudioSystem->IsIdle())
    {
        FlushAudio(g_pAudioSystem, true, true);
        static_cast<GameAudio*>(g_pAudioSystem)->Update(0.25f);
        nlServiceFileSystem();
        OSYieldThread();

        if (OSTicksToMilliseconds(OSGetTick() - startTick) > 400)
        {
            fn_802EC9D0(g_pAudioSystem);
            nlBreak();
        }
    }

    UnloadSoundBanks(static_cast<GameAudio*>(g_pAudioSystem));
    AudioBankTable* soundMap = g_pAudioSystem->GetBundleManager()->GetSoundMap();
    if (soundMap != 0)
    {
        soundMap->ClearSelectedGroups();
    }
    g_pAudioSystem->Shutdown();

    StopCrowdReactions();
    fn_801ACFC4();
    FreeImpostorLighting();
    fn_80183E4C();
    fn_802DB9C4(gCrowdModelCollection);
    CleanBoundingBoxCache();
    StatsTracker::Instance()->DestroyEventHandler();
    FEResourceManager::Instance()->UnloadPermanentResourceBundle();
    UnloadFEMiniBundle();
    FEResourceManager::Instance()->Cleanup();
    DestroyFEResourcePool();
    ScreenTransitionManager::Instance()->CancelAllTransitions();
    glGetCurrentResourcePool()->ReleaseResource((unsigned long)manager->mUnidentified50);

    if (FontManager::s_pInstance != 0)
    {
        delete FontManager::s_pInstance;
        FontManager::s_pInstance = 0;
    }

    nlFree(g_pLocalization->m_pFile);
    delete lbl_806E1050;
    DestroyPadBackends();

    cPN_SAnimController::m_SAnimControllerSlotPool.FreeBlocks();
    cPN_Blender::m_BlenderSlotPool.FreeBlocks();
    cPN_SingleAxisBlender::m_SingleAxisBlenderSlotPool.FreeBlocks();
    cPN_Feather::m_FeatherSlotPool.FreeBlocks();
    cPN_8030E550::mSlotPool.FreeBlocks();
    lbl_805840D8.FreeBlocks();

    if (g_bTweaking)
    {
        fn_802BDA28();
    }
    ResetDynamicTweaks();
    glCompact();
    fn_802B467C(&lbl_8057AB80);
    SlotPoolBase::BaseFreeBlocks(&lbl_8057AB80, 8);
    FreeFunctionMemoryPools();
    glGetCurrentResourcePool()->ReleaseResource((unsigned long)manager->mUnidentified4C);
    glDestroyResourcePool(glGetCurrentResourcePool());
    glSetCurrentResourcePool(0);
    FreeEventDataPools();
    fn_80143FD4();
    fn_80111658(true);

    manager->mLoadingState = 0;
    PopFunctionMemoryState();
    PopEventConnectionState();
    FinishLoadingStep(manager);
}

extern "C" void fn_8011B02C(AsyncLoadingManager* manager)
{
    if (g_e3_Build)
    {
        for (int i = 0; i < 4; ++i)
        {
            cGlobalPad* source = g_pPadManager->GetPad(i);
            if (source == 0)
            {
                return;
            }
            if (source->IsConnected()
                && source->IsPressed(0x200, false)
                && source->IsPressed(0x100, false)
                && source->IsPressed(0x8000, false))
            {
                SetE3DebugTime(5.0f);
            }
        }
    }

    GameSceneManager::Instance()->Push((SceneList)0x10, SCREEN_NOTHING, false);
    fn_80370E20();
    FinishLoadingStep(manager);
}

extern "C" void fn_8011B178(AsyncLoadingManager* manager)
{
    manager->mLoadingComment = "AsyncFEResourceLoadBeing";
    lbl_806E1068 = false;
    lbl_806E1069 = false;
    FinishLoadingStep(manager);
}

extern "C" void fn_8011B2E4(AsyncLoadingManager* manager)
{
    manager->mLoadingComment = "AsyncFEGameResourceLoadBegin";
    lbl_806E1068 = false;
    lbl_806E1069 = false;
    FinishLoadingStep(manager);
}

extern "C" void fn_8011B40C(void*, unsigned long, unsigned long)
{
    lbl_806E106A = true;
}

extern "C" void fn_8011B418(void*, unsigned long, unsigned long)
{
    lbl_806E1068 = true;
}

extern "C" void fn_8011B424(void*, unsigned long, unsigned long)
{
    lbl_806E1069 = true;
}

void AsyncLoadingManager::LoadTrophyTemplates()
{
    char trophyName[64];
    if (GetConfigBool(Config::Global(), "TrophyTest", false))
    {
        gNPCManager->CreateNPCTemplate("TrophyBanana", false);
        gNPCManager->CreateNPCTemplate("TrophyCrystalCup", false);
        gNPCManager->CreateNPCTemplate("TrophyFireCup", false);
        gNPCManager->CreateNPCTemplate("TrophyFlower", false);
        gNPCManager->CreateNPCTemplate("TrophyKonga", false);
        gNPCManager->CreateNPCTemplate("TrophyLava", false);
        gNPCManager->CreateNPCTemplate("TrophyMushroom", false);
        gNPCManager->CreateNPCTemplate("TrophyNextlevelCup", false);
        gNPCManager->CreateNPCTemplate("TrophyNintendo", false);
        gNPCManager->CreateNPCTemplate("TrophySand", false);
        gNPCManager->CreateNPCTemplate("TrophyStar", false);
        gNPCManager->CreateNPCTemplate("TrophyStrikerCup", false);
        gNPCManager->CreateNPCTemplate("TrophySunshine", false);
    }
    else if (GameInfoManager::Instance()->IsOnline()
        && GameInfoManager::Instance()->IsInMode1())
    {
        int cupPersona = NetTournManager::Instance()->GetCupPersona();
        const char** names = GetCupPersonaTrophyNames();
        nlSNPrintf(trophyName, sizeof(trophyName), "Trophy%s", names[cupPersona]);
        gNPCManager->CreateNPCTemplate(trophyName, false);
    }
    else if (GameInfoManager::Instance()->IsInMode3())
    {
        int cup = CupManager::Instance()->GetCurrentMode();
        const char** names = GetCupTrophyNames();
        nlSNPrintf(trophyName, sizeof(trophyName), "Trophy%s", names[cup]);
        gNPCManager->CreateNPCTemplate(trophyName, false);
    }
}

extern "C" void fn_8011B6E8(AsyncLoadingManager* manager)
{
    manager->mLoadingState = 2;
    manager->mLoadingComment = "InitializeStadiumViewer";
    FinishLoadingStep(manager);
}

GLResourcePool* AsyncLoadingManager::GetPersistentResourcePool()
{
    return sPersistentResourcePool;
}

extern "C" UnidentifiedOwnerHandle* fn_8011B858(
    UnidentifiedOwnerHandle* handle, int shouldDelete)
{
    if (handle != 0)
    {
        if (handle->mOwner != 0
            && ((handle->mOwner->mFlags >> 30) & 1) != 0)
        {
            handle->mOwner->mTarget->Release(handle);
        }
        if (shouldDelete > 0)
        {
            delete handle;
        }
    }
    return handle;
}
