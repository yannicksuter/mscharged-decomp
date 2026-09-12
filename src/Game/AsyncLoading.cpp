#include "Game/MiiManager.h"
#include "Game/HBMManager.h"

#include "Game/Sys/audio.h"
#include "Game/Render/StadiumLoading.h"
#include "Game/Font/fontmanager.h"
#include "Game/AsyncLoading.h"
#include "Game/Ball.h"
#include "Game/CharacterLoader.h"
#include "Game/DB/StadiumInfo.h"
#include "Game/GameObjectLighting.h"
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
#include "Game/FE/FEAudio.h"
#include "Game/FE/feResourceManager.h"
#include "Game/Render/CrowdManager.h"
#include "Game/Render/CrowdImpostors.h"
#include "Game/Render/CrowdModelCollection.h"
#include "Game/Render/Jumbotron.h"
#include "Game/Render/RLView.h"
#include "Game/Task/DispatchEventsTask.h"
#include "Game/Task/SmokeTestUpdateTask.h"
#include "NL/globalpad.h"
#include "Game/Render/Warble.h"
#include "Game/Render/WorldNPC.h"
#include "Game/SH/SHBootLoading.h"

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
#include "Game/FE/feHelpFuncs.h"
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
#include "NL/gl/glTexture.h"
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
#include "NL/plat/tu_80372B4C.h"
#include "types.h"
#include "unclassified/tu_80188884.h"
#include "Game/InputManager.h"
#include "Game/NetworkInput.h"
#include "Game/NetworkSync.h"
#include "NL/nlstring_tmpl.h"

#define OS_BUS_CLOCK_SPEED           (*(volatile u32*)0x800000F8)
#define OS_TIME_SPEED                (OS_BUS_CLOCK_SPEED / 4)
#define OSTicksToMilliseconds(ticks) ((ticks) / (OS_TIME_SPEED / 1000))

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
extern "C" bool fn_80200380(bool* inGame);
extern "C" bool fn_8020078C(bool* inGame);
extern "C" bool fn_802773B8(bool stadiumViewer);
extern "C" const char* fn_800E3198();
extern "C" void fn_80311AFC(const char* filename, bool async);
extern "C" bool fn_80311C5C();
extern "C" bool fn_800F08A4();
extern "C" void fn_800F06D4();
extern "C" void fn_800F030C(bool stadiumViewer);
extern "C" void fn_801FE81C();
extern "C" bool fn_801FE8EC();
extern "C" void fn_8013D8DC();
extern "C" void fn_80144070();
extern "C" void fn_8013D85C();
void fn_80056CF4(void*, int, bool);
extern "C" void fn_8030753C(FontManager*, GLResourcePool*);

extern "C" void fn_801ACFC4();
 void FreeImpostorLighting();
extern "C" void fn_802DB9C4(void*);
extern "C" void fn_80143FD4();

void fn_80056EA8();
void DestroyCharacters();

extern FrameTimingStat* lbl_806E1698;
extern FrameTimingStat* lbl_806E169C;
extern FrameTimingStat* lbl_806E16A0;
extern cBall* g_pBall;
extern u8 lbl_80574148[];
extern bool gAudioEnabled;
extern bool lbl_806E18A0;
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
static WorldNPCManager* lbl_806E1050;
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

static inline void FinishLoadingStepOrUndo(
    AsyncLoadingManager* manager, bool finished)
{
    if (finished)
    {
        FinishLoadingStep(manager);
    }
    else
    {
        manager->StopWithUndo();
    }
}

extern "C" void fn_80116988(AudioResourceLoadOwner*, void* context)
{
    const char* bankName = (const char*)context;
    if (nlStrCmp<char>(bankName, "FE_GEN") == 0)
    {
        lbl_806E104C = nlGetTickerDifference(lbl_806E1048, nlGetTicker());
        tDebugPrintManager::Print(DC_LOADER, "Loaded in %f MS big bank FE_GEN\n", lbl_806E104C);
    }

    nlPrintf("Bank load complete \"%s\"\n", bankName);
    lbl_806E1058--;
    nlPrintf("Remaining loads %d\n", lbl_806E1058);
}

template <typename T>
static inline void CreateInstance(T*& instance)
{
    if (instance == 0)
    {
        instance = new (8, false) T;
    }
}

void AsyncLoadingManager::DoFunctionCall(unsigned int functionIndex)
{
    switch (functionIndex)
    {
    case 0:
        LoadTrophyTemplates();
        break;
    case 1:
    {
        bool persistent = m_SP[-1] != 0;
        const char* name = (const char*)m_SP[-2];
        m_SP -= 2;
        if (gNPCManager == 0)
        {
            CreateInstance(gNPCManager);
        }
        gNPCManager->CreateNPCTemplate(name, persistent);
        break;
    }
    case 2:
        CreateLargeFEResourcePool();
        break;
    case 3:
        CreateFEResourcePool();
        break;
    case 4:
    {
        fn_80111654(4);
        GLMemoryRequirement requirements[] = {
            { GLM_Header, 0x3C00 },
            { GLM_TextureData, 0x390800 },
        };
        sPersistentResourcePool = glCreateResourcePool(
            requirements, 2, "PersistentResourcePool");
        break;
    }
    case 5:
        m_SP++;
        m_SP[-1] = CharacterLoader_8056B290::sUnidentifiedInstance.fn_8000AE04();
        break;
    case 6:
        fn_8011B2E4(this);
        break;
    case 7:
        mLoadingComment = "AsyncFEGameResourceLoadFinalize";
        if (lbl_806E1068 && lbl_806E1069)
        {
            FEResourceManager::Instance()->m_bPermanentBundleLoadInProgress = false;
            FinishLoadingStep(this);
        }
        else
        {
            StopWithUndo();
        }
        break;
    case 8:
        if (gAudioEnabled)
        {
            lbl_806E106A = false;
            LoadSoundBank((GameAudio*)g_pAudioSystem, 0x32, 0x14,
                fn_8011B40C, (void*)"FE2GameLoop");
        }
        else
        {
            lbl_806E106A = true;
        }
        break;
    case 9:
        if (lbl_806E106A)
        {
            if (FEMusic::IsEnabled())
            {
                PlayTrackedSound(0x14, 0x7FC13AA3,
                    "FE2GameLoadStream", this, true);
            }
            FinishLoadingStep(this);
        }
        else
        {
            StopWithUndo();
        }
        break;
    case 10:
        g_pAudioSystem->GetBundleManager()->GetSoundMap()->SelectGroup(1);
        lbl_806E1058 += gAudioEnabled ? 2 : 0;
        lbl_806E1048 = nlGetTicker();
        lbl_806E104C = 0.0f;
        LoadSoundBank((GameAudio*)g_pAudioSystem, 0x17, 0x15,
            fn_80116988, (void*)"FE_GEN");
        LoadSoundBank((GameAudio*)g_pAudioSystem, 0x1A, 0x16,
            fn_80116988, (void*)"FE_GEN_MUSIC");
        FEAudio::SetSoundCategory(0x15);
        FEMusic::SetInGame(false);
        break;
    case 11:
        mLoadingComment = "AsyncFELocalizationBegin";
        if (FontManager::Instance() == 0)
        {
            nlSingleton<FontManager>::s_pInstance = new (8, false) FontManager;
        }
        fn_8030753C(FontManager::Instance(), GetFEResourcePool());
        lbl_806E18A0 = false;
        fn_80200380(&lbl_806E18A0);
        FinishLoadingStep(this);
        break;
    case 12:
        if (!lbl_806E1044)
        {
            if (gAudioEnabled)
            {
                lbl_806E106A = false;
                LoadSoundBank((GameAudio*)g_pAudioSystem, 0x19, 0x17,
                    fn_8011B40C, (void*)"FE_GEN_SPLASH");
            }
        }
        else if (gAudioEnabled)
        {
            lbl_806E106A = false;
            g_pAudioSystem->GetBundleManager()->GetSoundMap()->SelectGroup(0);
            LoadSoundBank((GameAudio*)g_pAudioSystem, 0x18, 0x17,
                fn_8011B40C, (void*)"FESoundLoop");
        }
        else
        {
            lbl_806E106A = true;
        }
        break;
    case 13:
        if (!lbl_806E1044)
        {
            FinishLoadingStepOrUndo(this, lbl_806E106A);
        }
        else if (lbl_806E106A)
        {
            fn_80370E20();
            if (FEMusic::IsEnabled())
            {
                PlayTrackedSound(0x17, 0x6AF33AC2,
                    "FEGenLoadScreen", this, true);
            }
            FinishLoadingStep(this);
        }
        else
        {
            StopWithUndo();
        }
        break;
    case 14:
        StopSound(0x6AF33AC2, this);
        g_pAudioSystem->GetBundleManager()->GetSoundMap()->UnloadBank(0x12);
        break;
    case 15:
        fn_8011B178(this);
        break;
    case 16:
    {
        mLoadingComment = "AsyncFEResourceLoadFinalize";
        BootLoadingScene* loadingScene;
        if (g_pLocalization->m_CurrentLanguage == nlLocalization::LangJapanese)
        {
            loadingScene = static_cast<BootLoadingScene*>(
                GameSceneManager::Instance()->GetScene(SCENE_BOOT_LOADING_JPN));
        }
        else
        {
            loadingScene = static_cast<BootLoadingScene*>(
                GameSceneManager::Instance()->GetScene(SCENE_BOOT_LOADING));
        }
        if (lbl_806E1068 && lbl_806E1069
            && (loadingScene == 0 || !loadingScene->IsBootScreenPending()))
        {
            FinishLoadingStep(this);
        }
        else
        {
            StopWithUndo();
        }
        break;
    }
    case 17:
        lbl_806E1058 += gAudioEnabled ? 1 : 0;
        lbl_806E1048 = nlGetTicker();
        lbl_806E104C = 0.0f;
        LoadSoundBank((GameAudio*)g_pAudioSystem, 0x19, 0x15,
            fn_80116988, (void*)"FE_GEN_SPLASH");
        FEAudio::SetSoundCategory(0x15);
        break;
    case 18:
        lbl_806E1058 += gAudioEnabled ? 1 : 0;
        g_pAudioSystem->GetBundleManager()->GetSoundMap()->UnloadBank(0x12);
        LoadSoundBank((GameAudio*)g_pAudioSystem, 0x31, 0x12,
            fn_80116988, (void*)"MUSICSTREAMS");
        break;
    case 19:
        mLoadingComment = "AsyncFinalizeCameraLoading";
        FinishLoadingStepOrUndo(this, fn_800F08A4());
        break;
    case 20:
        mLoadingComment = "AsyncFinalizeGameWorldLoading";
        FinishLoadingStepOrUndo(this, fn_802773B8(false));
        break;
    case 21:
        mLoadingComment = "AsyncFinalizeGameWorldLoadingForStadiumViewer";
        FinishLoadingStepOrUndo(this, fn_802773B8(true));
        break;
    case 22:
        mLoadingComment = "AsyncFinalizeLoadingAI";
        FinishLoadingStepOrUndo(this, fn_80311C5C());
        break;
    case 23:
        if (lbl_806E1060 == 0)
        {
            StopWithUndo();
            break;
        }
        glEndLoadTextureBundle(lbl_806E1060, (unsigned long)lbl_806E1064,
            glGetCurrentResourcePool(), 0);
        nlFree(lbl_806E1060);
        lbl_806E1060 = 0;
        FrontEnd::Initialize();
        FinishLoadingStep(this);
        break;
    case 24:
        mLoadingComment = "AsyncFinalizeLoadingINIFiles";
        FinishLoadingStepOrUndo(this, UpdateGameTweaksLoading(&gGameTweaks));
        break;
    case 25:
        mLoadingComment = "AsyncGameLocalizationBegin";
        if (FontManager::Instance() == 0)
        {
            nlSingleton<FontManager>::s_pInstance = new (8, false) FontManager;
        }
        fn_8030753C(FontManager::Instance(), GetFEResourcePool());
        lbl_806E18A0 = true;
        fn_80200380(&lbl_806E18A0);
        FinishLoadingStep(this);
        break;
    case 26:
        mLoadingComment = "AsyncHBMFinalize";
        FinishLoadingStepOrUndo(this, gpHBMManager->mReady);
        break;
    case 27:
        mLoadingComment = "AsyncHBMLoad";
        if (gpHBMManager == 0)
        {
            CreateInstance(gpHBMManager);
        }
        gpHBMManager->LoadResources();
        break;
    case 28:
        mLoadingComment = "AsyncInitializeEmissionManager";
        if (ParticleUpdateTask::sInstance->FinishLoading(sPersistentResourcePool))
        {
            glDiscardFrame(1);
            FinishLoadingStep(this);
        }
        else
        {
            StopWithUndo();
        }
        break;
    case 29:
        CharacterLoader_8056B290::sUnidentifiedInstance.fn_8000C130();
        break;
    case 30:
        CharacterLoader_8056B290::sUnidentifiedInstance.fn_8000C0FC();
        break;
    case 31:
        CharacterLoader_8056B290::sUnidentifiedInstance.fn_8000C22C();
        break;
    case 32:
        CharacterLoader_8056B290::sUnidentifiedInstance.fn_8000C1B0();
        break;
    case 33:
        mLoadingComment = "AsyncFELocalizationFinalize";
        FinishLoadingStepOrUndo(this, fn_8020078C(&lbl_806E18A0));
        break;
    case 34:
        mLoadingComment = "AsyncMiiLoadingBegin";
        if (g_pMiiManager == 0)
        {
            CreateInstance(g_pMiiManager);
        }
        g_pMiiManager->LoadResources();
        break;
    case 35:
        mLoadingComment = "AsyncMiiLoadingFinalize";
        FinishLoadingStepOrUndo(this, g_pMiiManager->mResourcesLoaded);
        break;
    case 36:
    {
        u32 value = m_SP[-1];
        bool stadiumViewer = value != 0;
        m_SP--;
        mLoadingComment = "AsyncStartCameraLoading";
        fn_800F030C(stadiumViewer);
        FinishLoadingStep(this);
        break;
    }
    case 37:
        mLoadingComment = "AsyncStartCameraLoadingForStadiumViewer";
        fn_800F06D4();
        break;
    case 38:
        fn_8011A2E8(this);
        break;
    case 39:
        mLoadingComment = "AsyncStartGameWorldLoadingForStadiumViewer";
        fn_802772D0(GetStadiumName(GameInfoManager::Instance()->GetStadium()), true);
        FinishLoadingStep(this);
        break;
    case 40:
        mLoadingComment = "AsyncStartLoadingAI";
        fn_80311AFC(fn_800E3198(), true);
        FinishLoadingStep(this);
        break;
    case 41:
        mLoadingComment = "AsyncStartLoadingEmissionManager";
        ParticleUpdateTask::sInstance->StartLoading(false, false, false, true);
        FinishLoadingStep(this);
        break;
    case 42:
        mLoadingComment = "AsyncStartLoadingGameFE";
        lbl_806E1060 = 0;
        lbl_806E1064 = 0;
        glBeginLoadTextureBundle("art/fe/fe.rlt", fn_8011A2DC, 0,
            glGetCurrentResourcePool());
        FinishLoadingStep(this);
        break;
    case 43:
        mLoadingComment = "AsyncStartLoadingINIFiles";
        InitializeGameTweaks(&gGameTweaks);
        FinishLoadingStep(this);
        break;
    case 44:
        mLoadingComment = "AsyncStartupFEWorldBegin";
        fn_801FE81C();
        FinishLoadingStep(this);
        break;
    case 45:
        mLoadingComment = "AsyncStartupFEWorldFinalize";
        FinishLoadingStepOrUndo(this, fn_801FE8EC());
        break;
    case 46:
        StopSound(0x7FC13AA3, this);
        break;
    case 47:
        mLoadingComment = "AsyncWaitForAllScenesValid";
        if (!FESceneManager::Instance()->AreAllScenesValid())
        {
            FESceneManager::Instance()->Update(0.0f);
            FEResourceManager::Instance()->Run(0.0f);
            StopWithUndo();
        }
        else
        {
            FinishLoadingStepOrUndo(this, lbl_806E1058 == 0);
        }
        break;
    case 48:
        break;
    case 49:
        --m_SP;
        break;
    case 50:
        CharacterLoader_8056B290::sUnidentifiedInstance.fn_80009BC8();
        break;
    case 51:
        m_SP++;
        m_SP[-1] = CharacterLoader_8056B290::sUnidentifiedInstance.fn_8000C124();
        break;
    case 52:
        m_SP++;
        m_SP[-1] = CharacterLoader_8056B290::sUnidentifiedInstance.fn_8000BD88();
        break;
    case 53:
        mLoadingComment = "CreateCharacterInstance";
        CharacterLoader_8056B290::sUnidentifiedInstance.fn_8000BA00();
        FinishLoadingStep(this);
        break;
    case 54:
        InitializeCrowdImpostors(GetStadiumUnknown0x34(
            GameInfoManager::Instance()->GetStadium()));
        break;
    case 55:
        gCrowdModelCollection.CreateLoadedModel();
        break;
    case 56:
        mLoadingComment = "CreateGameBall";
        g_pBall = new (8, false) cBall;
        FakeBallWorld::Init(g_pBall);
        FinishLoadingStep(this);
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
    case 61:
        --m_SP;
        break;
    case 62:
        mLoadingComment = "FinalizeAudio";
        FinishLoadingStepOrUndo(this,
            CharacterLoader_8056B290::sUnidentifiedInstance.fn_8000C254());
        break;
    case 63:
        CharacterLoader_8056B290::sUnidentifiedInstance.fn_8000BD70();
        break;
    case 64:
        mLoadingComment = "FinalizeLoadingAnimRetarget";
        FinishLoadingStepOrUndo(this,
            CharacterLoader_8056B290::sUnidentifiedInstance.fn_8000B230());
        break;
    case 65:
        mLoadingComment = "FinalizeLoadingCaptainOrGoalieAlternateSwapTexture";
        FinishLoadingStepOrUndo(this,
            CharacterLoader_8056B290::sUnidentifiedInstance.fn_8000BFA0());
        break;
    case 66:
        mLoadingComment = "FinalizeLoadingCharacterAnimations";
        FinishLoadingStepOrUndo(this,
            CharacterLoader_8056B290::sUnidentifiedInstance.fn_8000B00C());
        break;
    case 67:
        mLoadingComment = "FinalizeLoadingCharacterEffects";
        FinishLoadingStepOrUndo(this,
            CharacterLoader_8056B290::sUnidentifiedInstance.fn_8000A5D8());
        break;
    case 68:
    {
        u32* stackPointer = m_SP;
        u32 value = stackPointer[-1];
        m_SP = stackPointer - 1;
        mLoadingComment = "FinalizeLoadingCharacterModel";
        FinishLoadingStepOrUndo(this,
            CharacterLoader_8056B290::sUnidentifiedInstance.fn_8000A9A4(value));
        break;
    }
    case 69:
        mLoadingComment = "FinalizeLoadingCharacterPhysicsElements";
        FinishLoadingStepOrUndo(this,
            CharacterLoader_8056B290::sUnidentifiedInstance.fn_8000AD4C());
        break;
    case 70:
        mLoadingComment = "FinalizeLoadingCharacterTextures";
        FinishLoadingStepOrUndo(this,
            CharacterLoader_8056B290::sUnidentifiedInstance.fn_8000A144());
        break;
    case 71:
        mLoadingComment = "FinalizeLoadingCharacterTriggers";
        FinishLoadingStepOrUndo(this,
            CharacterLoader_8056B290::sUnidentifiedInstance.fn_8000B14C());
        break;
    case 72:
        mLoadingComment = "FinalizeLoadingCharINIFiles";
        FinishLoadingStepOrUndo(this,
            CharacterLoader_8056B290::sUnidentifiedInstance.fn_8000B9F4());
        break;
    case 73:
        mLoadingComment = "FinalizeLoadingCrowdCharacter";
        FinishLoadingStepOrUndo(this, gCrowdModelCollection.UpdateModelLoad());
        break;
    case 74:
        mLoadingComment = "FinalizeLoadingExtraTextures";
        FinishLoadingStepOrUndo(this,
            CharacterLoader_8056B290::sUnidentifiedInstance.fn_8000A790());
        break;
    case 75:
        mLoadingComment = "FinalizeLoadingHierarchy";
        FinishLoadingStepOrUndo(this,
            CharacterLoader_8056B290::sUnidentifiedInstance.fn_8000AB18());
        break;
    case 76:
        mLoadingComment = "FinalizeLoadingNPCTemplate";
        FinishLoadingStepOrUndo(this, gNPCManager->FinishLoadNPCTemplate());
        break;
    case 77:
        mLoadingComment = "FinalizeLoadingSharedTextures";
        FinishLoadingStepOrUndo(this,
            CharacterLoader_8056B290::sUnidentifiedInstance.fn_8000A378());
        break;
    case 78:
        mLoadingComment = "FinalizeLoadingShockTexture";
        FinishLoadingStepOrUndo(this,
            CharacterLoader_8056B290::sUnidentifiedInstance.fn_8000A224());
        break;
    case 79:
        mLoadingComment = "FinalizeLoadingSidekickSwapTexture";
        FinishLoadingStepOrUndo(this,
            CharacterLoader_8056B290::sUnidentifiedInstance.fn_8000B6C4());
        break;
    case 80:
        mLoadingComment = "FinalizeLoadingWorldNPCs";
        if (lbl_806E1050->UpdateModelLoading())
        {
            SetStadiumBannerTextures();
            FinishLoadingStep(this);
        }
        else
        {
            StopWithUndo();
        }
        break;
    case 81:
    {
        FileCache_80535C20* fileCache = fn_803733D4();
        fileCache->m_5D4 = true;
        fn_80370E20();
        break;
    }
    case 82:
        DestroyFEResourcePool();
        break;
    case 83:
        break;
    case 84:
        mLoadingState = 2;
        fn_80111660(2);
        DestroyFEResourcePool();
        CreateLargeFEResourcePool();
        CreatePadBackends();
        EnableAutoPressed();
        FEResourceManager::Instance()->LoadPermanentResourceBundle(
            "art/fe/BootUI.Res");
        if (GameSceneManager::Instance() == 0)
        {
            nlSingleton<GameSceneManager>::s_pInstance
                = new (8, false) GameSceneManager;
        }
        mLoadingState = 1;
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
    case 89:
        mLoadingComment = "AudioLoader";
        FinishLoadingStep(this);
        break;
    case 90:
        fn_8011A800(this);
        break;
    case 91:
        mLoadingComment = "InitializeGameStateFinalizeForStadimViewer";
        BeginFrameTask::s_FramerateLocked = false;
        InitializeGameObjectLighting();
        UseDefaultFreestyleButtonRemap(IsNetworkOrRecordedGame());
        mLoadingState = 1;
        FinishLoadingStep(this);
        break;
    case 92:
        fn_8011A570(this);
        break;
    case 93:
        mLoadingComment = "InitializeGameStateInGameFE2";
        FESceneManager::Instance()->GetTopSceneHandler()->SetVisible(false);
        g_pNetworkSession->NotifyGameLoaded();
        FinishLoadingStep(this);
        break;
    case 94:
        mLoadingComment = "NetworkWaitGameLoadedForEveryone";
        if (g_pNetworkSession->PollGameLoaded())
        {
            FinishLoadingStep(this);
        }
        else
        {
            StopWithUndo();
        }
        break;
    case 95:
        mLoadingComment = "PhysicsLoad";
        fn_8013D8DC();
        FinishLoadingStep(this);
        break;
    case 96:
        mLoadingComment = "PostCameraLoader";
        FrontEnd::EnterStartScreen(false);
        ReplayManager::Instance()->InitializeSnapshots();
        InitializeGameObjectLighting();
        FinishLoadingStep(this);
        break;
    case 97:
        mLoadingComment = "PhysicsPreGameLoad";
        fn_80144070();
        fn_8013D85C();
        FinishLoadingStep(this);
        break;
    case 98:
        fn_8011B6E8(this);
        break;
    case 99:
    {
        mLoadingComment = "InitializeStadiumViewer2";
        fn_80056CF4((void*)gGameTweaks.mTerrainType,
            gGameTweaks.mUnidentified08, gGameTweaks.mUnidentified0C);
        ParticleUpdateTask* particleUpdateTask = ParticleUpdateTask::sInstance;
        RLView* particleView = GetLayerView(eCLV_Particles);
        particleUpdateTask->Initialize(particleView, 0x5F6, 0x2FB);
        LoadCrowdCharacterList();
        FinishLoadingStep(this);
        break;
    }
    case 100:
        mLoadingComment = "InitializeWorldNPCTemplates";
        lbl_806E1050->LoadTemplates("ini/WorldNPCs.ini");
        break;
    case 101:
    {
        int value = m_SP[-1];
        BasicGameInfo* info = GameInfoManager::Instance()->GetCurrentGameInfo();
        bool found;
        for (short side = 0; side < 2; ++side)
        {
            if (value == info->mTeamIndex[side])
            {
                found = true;
                goto foundTeam;
            }
        }
        found = false;
    foundTeam:
        m_SP[-1] = found;
        if (m_RunState == 3)
        {
            m_SP[-1] = value;
        }
        break;
    }
    case 102:
    {
        int value = m_SP[-1];
        m_SP[-1] = GameInfoManager::Instance()->GetStadium() == value;
        if (m_RunState == 3)
        {
            m_SP[-1] = value;
        }
        break;
    }
    case 103:
    {
        int value = m_SP[-1];
        u32 found;
        for (short side = 0; side < 2; ++side)
        {
            short sideIndex = side;
            BasicGameInfo* info
                = GameInfoManager::Instance()->GetCurrentGameInfo();
            for (int slot = 0; slot < 3; ++slot)
            {
                if (value == info->mSidekickIndex[sideIndex][slot])
                {
                    found = true;
                    goto foundSidekick;
                }
            }
        }
        found = false;
    foundSidekick:
        m_SP[-1] = found;
        if (m_RunState == 3)
        {
            m_SP[-1] = value;
        }
        break;
    }
    case 104:
        m_SP++;
        m_SP[-1] = CharacterLoader_8056B290::sUnidentifiedInstance.fn_8000B1B8();
        break;
    case 105:
        m_SP++;
        m_SP[-1] = CharacterLoader_8056B290::sUnidentifiedInstance.fn_8000A870();
        break;
    case 106:
        m_SP++;
        m_SP[-1] = CharacterLoader_8056B290::sUnidentifiedInstance.fn_80009F48();
        break;
    case 107:
        m_SP++;
        m_SP[-1] = CharacterLoader_8056B290::sUnidentifiedInstance.fn_8000A2FC();
        break;
    case 108:
        m_SP++;
        m_SP[-1] = gNPCManager->SelectNextNPCTemplate();
        break;
    case 109:
        gNPCManager->CreateDiddyBanana();
        break;
    case 110:
        gNPCManager->CreateBirdoEgg();
        break;
    case 111:
        gNPCManager->CreateChainChomp();
        break;
    case 112:
        gNPCManager->fn_801A9C3C();
        break;
    case 113:
        gNPCManager->fn_801AA2C0();
        break;
    case 114:
        gNPCManager->fn_801A9BD0();
        break;
    case 115:
        gNPCManager->fn_801AA4C0();
        break;
    case 116:
        gNPCManager->fn_801A9DF0();
        break;
    case 117:
        gNPCManager->fn_801A9AF8();
        break;
    case 118:
        fn_80111664();
        break;
    case 119:
        m_SP++;
        m_SP[-1] = true;
        break;
    case 120:
    {
        u32* stackPointer = m_SP;
        u32 value = stackPointer[-1];
        m_SP = stackPointer - 1;
        fn_8011165C(value);
        break;
    }
    case 121:
        ReplayManager::Instance()->ResetSnapshots();
        break;
    case 122:
        m_SP++;
        m_SP[-1] = CharacterLoader_8056B290::sUnidentifiedInstance.fn_80009EFC();
        break;
    case 123:
        m_SP++;
        m_SP[-1] = gCrowdModelCollection.HasMoreModels();
        break;
    case 124:
        m_SP++;
        m_SP[-1] = CharacterLoader_8056B290::sUnidentifiedInstance.fn_8000C1A4();
        break;
    case 125:
        m_SP++;
        m_SP[-1] = CharacterLoader_8056B290::sUnidentifiedInstance.fn_8000B3C0();
        break;
    case 126:
        mLoadingComment = "StartLoadingAnimRetarget";
        CharacterLoader_8056B290::sUnidentifiedInstance.fn_8000B1F8();
        FinishLoadingStep(this);
        break;
    case 127:
        m_SP++;
        m_SP[-1] = CharacterLoader_8056B290::sUnidentifiedInstance.fn_8000BF04();
        break;
    case 128:
        mLoadingComment = "StartLoadingCharacterAnimations";
        CharacterLoader_8056B290::sUnidentifiedInstance.fn_8000AE90();
        FinishLoadingStep(this);
        break;
    case 129:
        mLoadingComment = "StartLoadingCharacterEffects";
        CharacterLoader_8056B290::sUnidentifiedInstance.fn_8000A418();
        FinishLoadingStep(this);
        break;
    case 130:
    {
        u32* stackPointer = m_SP;
        u32 value = stackPointer[-1];
        m_SP = stackPointer - 1;
        mLoadingComment = "StartLoadingCharacterModel";
        CharacterLoader_8056B290::sUnidentifiedInstance.fn_8000A8E4(value);
        FinishLoadingStep(this);
        break;
    }
    case 131:
        mLoadingComment = "StartLoadingCharacterPhysicsElements";
        CharacterLoader_8056B290::sUnidentifiedInstance.fn_8000ACEC();
        FinishLoadingStep(this);
        break;
    case 132:
        mLoadingComment = "StartLoadingCharacterTextures";
        CharacterLoader_8056B290::sUnidentifiedInstance.fn_80009FCC();
        FinishLoadingStep(this);
        break;
    case 133:
        mLoadingComment = "StartLoadingCharacterTriggers";
        CharacterLoader_8056B290::sUnidentifiedInstance.fn_8000B0E8();
        FinishLoadingStep(this);
        break;
    case 134:
        mLoadingComment = "StartLoadingCharINIFiles";
        CharacterLoader_8056B290::sUnidentifiedInstance.fn_8000B8E8();
        FinishLoadingStep(this);
        break;
    case 135:
        mLoadingComment = "StartLoadingCrowdCharacter";
        CurrentAllocator = &VirtualAllocator;
        AllocatorStack[AllocatorStackDepth++] = &VirtualAllocator;
        gCrowdModelCollection.BeginNextModelLoad();
        --AllocatorStackDepth;
        AllocatorStack[AllocatorStackDepth] = 0;
        CurrentAllocator = AllocatorStack[AllocatorStackDepth - 1];
        FinishLoadingStep(this);
        break;
    case 136:
        m_SP++;
        m_SP[-1] = CharacterLoader_8056B290::sUnidentifiedInstance.fn_8000A67C();
        break;
    case 137:
        mLoadingComment = "StartLoadingHierarchy";
        CharacterLoader_8056B290::sUnidentifiedInstance.fn_8000AAB8();
        FinishLoadingStep(this);
        break;
    case 138:
        mLoadingComment = "StartLoadingNPCTemplate";
        gNPCManager->BeginLoadNPCTemplate();
        FinishLoadingStep(this);
        break;
    case 139:
        mLoadingComment = "StartLoadingSharedTextures";
        CharacterLoader_8056B290::sUnidentifiedInstance.fn_8000A324();
        FinishLoadingStep(this);
        break;
    case 140:
        mLoadingComment = "StartLoadingShockTextures";
        CharacterLoader_8056B290::sUnidentifiedInstance.fn_8000A0A8();
        FinishLoadingStep(this);
        break;
    case 141:
        m_SP++;
        m_SP[-1] = CharacterLoader_8056B290::sUnidentifiedInstance.fn_8000B3E0();
        break;
    case 142:
        mLoadingComment = "StartLoadingWorldNPCs";
        lbl_806E1050->BeginModelLoading();
        FinishLoadingStep(this);
        break;
    case 143:
    {
        BootLoadingScene* loadingScene;
        if (g_pLocalization->m_CurrentLanguage == nlLocalization::LangJapanese)
        {
            loadingScene = static_cast<BootLoadingScene*>(
                GameSceneManager::Instance()->GetScene(SCENE_BOOT_LOADING_JPN));
        }
        else
        {
            loadingScene = static_cast<BootLoadingScene*>(
                GameSceneManager::Instance()->GetScene(SCENE_BOOT_LOADING));
        }
        if (loadingScene == 0 || loadingScene->mPhase == 4)
        {
            FinishLoadingStep(this);
        }
        else
        {
            StopWithUndo();
        }
        break;
    }
    default:
        nlBreak();
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

extern "C" void fn_8011A2DC(void* value0, unsigned long value1, void*)
{
    lbl_806E1060 = value0;
    lbl_806E1064 = (void*)value1;
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
    NisPlayer::Instance()->fn_8027ED18();
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
    fn_802DB9C4(&gCrowdModelCollection);
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

extern "C" void fn_8011B40C(AudioResourceLoadOwner*, void*)
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
