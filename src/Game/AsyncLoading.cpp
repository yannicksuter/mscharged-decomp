#include "Game/AsyncLoading.h"
#include "Game/FE/feMusic.h"
#include "Game/Render/CrowdManager.h"
#include "Game/Render/Jumbotron.h"
#include "Game/Render/RLView.h"
#include "unclassified/tu_801B369C.h"

#include "Game/Audio/AudioBundleManager_802EDA7C.h"
#include "Game/Audio/AudioLoadMode_806E201C.h"
#include "Game/BaseGameSceneManager.h"
#include "Game/Task/BeginFrameTask.h"
#include "Game/Task/FrontEndTask.h"
#include "Game/Camera/CameraMan.h"
#include "Game/DB/StatsTracker.h"
#include "Game/Debug/FrameCounter.h"
#include "Game/Debug/TimeRegions.h"
#include "Game/Drawable/DrawableModel.h"
#include "Game/FE/feManager.h"
#include "Game/FE/feSceneManager.h"
#include "Game/Task/FixedUpdateTask.h"
#include "Game/Game.h"
#include "Game/NetworkSession.h"
#include "Game/NisPlayer.h"
#include "Game/PadActions.h"
#include "Game/Physics/PhysicsFakeBall.h"
#include "Game/Physics/PhysicsPatch.h"
#include "Game/Render/ElectricFence.h"
#include "Game/Render/FlareHandler.h"
#include "Game/ReplayChoreo.h"
#include "Game/ReplayManager.h"
#include "Game/ResourceInterface_802CC094.h"
#include "Game/SAnim.h"
#include "Game/SAnim/pnBlender.h"
#include "Game/SAnim/pnFeather.h"
#include "Game/SAnim/pnSAnimController.h"
#include "Game/SAnim/pnSingleAxisBlender.h"
#include "Game/SAnim/tu_8030E550.h"
#include "Game/Task/ParticleUpdateTask.h"
#include "Game/Transitions/ScreenTransitionManager.h"
#include "Game/TweakValue.h"
#include "Game/Task/TweakerTask.h"
#include "unclassified/tu_80332DC0.h"
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
#include "unclassified/tu_80332770.h"
#include "unclassified/tu_80336B2C.h"
#include "unclassified/tu_80338898.h"

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

class GameAudio_800EB6AC : public AudioLoadMode_806E201C
{
public:
    void Update(float deltaTime);
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

extern "C" int fn_8004F594(int category, const char* format, ...);
extern "C" void fn_801CC114();
extern "C" void* fn_802C082C(void*, int);
extern "C" void fn_801A95F0(void*, const char*, int);
extern "C" bool fn_80332770();
extern "C" u32 OSGetTick();
extern "C" void OSYieldThread();

extern "C" ResourceInterface_802CC094* fn_802CC094();
extern "C" bool fn_802BD63C();
extern "C" void fn_802BD718(
    const char* name, const char* units, float value);
extern "C" void fn_801B2770();
extern "C" void fn_8027ED18();
extern "C" void fn_8027E5D4();
extern "C" void fn_802B2E8C(UnidentifiedOwnerHandle* handle);
extern "C" void fn_800A6EDC(void*);
extern "C" void fn_800AA3E8(void*, int);
extern "C" void fn_801AF97C(void*);
extern "C" void fn_80013660(void*, int);
extern "C" void fn_801AB9D4(void*);
extern "C" void fn_801A01F8();
extern "C" void fn_801AAD0C(void*);
extern "C" void fn_80276F5C();
extern "C" void fn_80115FB4();
extern "C" void fn_801440BC();
extern "C" void fn_8013DB18();
extern "C" void fn_801B4238(void*);
extern "C" void fn_800741A4(void*);
extern "C" void fn_8013DDD4();
extern "C" void fn_802EC8A0(void*, bool, bool);
extern "C" void fn_802EC9D0(void*);
extern "C" void fn_800EBBD8(GameAudio_800EB6AC*);
extern "C" void fn_802EBBBC(void*);
extern "C" void fn_800ED8D8();
extern "C" void fn_801ACFC4();
extern "C" void fn_801A5328();
extern "C" void fn_80183E4C();
extern "C" void fn_802DB9C4(void*);
extern "C" bool fn_801C4D40();
extern "C" void fn_801C4CBC();
extern "C" void fn_802BDA28();
extern "C" void fn_802C0CCC();
extern "C" void fn_802C8180();
extern "C" void fn_802B1AE4();
extern "C" void fn_80370E20();
extern "C" void fn_802CC02C(ResourceInterface_802CC094*);
extern "C" void fn_802CC08C(ResourceInterface_802CC094*);
extern "C" void fn_80197120();
extern "C" void fn_80143FD4();
extern "C" void fn_80111658(bool);
extern "C" void fn_802B1B50();
extern "C" void fn_802B26D4();

void fn_80056EA8();
void DestroyCharacters();

extern FrameTimingStat* lbl_806E1698;
extern FrameTimingStat* lbl_806E169C;
extern FrameTimingStat* lbl_806E16A0;
extern BaseGameSceneManager* lbl_806E1838;
extern BaseGameSceneManager* lbl_806E1860;
extern void* g_pTeams[];
extern cBall* g_pBall;
extern u8 lbl_80574148[];
extern u8 lbl_806E16D4;
extern u8 lbl_8056CF08[];
extern u8 lbl_805721E8[];
extern UnidentifiedDeletable* lbl_806E2090;
extern SlotPool<cSAnimCallback> lbl_805840D8;
extern SlotPoolBase lbl_8057AB80;
extern bool g_e3_Build;
extern void* lbl_806E18C0;

namespace Detail
{
extern SlotPoolBase sTempStringAllocatorPool;
}

extern void* lbl_806E1E28;
extern void* lbl_806E1608;

bool g_VerboseAudio;
float g_fScriptBlockingWarningMS = 50.0f;
float g_fYieldScriptBlockingTimeMS = 45.0f;

static const char* lbl_806E103C;
static BaseSceneHandler* lbl_806E1040;
static u8 lbl_806E1044;
static u32 lbl_806E1048;
static float lbl_806E104C;
static UnidentifiedDeletable* lbl_806E1050;
static void* lbl_806E1054;
static int lbl_806E1058;
static float lbl_806E105C;
static void* lbl_806E1060;
static void* lbl_806E1064;
static bool lbl_806E1068;
static bool lbl_806E1069;
static bool lbl_806E106A;

static TweakValueBoolImpl_804F4538 lbl_8056E458(
    "g_VerboseAudio", "Audio", &g_VerboseAudio, true);
static TweakValueBoolImpl_804F4538 lbl_8056E478(
    "g_bDumpMemoryStatsOnLoad", "General/Memory",
    &g_e3_Build, true);
static TweakValueImpl_804F4DC8 lbl_8056E498(
    "g_fScriptBlockingWarningMS", "Loading",
    &g_fScriptBlockingWarningMS);
static TweakValueImpl_804F4DC8 lbl_8056E4B8(
    "g_fYieldScriptBlockingTimeMS", "Loading",
    &g_fYieldScriptBlockingTimeMS);
static AsyncLoadingManager lbl_8056E4D8;

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
        fn_8004F594(12, "Loaded in %f MS big bank FE_GEN\n", lbl_806E104C);
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
        fn_8011B430(this);
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

extern "C" AsyncLoadingManager* fn_80118A74()
{
    return &lbl_8056E4D8;
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
    AsyncLoadingManager* manager = &lbl_8056E4D8;
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
        fn_8004F594(12, "Processed %f, %s -> %s\n",
            nlGetTickerDifference(manager->mPreviousStageTick, ticker),
            lbl_806E103C, manager->mLoadingComment);
        lbl_806E103C = manager->mLoadingComment;
        manager->mPreviousStageTick = ticker;
    }

    if (nlGetTickerDifference(manager->mStageStartTick, nlGetTicker())
        > g_fScriptBlockingWarningMS)
    {
        fn_8004F594(12, "Script function %s blocked for more than %f MS\n",
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

    lbl_806E18C0 = 0;
    InitializeElectricFence(GetLayerView(eCLV_ElectricFence));
    BeginFrameTask::s_FramerateLocked = false;
    fn_801CC114();
    InitializeTimeRegions();
    fn_80137824(fn_80332770());

    manager->mLoadingState = 1;
    lbl_806E1040->SetVisible(false);
    lbl_806E10EC->mUnidentified2472 = true;

    float mem1Free = (float)StandardAllocator.TotalFreeMemory();
    float mem2Free = (float)VirtualAllocator.TotalFreeMemory();
    float totalMemFree = mem1Free + mem2Free;

    fn_8004F594(9,
        "MEM1 Free at end of InitializeGameState: %f bytes, or %f KB, or %f MB\n",
        mem1Free, mem1Free / 1024.0f, mem1Free / 1048576.0f);
    fn_8004F594(9,
        "MEM2 Free at end of InitializeGameState: %f bytes, or %f KB, or %f MB\n",
        mem2Free, mem2Free / 1024.0f, mem2Free / 1048576.0f);
    fn_8004F594(9,
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

    nlPrintf("RL memory free: %dK\n", fn_802CC094()->GetFreeMemory() >> 10);
    fn_802CC094();

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
    lbl_806E1860->PopEntireStack();
    if (lbl_806E1860 != 0)
    {
        delete lbl_806E1860;
        lbl_806E1860 = 0;
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

    fn_802B2E8C(&manager->mLoadingHandle);
    FEMusic::StopStream();
    BeginFrameTask::s_FramerateLocked = false;
    fn_80332EC8();
    fn_80337FF0(lbl_806E2164, 0);
    fn_80338900(lbl_806E2168, 0);
    lbl_806E20D8->BaseVirtual48(5);
    fn_803330AC()->Reset(0);
    lbl_806E2138->fn_8033288C();
    lbl_806E20D8->Initialize(false);

    fn_800A6EDC(g_pTeams[0]);
    fn_800A6EDC(g_pTeams[1]);
    DestroyPowerups();
    lbl_806E12C8->ResetEffects();
    DestroyCharacters();
    fn_800AA3E8(lbl_806E0C94->mUnidentified10DC, 1);
    fn_801AF97C(lbl_80574148);
    fn_80013660(g_pBall, 1);
    g_pBall = 0;
    FakeBallWorld::Destroy();
    cCameraManager::Shutdown();
    fn_801AB9D4(lbl_806E1608);
    fn_801A01F8();
    fn_801AAD0C(lbl_806E1608);
    ParticleUpdateTask::sInstance->Shutdown();
    fn_80276F5C();
    fn_80115FB4();
    GetFixedUpdateTask()->Reset();
    fn_801440BC();
    fn_8013DB18();
    FrontEnd::Destroy();
    Jumbotron::instance.Uninitialize();
    CrowdManager::instance.Uninitialize();
    ShutdownWarble(&gWarble);
    fn_801B4238(&lbl_806E16D4);

    if (nlSingleton<UnidentifiedManager_80188928>::s_pInstance != 0)
    {
        delete nlSingleton<UnidentifiedManager_80188928>::s_pInstance;
        nlSingleton<UnidentifiedManager_80188928>::s_pInstance = 0;
    }

    FreeElectricFence();
    DestroyGame();
    fn_800741A4(lbl_8056CF08);
    fn_8013DDD4();

    fn_802B467C(&Detail::sTempStringAllocatorPool);
    SlotPoolBase::BaseFreeBlocks(&Detail::sTempStringAllocatorPool, 0x40);

    fn_802EC8A0(lbl_806E201C, true, true);
    u32 startTick = OSGetTick();
    while (!lbl_806E201C->fn_802EBF78())
    {
        fn_802EC8A0(lbl_806E201C, true, true);
        static_cast<GameAudio_800EB6AC*>(lbl_806E201C)->Update(0.25f);
        nlServiceFileSystem();
        OSYieldThread();

        if (OSTicksToMilliseconds(OSGetTick() - startTick) > 400)
        {
            fn_802EC9D0(lbl_806E201C);
            nlBreak();
        }
    }

    fn_800EBBD8(static_cast<GameAudio_800EB6AC*>(lbl_806E201C));
    void* soundMap = lbl_806E201C->GetBundleManager()->GetSoundMap();
    if (soundMap != 0)
    {
        fn_802EBBBC(soundMap);
    }
    lbl_806E201C->Shutdown();

    fn_800ED8D8();
    fn_801ACFC4();
    fn_801A5328();
    fn_80183E4C();
    fn_802DB9C4(lbl_805721E8);
    CleanBoundingBoxCache();
    StatsTracker::Instance()->DestroyEventHandler();
    FEResourceManager::Instance()->UnloadPermanentResourceBundle();
    fn_801C4D40();
    FEResourceManager::Instance()->Cleanup();
    fn_801C4CBC();
    ScreenTransitionManager::Instance()->CancelAllTransitions();
    fn_802CC094()->ReleaseResource((unsigned long)manager->mUnidentified50);

    if (lbl_806E2090 != 0)
    {
        delete lbl_806E2090;
        lbl_806E2090 = 0;
    }

    nlFree(g_pLocalization->m_pFile);
    delete lbl_806E1050;
    fn_801379AC();

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
    fn_802C0CCC();
    fn_802C8180();
    fn_802B467C(&lbl_8057AB80);
    SlotPoolBase::BaseFreeBlocks(&lbl_8057AB80, 8);
    fn_802B1AE4();
    fn_802CC094()->ReleaseResource((unsigned long)manager->mUnidentified4C);
    fn_802CC02C(fn_802CC094());
    fn_802CC08C(0);
    fn_80197120();
    fn_80143FD4();
    fn_80111658(true);

    manager->mLoadingState = 0;
    fn_802B1B50();
    fn_802B26D4();
    FinishLoadingStep(manager);
}

class UnidentifiedMemoryStatSource
{
public:
    virtual ~UnidentifiedMemoryStatSource();
    virtual bool HasMemoryStats() = 0;
    virtual bool ReadMemoryStats(u32 size, void* output) = 0;
};

extern "C" void fn_8011B02C(AsyncLoadingManager* manager)
{
    if (g_e3_Build)
    {
        for (int i = 0; i < 4; ++i)
        {
            UnidentifiedMemoryStatSource* source
                = (UnidentifiedMemoryStatSource*)fn_802C082C(
                    lbl_806E1E28, i);
            if (source == 0)
            {
                return;
            }
            if (source->HasMemoryStats()
                && source->ReadMemoryStats(0x200, 0)
                && source->ReadMemoryStats(0x100, 0)
                && source->ReadMemoryStats(0x8000, 0))
            {
                SetE3DebugTime(5.0f);
            }
        }
    }

    lbl_806E1838->Push((SceneList)0x10, SCREEN_NOTHING, false);
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

extern "C" void fn_8011B430(AsyncLoadingManager*)
{
    if (GetConfigBool(Config::Global(), "TrophyTest", false))
    {
        fn_801A95F0(lbl_806E1608, "TrophyBanana", 0);
        fn_801A95F0(lbl_806E1608, "TrophyCrystalCup", 0);
        fn_801A95F0(lbl_806E1608, "TrophyFireCup", 0);
        fn_801A95F0(lbl_806E1608, "TrophyFlower", 0);
        fn_801A95F0(lbl_806E1608, "TrophyKonga", 0);
        fn_801A95F0(lbl_806E1608, "TrophyLava", 0);
        fn_801A95F0(lbl_806E1608, "TrophyMushroom", 0);
        fn_801A95F0(lbl_806E1608, "TrophyNextlevelCup", 0);
        fn_801A95F0(lbl_806E1608, "TrophyNintendo", 0);
        fn_801A95F0(lbl_806E1608, "TrophySand", 0);
        fn_801A95F0(lbl_806E1608, "TrophyStar", 0);
        fn_801A95F0(lbl_806E1608, "TrophyStrikerCup", 0);
        fn_801A95F0(lbl_806E1608, "TrophySunshine", 0);
    }
}

extern "C" void fn_8011B6E8(AsyncLoadingManager* manager)
{
    manager->mLoadingState = 2;
    manager->mLoadingComment = "InitializeStadiumViewer";
    FinishLoadingStep(manager);
}

extern "C" void* fn_8011B850()
{
    return lbl_806E1054;
}

extern "C" UnidentifiedOwnerHandle* fn_8011B858(
    UnidentifiedOwnerHandle* handle, int shouldDelete)
{
    if (handle != 0)
    {
        ReleaseUnidentifiedOwner(handle);
        if (shouldDelete > 0)
        {
            delete handle;
        }
    }
    return handle;
}

extern "C" void* fn_8011B8D8(UnidentifiedLoadingStateProvider* provider)
{
    return provider->mUnidentified18;
}
