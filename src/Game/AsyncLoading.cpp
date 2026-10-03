#include "Game/Font/FontLoading.h"
#include "Game/MiiManager.h"
#include "Game/AI/FuzzyAIRuntime.h"
#include "Game/Physics/PhysicsEventQueue.h"
#include "Game/Physics/Physics.h"
#include "Game/HBMManager.h"
#include "Game/ObjectBlur.h"

#include "Game/Sys/audio.h"
#include "Game/Render/StadiumLoading.h"
#include "Game/Render/ShootToScoreMeter.h"
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
#include "Game/Goalie.h"
#include "NL/nlFunctionMemory.h"
#include "Game/EventDataTypes.h"
#include "Game/Event.h"
#include "Game/Sys/debug.h"
#include "Game/FE/feMusic.h"
#include "Game/FE/FEAudio.h"
#include "Game/FE/feCamera.h"
#include "Game/FE/feInput.h"
#include "Game/OverlayManager.h"
#include "Game/Render/FrontEndPresentation.h"
#include "Game/SH/SHChallengeSelect.h"
#include "Game/SH/SHHallOfFame.h"
#include "Game/DB/SaveLoad.h"
#include "Game/Effects/EmissionManager.h"
#include "Game/EventRegistry.h"
#include "Game/ExcitementSystem.h"
#include "Game/Camera/tu_800F9460.h"
#include "Game/Render/Presentation.h"
#include "Game/SH/SHNavigation.h"
#include "Game/FE/feModelManager.h"
#include "Game/Render/ImpostorManager.h"
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
#include "Game/UnidentifiedStaticStorage.h"
#include "Game/Audio/RegistryPools.h"
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
#include "Game/FE/feHelpFuncs_decl.h"
#include "Game/FE/feSceneManager.h"
#include "Game/SH/SHPause.h"
#include "Game/Task/FixedUpdateTask.h"
#include "Game/Game.h"
#include "Game/Weather.h"
#include "Game/NetworkSession.h"
#include "Game/NisPlayer.h"
#include "Game/Team.h"
#include "Game/PadActions.h"
#include "Game/Physics/PhysicsFakeBall.h"
#include "Game/Physics/PhysicsPatch.h"
#include "Game/Render/ElectricFence.h"
#include "Game/Render/FlareHandler.h"
#include "Game/Render/FlyingCamera.h"
#include "Game/ReplayChoreo.h"
#include "Game/ReplayManager.h"
#include "NL/gl/glMemory.h"
#include "NL/gl/glTexture.h"
#include "Game/SAnim.h"
#include "Game/SAnim/pnBlender.h"
#include "Game/SAnim/pnFeather.h"
#include "Game/SAnim/pnSAnimController.h"
#include "Game/SAnim/pnSingleAxisBlender.h"
#include "Game/SAnim/pnScaleBlender.h"
#include "Game/Task/ParticleUpdateTask.h"
#include "Game/Transitions/ScreenTransitionManager.h"
#include "Game/TweakValue.h"
#include "NL/nlFunction.inl"
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
#include "NL/plat/nlFileCache.h"
#include "types.h"
#include "Game/Render/TimedObject.h"
#include "Game/InputManager.h"
#include "Game/NetworkInput.h"
#include "Game/NetworkSync.h"
#include "NL/nlstring_tmpl.h"
#include "Game/main.h"

#include <revolution/os/OSThread.h>
#include <revolution/os/OSTime_fwd.h>

#define OS_BUS_CLOCK_SPEED           (*(volatile u32*)0x800000F8)
#define OS_TIME_SPEED                (OS_BUS_CLOCK_SPEED / 4)
#define OSTicksToMilliseconds(ticks) ((ticks) / (OS_TIME_SPEED / 1000))

bool IsNetworkOrRecordedGame();

void ShutdownWarbleRendering(void*);
extern "C" bool fn_802773B8(bool stadiumViewer);
extern "C" bool fn_80277DD4(ImpostorModel*);
extern "C" void fn_80194EF8(ReplayChoreo*);
extern "C" void fn_8001FE80();
extern "C" bool fn_802F49C0(const u32* bindingKey, const u32* definitionKey,
    void* parameterData, bool immediate, float value);
void fn_80056CF4(void*, int, bool);
extern "C" void fn_8030753C(FontManager*, GLResourcePool*);

void FreeImpostorLighting();

void fn_80056EA8();
void DestroyCharacters();

extern SlotPool<cSAnimCallback> lbl_805840D8;
extern SlotPoolBase lbl_8057AB80;

bool g_VerboseAudio;
bool g_bDumpMemoryStatsOnLoad;
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
    &g_bDumpMemoryStatsOnLoad, true);
static TweakFloatBinding lbl_8056E498(
    "g_fScriptBlockingWarningMS", "Loading",
    &g_fScriptBlockingWarningMS, true);
static TweakFloatBinding lbl_8056E4B8(
    "g_fYieldScriptBlockingTimeMS", "Loading",
    &g_fYieldScriptBlockingTimeMS, true);

static AsyncLoadingManager sAsyncLoadingManager;

static inline void ReleaseUnidentifiedOwner(UnidentifiedOwnerHandle* handle)
{
    if (handle != 0 && handle->mOwner != 0
        && ((handle->mOwner->mFlags >> 30) & 1) != 0)
    {
        handle->mOwner->mTarget->Release(handle);
    }
}

struct PersistentResourceRequirements
{
    GLMemoryRequirement entries[2];
};

extern PersistentResourceRequirements gPersistentResourceRequirements;

static inline bool IsTeamInCurrentGame(int team)
{
    BasicGameInfo* info = GameInfoManager::Instance()->GetCurrentGameInfo();
    for (short side = 0; side < 2; ++side)
    {
        if (team == info->mTeamIndex[side])
            return true;
    }
    return false;
}

static inline bool IsSidekickInCurrentGame(eSidekickID sidekick)
{
    for (short side = 0; side < 2; ++side)
    {
        short sideIndex = side;
        GameInfoManager* info = GameInfoManager::Instance();
        for (int slot = 0; slot < 3; ++slot)
        {
            if (sidekick == info->GetCurrentGameInfo()->GetSidekick(sideIndex, slot))
                return true;
        }
    }
    return false;
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
        PersistentResourceRequirements requirements = gPersistentResourceRequirements;
        sPersistentResourcePool = glCreateResourcePool(
            requirements.entries, 2, "PersistentResourcePool");
        break;
    }
    case 5:
        m_SP++;
        m_SP[-1] = CharacterLoader::sInstance.ShareDuplicateAnimInventory();
        break;
    case 6:
        fn_8011B2E4(this);
        break;
    case 7:
        SetLoadingComment("AsyncFEGameResourceLoadFinalize");
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
        SetLoadingComment("AsyncFELocalizationBegin");
        if (FontManager::Instance() == 0)
        {
            nlSingleton<FontManager>::s_pInstance = new (8, false) FontManager;
        }
        fn_8030753C(FontManager::Instance(), GetFEResourcePool());
        gLoadInGameFonts = false;
        BeginFontLoading(&gLoadInGameFonts);
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
        SetLoadingComment("AsyncFEResourceLoadFinalize");
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
        SetLoadingComment("AsyncFinalizeCameraLoading");
        FinishLoadingStepOrUndo(this, AsyncFinalizeCameraLoading());
        break;
    case 20:
        SetLoadingComment("AsyncFinalizeGameWorldLoading");
        FinishLoadingStepOrUndo(this, fn_802773B8(false));
        break;
    case 21:
        SetLoadingComment("AsyncFinalizeGameWorldLoadingForStadiumViewer");
        FinishLoadingStepOrUndo(this, fn_802773B8(true));
        break;
    case 22:
        SetLoadingComment("AsyncFinalizeLoadingAI");
        FinishLoadingStepOrUndo(this, ApplyFuzzyByteCode());
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
        SetLoadingComment("AsyncFinalizeLoadingINIFiles");
        FinishLoadingStepOrUndo(this, UpdateGameTweaksLoading(&gGameTweaks));
        break;
    case 25:
        SetLoadingComment("AsyncGameLocalizationBegin");
        if (FontManager::Instance() == 0)
        {
            nlSingleton<FontManager>::s_pInstance = new (8, false) FontManager;
        }
        fn_8030753C(FontManager::Instance(), GetFEResourcePool());
        gLoadInGameFonts = true;
        BeginFontLoading(&gLoadInGameFonts);
        FinishLoadingStep(this);
        break;
    case 26:
        SetLoadingComment("AsyncHBMFinalize");
        FinishLoadingStepOrUndo(this, gpHBMManager->mReady);
        break;
    case 27:
        SetLoadingComment("AsyncHBMLoad");
        if (gpHBMManager == 0)
        {
            CreateInstance(gpHBMManager);
        }
        gpHBMManager->LoadResources();
        break;
    case 28:
        SetLoadingComment("AsyncInitializeEmissionManager");
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
        CharacterLoader::sInstance.StartLoadingCaptainAudio();
        break;
    case 30:
        CharacterLoader::sInstance.StartLoadingAudioBank0();
        break;
    case 31:
        CharacterLoader::sInstance.StartLoadingAudioBank13();
        break;
    case 32:
        CharacterLoader::sInstance.StartLoadingSidekickAudio();
        break;
    case 33:
        SetLoadingComment("AsyncFELocalizationFinalize");
        FinishLoadingStepOrUndo(this, UpdateFontLoading(&gLoadInGameFonts));
        break;
    case 34:
        SetLoadingComment("AsyncMiiLoadingBegin");
        if (MiiManager::s_pInstance == 0)
        {
            CreateInstance(MiiManager::s_pInstance);
        }
        MiiManager::s_pInstance->LoadResources();
        break;
    case 35:
        SetLoadingComment("AsyncMiiLoadingFinalize");
        FinishLoadingStepOrUndo(this, MiiManager::s_pInstance->mResourcesLoaded);
        break;
    case 36:
    {
        bool frontEnd = m_SP[-1] != 0;
        m_SP--;
        SetLoadingComment("AsyncStartCameraLoading");
        AsyncStartCameraLoading(frontEnd);
        FinishLoadingStep(this);
        break;
    }
    case 37:
        SetLoadingComment("AsyncStartCameraLoadingForStadiumViewer");
        AsyncStartCameraLoadingForStadiumViewer();
        break;
    case 38:
        fn_8011A2E8(this);
        break;
    case 39:
        SetLoadingComment("AsyncStartGameWorldLoadingForStadiumViewer");
        fn_802772D0(GetStadiumName(GameInfoManager::Instance()->GetStadium()), true);
        FinishLoadingStep(this);
        break;
    case 40:
        SetLoadingComment("AsyncStartLoadingAI");
        LoadFuzzyByteCode(GetFuzzyAIScriptFilename(), true);
        FinishLoadingStep(this);
        break;
    case 41:
        SetLoadingComment("AsyncStartLoadingEmissionManager");
        ParticleUpdateTask::sInstance->StartLoading(false, false, false, true);
        FinishLoadingStep(this);
        break;
    case 42:
        SetLoadingComment("AsyncStartLoadingGameFE");
        lbl_806E1060 = 0;
        lbl_806E1064 = 0;
        glBeginLoadTextureBundle("art/fe/fe.rlt", fn_8011A2DC, 0,
            glGetCurrentResourcePool());
        FinishLoadingStep(this);
        break;
    case 43:
        SetLoadingComment("AsyncStartLoadingINIFiles");
        InitializeGameTweaks(&gGameTweaks);
        FinishLoadingStep(this);
        break;
    case 44:
        SetLoadingComment("AsyncStartupFEWorldBegin");
        BeginLoadFEWorld();
        FinishLoadingStep(this);
        break;
    case 45:
        SetLoadingComment("AsyncStartupFEWorldFinalize");
        FinishLoadingStepOrUndo(this, FinishLoadFEWorld());
        break;
    case 46:
        StopSound(0x7FC13AA3, this);
        break;
    case 47:
        SetLoadingComment("AsyncWaitForAllScenesValid");
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
        CharacterLoader::sInstance.BuildCharacterList();
        break;
    case 51:
        m_SP++;
        m_SP[-1] = CharacterLoader::sInstance.NeedsCaptainAudio();
        break;
    case 52:
        m_SP++;
        m_SP[-1] = CharacterLoader::sInstance.HasAlternateSwapTexture();
        break;
    case 53:
        SetLoadingComment("CreateCharacterInstance");
        CharacterLoader::sInstance.CreateCharacterInstance();
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
        SetLoadingComment("CreateGameBall");
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
        SetLoadingComment("FinalizeAudio");
        FinishLoadingStepOrUndo(this,
            CharacterLoader::sInstance.FinalizeAudio());
        break;
    case 63:
        CharacterLoader::sInstance.fn_8000BD70();
        break;
    case 64:
        SetLoadingComment("FinalizeLoadingAnimRetarget");
        FinishLoadingStepOrUndo(this,
            CharacterLoader::sInstance.FinalizeLoadingAnimRetarget());
        break;
    case 65:
        SetLoadingComment("FinalizeLoadingCaptainOrGoalieAlternateSwapTexture");
        FinishLoadingStepOrUndo(this,
            CharacterLoader::sInstance.FinalizeLoadingCaptainOrGoalieAlternateSwapTexture());
        break;
    case 66:
        SetLoadingComment("FinalizeLoadingCharacterAnimations");
        FinishLoadingStepOrUndo(this,
            CharacterLoader::sInstance.FinalizeLoadingCharacterAnimations());
        break;
    case 67:
        SetLoadingComment("FinalizeLoadingCharacterEffects");
        FinishLoadingStepOrUndo(this,
            CharacterLoader::sInstance.FinalizeLoadingCharacterEffects());
        break;
    case 68:
    {
        int value = m_SP[-1];
        m_SP--;
        SetLoadingComment("FinalizeLoadingCharacterModel");
        FinishLoadingStepOrUndo(this,
            CharacterLoader::sInstance.FinalizeLoadingCharacterModel(value));
        break;
    }
    case 69:
        SetLoadingComment("FinalizeLoadingCharacterPhysicsElements");
        FinishLoadingStepOrUndo(this,
            CharacterLoader::sInstance.FinalizeLoadingCharacterPhysicsElements());
        break;
    case 70:
        SetLoadingComment("FinalizeLoadingCharacterTextures");
        FinishLoadingStepOrUndo(this,
            CharacterLoader::sInstance.FinalizeLoadingCharacterTextures());
        break;
    case 71:
        SetLoadingComment("FinalizeLoadingCharacterTriggers");
        FinishLoadingStepOrUndo(this,
            CharacterLoader::sInstance.FinalizeLoadingCharacterTriggers());
        break;
    case 72:
        SetLoadingComment("FinalizeLoadingCharINIFiles");
        FinishLoadingStepOrUndo(this,
            CharacterLoader::sInstance.FinalizeLoadingCharINIFiles());
        break;
    case 73:
        SetLoadingComment("FinalizeLoadingCrowdCharacter");
        FinishLoadingStepOrUndo(this, gCrowdModelCollection.UpdateModelLoad());
        break;
    case 74:
        SetLoadingComment("FinalizeLoadingExtraTextures");
        FinishLoadingStepOrUndo(this,
            CharacterLoader::sInstance.FinalizeLoadingExtraTextures());
        break;
    case 75:
        SetLoadingComment("FinalizeLoadingHierarchy");
        FinishLoadingStepOrUndo(this,
            CharacterLoader::sInstance.FinalizeLoadingHierarchy());
        break;
    case 76:
        SetLoadingComment("FinalizeLoadingNPCTemplate");
        FinishLoadingStepOrUndo(this, gNPCManager->FinishLoadNPCTemplate());
        break;
    case 77:
        SetLoadingComment("FinalizeLoadingSharedTextures");
        FinishLoadingStepOrUndo(this,
            CharacterLoader::sInstance.FinalizeLoadingSharedTextures());
        break;
    case 78:
        SetLoadingComment("FinalizeLoadingShockTexture");
        FinishLoadingStepOrUndo(this,
            CharacterLoader::sInstance.FinalizeLoadingShockTexture());
        break;
    case 79:
        SetLoadingComment("FinalizeLoadingSidekickSwapTexture");
        FinishLoadingStepOrUndo(this,
            CharacterLoader::sInstance.FinalizeLoadingSidekickSwapTexture());
        break;
    case 80:
        SetLoadingComment("FinalizeLoadingWorldNPCs");
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
        nlFileCache* fileCache = nlGetFileCache();
        fileCache->mCacheWritesEnabled = true;
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
        SetLoadingComment("AudioLoader");
        FinishLoadingStep(this);
        break;
    case 90:
        fn_8011A800(this);
        break;
    case 91:
        SetLoadingComment("InitializeGameStateFinalizeForStadimViewer");
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
        SetLoadingComment("InitializeGameStateInGameFE2");
        FESceneManager::Instance()->GetTopSceneHandler()->SetVisible(false);
        g_pNetworkSession->NotifyGameLoaded();
        FinishLoadingStep(this);
        break;
    case 94:
        SetLoadingComment("NetworkWaitGameLoadedForEveryone");
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
        SetLoadingComment("PhysicsLoad");
        CreateStaticPhysicsPrimitives();
        FinishLoadingStep(this);
        break;
    case 96:
        SetLoadingComment("PostCameraLoader");
        FrontEnd::EnterStartScreen(false);
        ReplayManager::Instance()->InitializeSnapshots();
        InitializeGameObjectLighting();
        FinishLoadingStep(this);
        break;
    case 97:
        SetLoadingComment("PhysicsPreGameLoad");
        CreatePhysicsEventQueue();
        CreatePhysicsWorld();
        FinishLoadingStep(this);
        break;
    case 98:
        fn_8011B6E8(this);
        break;
    case 99:
    {
        SetLoadingComment("InitializeStadiumViewer2");
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
        SetLoadingComment("InitializeWorldNPCTemplates");
        lbl_806E1050->LoadTemplates("ini/WorldNPCs.ini");
        break;
    case 101:
    {
        int value = m_SP[-1];
        m_SP[-1] = IsTeamInCurrentGame(value);
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
        eSidekickID value = (eSidekickID)m_SP[-1];
        m_SP[-1] = IsSidekickInCurrentGame(value);
        if (m_RunState == 3)
        {
            m_SP[-1] = value;
        }
        break;
    }
    case 104:
        m_SP++;
        m_SP[-1] = CharacterLoader::sInstance.HasAnimRetarget();
        break;
    case 105:
        m_SP++;
        m_SP[-1] = CharacterLoader::sInstance.AcquireCurrentTemplate();
        break;
    case 106:
        m_SP++;
        m_SP[-1] = CharacterLoader::sInstance.NeedsCharacterTextures();
        break;
    case 107:
        m_SP++;
        m_SP[-1] = CharacterLoader::sInstance.NeedsSharedTextures();
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
        gNPCManager->CreateDaisyFists();
        break;
    case 113:
        gNPCManager->CreateHammers();
        break;
    case 114:
        gNPCManager->CreateKoopaShell();
        break;
    case 115:
        gNPCManager->CreateThwomps();
        break;
    case 116:
        gNPCManager->CreateWindDebris();
        break;
    case 117:
        gNPCManager->CreateYoshiEgg();
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
        m_SP[-1] = CharacterLoader::sInstance.NextCharacter();
        break;
    case 123:
        m_SP++;
        m_SP[-1] = gCrowdModelCollection.HasMoreModels();
        break;
    case 124:
        m_SP++;
        m_SP[-1] = CharacterLoader::sInstance.NeedsSidekickAudio();
        break;
    case 125:
        m_SP++;
        m_SP[-1] = CharacterLoader::sInstance.NeedsSidekickSwapTexture();
        break;
    case 126:
        SetLoadingComment("StartLoadingAnimRetarget");
        CharacterLoader::sInstance.StartLoadingAnimRetarget();
        FinishLoadingStep(this);
        break;
    case 127:
        m_SP++;
        m_SP[-1] = CharacterLoader::sInstance.StartLoadingCaptainOrGoalieAlternateSwapTexture();
        break;
    case 128:
        SetLoadingComment("StartLoadingCharacterAnimations");
        CharacterLoader::sInstance.StartLoadingCharacterAnimations();
        FinishLoadingStep(this);
        break;
    case 129:
        SetLoadingComment("StartLoadingCharacterEffects");
        CharacterLoader::sInstance.StartLoadingCharacterEffects();
        FinishLoadingStep(this);
        break;
    case 130:
    {
        int value = m_SP[-1];
        m_SP--;
        SetLoadingComment("StartLoadingCharacterModel");
        CharacterLoader::sInstance.StartLoadingCharacterModel(value);
        FinishLoadingStep(this);
        break;
    }
    case 131:
        SetLoadingComment("StartLoadingCharacterPhysicsElements");
        CharacterLoader::sInstance.StartLoadingCharacterPhysicsElements();
        FinishLoadingStep(this);
        break;
    case 132:
        SetLoadingComment("StartLoadingCharacterTextures");
        CharacterLoader::sInstance.StartLoadingCharacterTextures();
        FinishLoadingStep(this);
        break;
    case 133:
        SetLoadingComment("StartLoadingCharacterTriggers");
        CharacterLoader::sInstance.StartLoadingCharacterTriggers();
        FinishLoadingStep(this);
        break;
    case 134:
        SetLoadingComment("StartLoadingCharINIFiles");
        CharacterLoader::sInstance.StartLoadingCharINIFiles();
        FinishLoadingStep(this);
        break;
    case 135:
        SetLoadingComment("StartLoadingCrowdCharacter");
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
        m_SP[-1] = CharacterLoader::sInstance.StartLoadingExtraTextures();
        break;
    case 137:
        SetLoadingComment("StartLoadingHierarchy");
        CharacterLoader::sInstance.StartLoadingHierarchy();
        FinishLoadingStep(this);
        break;
    case 138:
        SetLoadingComment("StartLoadingNPCTemplate");
        gNPCManager->BeginLoadNPCTemplate();
        FinishLoadingStep(this);
        break;
    case 139:
        SetLoadingComment("StartLoadingSharedTextures");
        CharacterLoader::sInstance.StartLoadingSharedTextures();
        FinishLoadingStep(this);
        break;
    case 140:
        SetLoadingComment("StartLoadingShockTextures");
        CharacterLoader::sInstance.StartLoadingShockTextures();
        FinishLoadingStep(this);
        break;
    case 141:
        m_SP++;
        m_SP[-1] = CharacterLoader::sInstance.StartLoadingSidekickSwapTexture();
        break;
    case 142:
        SetLoadingComment("StartLoadingWorldNPCs");
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

AsyncLoadingManager::AsyncLoadingManager()
    : InterpreterCore(100)
{
    mLoadingHandle.mOwner = 0;
    mByteCode = 0;
    mSequenceState = ASYNC_LOADING_IDLE;
    mLoadingState = 0;
    mPreviousStageTick = 0;
    mSequenceStartTime = 0;
    mStageStartTick = 0;
    lbl_806E103C = 0;
    SetLoadingComment("No Loading Comment");
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

    int result = ASYNC_LOADING_NO_TRANSITION;
    manager->mStageStartTick = nlGetTicker();
    manager->SetLoadingComment("No Loading Comment");

    bool completed = false;
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

    float elapsed = nlGetTickerDifference(manager->mStageStartTick, nlGetTicker());
    if (elapsed > g_fScriptBlockingWarningMS)
    {
        tDebugPrintManager::Print(DC_LOADER, "Fixme: %s blocked for %fMS\n",
            manager->mLoadingComment, elapsed);
    }

    if (completed)
    {
        lbl_806E105C = nlGetTimeDifference(manager->mSequenceStartTime, nlGetTime());
        char buffer[200];
        nlSNPrintf(buffer, sizeof(buffer), "Total Load Time %f MS\n", lbl_806E105C);
        tDebugPrintManager::Print(DC_LOADER, buffer);
        fn_802BD718("Total Load Time", "seconds", lbl_806E105C / 1000.0f);
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
    manager->SetLoadingComment("DestroyFEFast");
    g_pFEInput->Reset(true);
    while (!FESceneManager::Instance()->AreAllScenesValid())
    {
        nlServiceFileSystem();
        FESceneManager::Instance()->Update(0.0f);
        FEResourceManager::Instance()->Run(0.0f);
    }
    GameSceneManager::Instance()->PopEntireStack();
    if (GameSceneManager::s_pInstance != 0)
    {
        delete GameSceneManager::s_pInstance;
        GameSceneManager::s_pInstance = 0;
    }

    g_pAudioSystem->GetBundleManager()->GetSoundMap()->UnloadBank(0x15);
    FESceneManager::Instance()->ForceImmediateStackProcessing();
    FEResourceManager::Instance()->UnloadPermanentResourceBundle();
    FEResourceManager::Instance()->Cleanup();
    DestroyFEResourcePool();
    DestroyPadBackends();
    cPN_SAnimController::m_SAnimControllerSlotPool.FreeBlocks();
    cPN_Blender::m_BlenderSlotPool.FreeBlocks();
    cPN_SingleAxisBlender::m_SingleAxisBlenderSlotPool.FreeBlocks();
    cPN_Feather::m_FeatherSlotPool.FreeBlocks();
    cPN_ScaleBlender::m_ScaleBlenderSlotPool.FreeBlocks();
    lbl_805840D8.FreeBlocks();
    if (g_bTweaking)
    {
        fn_802BDA28();
    }
    ResetDynamicTweaks();
    glCompact();
    fn_802B467C(&lbl_8057AB80);
    SlotPoolBase::BaseFreeBlocks(&lbl_8057AB80, 8);

    fn_80111658(2);
    manager->mLoadingState = 0;
    FinishLoadingStep(manager);
}

extern "C" void fn_80119454(AsyncLoadingManager* manager)
{
    manager->mLoadingState = 2;
    manager->SetLoadingComment("InitializeFEState1");
    fn_80111654(2);
    ClearTweakRegistryReset();
    GLMemoryRequirement requirements[] = {
        { GLM_Header, 0x100000 },
        { GLM_TextureData, 0xA00000 },
    };
    glSetCurrentResourcePool(glCreateResourcePool(requirements, 2, "FE"));
    manager->mUnidentified4C = (void*)glGetCurrentResourcePool()->MarkResource();
    FinishLoadingStep(manager);
    if (GameInfoManager::Instance()->mCurrentMode == GameInfoManager::GM_MODE_3)
    {
        CupManager::Instance()->AdvanceToNextUserGame();
    }
}

extern "C" void fn_80119528(AsyncLoadingManager* manager)
{
    static GLMemoryRequirement requirements[] = {
        { GLM_Header, 0x400 },
        { GLM_VertexData, 0x800 },
    };
    GLView* view = GetLayerView((eCLV)0);
    ImpostorManager::GetInstance()->Initialize(view, 30, requirements, 2, true);
    ImpostorManager::GetInstance()->SetEnabled(true);
    manager->SetLoadingComment("InitializeFEState2");
    ParticleUpdateTask* particleTask = ParticleUpdateTask::sInstance;
    particleTask->Initialize(GetLayerView((eCLV)0x19), 0x5F6, 0x2FB);
    GameInfoManager::Instance()->ResetUnknown0xA0();
    FEMusic::StopStream();
    BaseSceneHandler* scene = GameSceneManager::Instance()->Push((SceneList)0x1D, SCREEN_NOTHING, false);
    FESceneManager::Instance()->SetTopMostScene(scene);
    SetPointerEnabled(false);
    if (!lbl_806E1044)
    {
        lbl_806E1044 = true;
        GameSceneManager::Instance()->Push((SceneList)0x16, SCREEN_NOTHING, false);
        FrontEndPresentation::GetInstance()->Call("StartTitleScreenSequence");
    }
    else
    {
        SetPointerEnabled(true);
        if (g_e3_Build)
        {
            for (int i = 0; i < 4; ++i)
            {
                GameInfoManager::Instance()->SetPlayingSide(i, -1);
            }
        }
        if (NetTournManager::Instance()->mState != 0)
        {
            NetTournManager::Instance()->NotifyFinishedLoadingToKnockout();
            FrontEndPresentation::GetInstance()->Call("TransitionGameToOnlineTournament");
            GameSceneManager::Instance()->Push((SceneList)0x22, SCREEN_NOTHING, false);
        }
        else if (GameInfoManager::Instance()->IsOnline())
        {
            FEAudio::PlayAnimAudioEvent(0x37A9934D, 0, 0, true);
            GameSceneManager::Instance()->Push((SceneList)0x28, SCREEN_NOTHING, false);
            FrontEndPresentation::GetInstance()->Call("TransitionGameToOnlineMainMenu");
        }
        else if (GameInfoManager::Instance()->IsInMode2())
        {
            GameSceneManager::Instance()->Push(SCENE_TITLE, SCREEN_NOTHING, false);
            FrontEndPresentation::GetInstance()->Call("StartTitleScreenSequence");
            GameInfoManager::Instance()->SetMode(GameInfoManager::GM_MODE_2, 0);
        }
        else if (GameInfoManager::Instance()->IsInFriendlyMode())
        {
            if (GameInfoManager::Instance()->unknown_0x71C8 == 2)
            {
                FrontEndPresentation::GetInstance()->Call("TransitionGameToChooseCaptains");
                GameSceneManager::Instance()->Push(SCENE_CHOOSE_CAPTAINS_DOMINATION, SCREEN_BACK, false);
                FEMusic::StartStreamIfDifferent(1);
            }
            else
            {
                FEAudio::PlayAnimAudioEvent(0x80060B2D, 0, 0, true);
                FrontEndPresentation::GetInstance()->Call("StartMainMenuSequence");
            }
        }
        else if (GameInfoManager::Instance()->IsInMode3())
        {
            CupManager* cup = CupManager::Instance();
            if (cup->GetCurrentRoundNumber() == -5 && cup->mState == 4)
            {
                if (cup->GetCurrentMode() == 0)
                {
                    SetUnlockFlag(1);
                    cup->SaveCupRecord();
                }
                else if (cup->GetCurrentMode() == 1)
                {
                    SetUnlockFlag(2);
                    cup->SaveCupRecord();
                }
                else
                {
                    SetUnlockFlag(4);
                }
            }
            CupManager::Instance()->ShowRoundNews();
            CupManager::Instance()->mGameInProgress = false;
            CupManager::Instance()->AwardGoalTrophies();
            SaveLoad::StartSave(false);
            FrontEndPresentation::GetInstance()->Call("TransitionGameToStrikerCup");
        }
        else if (GameInfoManager::Instance()->IsInMode4())
        {
            if (GameInfoManager::Instance()->unknown_0x71C8 == 1)
            {
                ChallengeSelectScene* challengeScene = static_cast<ChallengeSelectScene*>(
                    GameSceneManager::Instance()->Push(g_pStrikerChallenge->mCurrentChallenge < 10 ? (SceneList)0x4C : (SceneList)0x4B,
                        SCREEN_NOTHING, false));
                if (challengeScene != 0)
                {
                    challengeScene->mChallengeOffset = g_pStrikerChallenge->mChallengeOffset;
                }
                FrontEndPresentation::GetInstance()->Call("TransitionGameToStrikerChallenge");
            }
            else
            {
                FEAudio::PlayAnimAudioEvent(0x80060B2D, 0, 0, true);
                FrontEndPresentation::GetInstance()->Call("StartMainMenuSequence");
            }
        }
    }
    StopSound(0x7FC13AA3, manager);
    g_pAudioSystem->GetBundleManager()->GetSoundMap()->UnloadBank(0x12);
    {
        int parameter[2];
        parameter[0] = 1;
        parameter[1] = 0;
        u32 binding;
        u32 definition;
        definition = nlStringLowerHash("SendToSpeaker");
        binding = nlStringLowerHash("ControllerSpeaker1");
        fn_802F49C0(&binding, &definition, parameter, false, 0.0f);
    }
    {
        int parameter[2];
        parameter[0] = 2;
        parameter[1] = 0;
        u32 binding;
        u32 definition;
        definition = nlStringLowerHash("SendToSpeaker");
        binding = nlStringLowerHash("ControllerSpeaker2");
        fn_802F49C0(&binding, &definition, parameter, false, 0.0f);
    }
    {
        int parameter[2];
        parameter[0] = 3;
        parameter[1] = 0;
        u32 binding;
        u32 definition;
        definition = nlStringLowerHash("SendToSpeaker");
        binding = nlStringLowerHash("ControllerSpeaker3");
        fn_802F49C0(&binding, &definition, parameter, false, 0.0f);
    }
    {
        int parameter[2];
        parameter[0] = 4;
        parameter[1] = 0;
        u32 binding;
        u32 definition;
        definition = nlStringLowerHash("SendToSpeaker");
        binding = nlStringLowerHash("ControllerSpeaker4");
        fn_802F49C0(&binding, &definition, parameter, false, 0.0f);
    }

    GameInfoManager::Instance()->unknown_0x71C8 = 0;
    manager->mLoadingState = 1;
    lbl_806E1040->SetVisible(false);
    ResetHallOfFameImagePreload();
    FinishLoadingStep(manager);
}

extern "C" void fn_80119B0C(AsyncLoadingManager* manager)
{
    manager->mLoadingState = 3;
    manager->SetLoadingComment("DestroyFEState");
    g_pFEInput->Reset(true);
    while (!FESceneManager::Instance()->AreAllScenesValid())
    {
        nlServiceFileSystem();
        FESceneManager::Instance()->Update(0.0f);
        FEResourceManager::Instance()->Run(0.0f);
    }

    ImpostorManager::GetInstance()->ResetImpostors();
    ImpostorManager::GetInstance()->Uninitialize();
    FESceneManager::Instance()->ClearTopMostScene();
    GameSceneManager::Instance()->PopEntireStack();
    if (GameSceneManager::s_pInstance != 0)
    {
        delete GameSceneManager::s_pInstance;
        GameSceneManager::s_pInstance = 0;
    }

    if (MiiManager::s_pInstance != 0)
    {
        delete MiiManager::s_pInstance;
        MiiManager::s_pInstance = 0;
    }
    FESceneManager::Instance()->ForceImmediateStackProcessing();
    FEResourceManager::Instance()->UnloadPermanentResourceBundle();
    UnloadFEMiniBundle();
    FEResourceManager::Instance()->Cleanup();
    DestroyFEResourcePool();
    if (FontManager::s_pInstance != 0)
    {
        delete FontManager::s_pInstance;
        FontManager::s_pInstance = 0;
    }
    nlFree(g_pLocalization->m_pFile);
    ParticleUpdateTask::sInstance->Shutdown();
    cCameraManager::Shutdown();
    DestroyFEWorld();
    if (FEModelManager::s_pInstance != 0)
    {
        delete FEModelManager::s_pInstance;
        FEModelManager::s_pInstance = 0;
    }
    FEMusic::StopStream();
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
            PrintAudioSystem(g_pAudioSystem);
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

    fn_80370E64();
    DestroyPadBackends();
    fn_802B467C(&Detail::sTempStringAllocatorPool.allocator.pool);
    SlotPoolBase::BaseFreeBlocks(&Detail::sTempStringAllocatorPool.allocator.pool, 0x40);
    cPN_SAnimController::m_SAnimControllerSlotPool.FreeBlocks();
    cPN_Blender::m_BlenderSlotPool.FreeBlocks();
    cPN_SingleAxisBlender::m_SingleAxisBlenderSlotPool.FreeBlocks();
    cPN_Feather::m_FeatherSlotPool.FreeBlocks();
    cPN_ScaleBlender::m_ScaleBlenderSlotPool.FreeBlocks();
    lbl_805840D8.FreeBlocks();
    if (g_bTweaking)
    {
        fn_802BDA28();
    }
    ResetDynamicTweaks();
    glCompact();
    fn_802B467C(&lbl_8057AB80);
    SlotPoolBase::BaseFreeBlocks(&lbl_8057AB80, 8);

    glGetCurrentResourcePool()->ReleaseResource((unsigned long)manager->mUnidentified4C);
    glDestroyResourcePool(glGetCurrentResourcePool());
    glSetCurrentResourcePool(0);
    fn_80111658(2);
    manager->mLoadingState = 0;
    FinishLoadingStep(manager);
}

extern "C" void fn_80119EC0(AsyncLoadingManager* manager)
{
    manager->mLoadingState = 2;
    manager->SetLoadingComment("InitializeGameState1");
    fn_80111654(1);
    ClearTweakRegistryReset();
    if (GameInfoManager::Instance()->IsInMode3())
    {
        CupManager::Instance()->PrepareCurrentGame();
    }
    GameInfoManager::Instance()->SetupGameFromConfig();
    GLMemoryRequirement requirements[] = {
        { GLM_Header, 0 },
        { GLM_TextureData, 0 },
        { GLM_VertexData, 0 },
    };
    requirements[0].mSize = 0x140000;
    requirements[1].mSize = 0x8B3333;
    requirements[2].mSize = 0x580000;
    glSetCurrentResourcePool(glCreateResourcePool(requirements, 3, "InGame"));
    manager->mUnidentified4C = (void*)glGetCurrentResourcePool()->MarkResource();
    CreatePadBackends();
    NisPlayer::Instance()->fn_8027BD60();
    gDispatchEventsTask->dispatcher.Dispatch(true);
    if (g_pNetworkSessionBase->GetSessionMode() == 0)
    {
        if (gNetworkInputRecording->mPlaybackEnabled)
        {
            PlaybackRecordedGame();
        }
        else
        {
            StartSinglePlayerGame();
        }
    }
    GameInfoManager::Instance()->ApplyDifficultySettings();
    ReplayManager::Instance()->Initialize();
    manager->mUnidentified50 = (void*)glGetCurrentResourcePool()->MarkResource();
    g_pAudioSystem->GetBundleManager()->GetSoundMap()->SelectGroup(0);
    lbl_806E1050 = new (8, false) WorldNPCManager;
    lbl_806E1050->LoadTemplates("ini/WorldNPCs.ini");
    lbl_806E1050->mModelCallback = fn_80183E8C;
    lbl_806E1050->mRenderFilter = fn_80277DD4;
    FinishLoadingStep(manager);
}

extern "C" void fn_8011A0A8(AsyncLoadingManager* manager)
{
    manager->SetLoadingComment("InitializeGameState2");
    fn_80056CF4((void*)gGameTweaks.mTerrainType, gGameTweaks.mUnidentified08, gGameTweaks.mUnidentified0C);
    static_cast<OverlayManager*>(g_pOverlayManager)->fn_801E1514();
    InitializeGameStreams();
    StatsTracker::Instance()->SetBasicGameInfoPointer(GameInfoManager::Instance()->GetCurrentGameInfo(), true);
    StatsTracker::Instance()->CreateEventHandler();
    ReplayManager::Instance()->RegisterEventHandlers();
    fn_80194EF8(&ReplayChoreo::Instance());
    NisPlayer::Instance()->RegisterEventHandlers();
    GetPresentation()->RegisterEventListeners();
    GetPresentation()->fn_80285E1C();
    ExcitementSystem::fn_80196644().fn_80196924();
    fn_8001FE80();
    fn_80018A00();
    UnidentifiedCameraEffects::Instance()->RegisterEventListeners();
    UnidentifiedFindEvent<UnidentifiedEventNoData>("GameOver", -1)->Add(Function<FnVoidVoid>(GoalieOnGameOver), (unsigned int)&manager->mLoadingHandle, -1);
    GLResourcePool* pool = glGetCurrentResourcePool();
    Jumbotron::instance.Initialize(pool);
    CrowdManager::instance.Initialize(pool);
    InitializeWarble(&gWarble);
    InitializeWarbleRendering(&gWarbleEnabled);
    CreateInstance(nlSingleton<TimedObjectManager>::s_pInstance);
    ParticleUpdateTask* particleTask = ParticleUpdateTask::sInstance;
    particleTask->Initialize(GetLayerView((eCLV)0x19), 0x5F6, 0x2FB);
    LoadCrowdCharacterList();
    FinishLoadingStep(manager);
}

extern "C" void fn_8011A2DC(void* value0, unsigned long value1, void*)
{
    lbl_806E1060 = value0;
    lbl_806E1064 = (void*)value1;
}

extern "C" void fn_8011A2E8(AsyncLoadingManager* manager)
{
    manager->SetLoadingComment("AsyncStartGameWorldLoading");
    int stadium = GameInfoManager::Instance()->GetStadium();
    fn_802772D0(GetStadiumName(stadium), false);
    fxSetTerrain(nlStringLowerHash(GetStadiumTerrain(stadium)));
    lbl_806E1058 += gAudioEnabled ? 7 : 0;
    GameAudio* audio = static_cast<GameAudio*>(g_pAudioSystem);
    LoadSoundBank(audio, GetStadiumUnknown0x14(stadium), 0xB, fn_80116988, (void*)"STAD_*");
    LoadSoundBank(static_cast<GameAudio*>(g_pAudioSystem), 0x1F, 0xA, fn_80116988, (void*)"STAD_GEN");
    LoadSoundBank(static_cast<GameAudio*>(g_pAudioSystem), 0x35, 0xC, fn_80116988, (void*)"WEATHER_LIGHTNING");
    LoadSoundBank(static_cast<GameAudio*>(g_pAudioSystem), 0x16, 0xD, fn_80116988, (void*)"CROWD_GEN");
    LoadSoundBank(static_cast<GameAudio*>(g_pAudioSystem), 0x34, 0xE, fn_80116988, (void*)"CROWDStreams");
    LoadSoundBank(static_cast<GameAudio*>(g_pAudioSystem), 0x31, 0x12, fn_80116988, (void*)"MUSICSTREAMS");
    LoadSoundBank(static_cast<GameAudio*>(g_pAudioSystem), 0x33, 0x13, fn_80116988, (void*)"NisStreams");
    {
        int parameter[2];
        parameter[0] = 1;
        parameter[1] = 0;
        u32 binding;
        u32 definition;
        definition = nlStringLowerHash("SendToSpeaker");
        binding = nlStringLowerHash("ControllerSpeaker1");
        fn_802F49C0(&binding, &definition, parameter, false, 0.0f);
    }
    {
        int parameter[2];
        parameter[0] = 2;
        parameter[1] = 0;
        u32 binding;
        u32 definition;
        definition = nlStringLowerHash("SendToSpeaker");
        binding = nlStringLowerHash("ControllerSpeaker2");
        fn_802F49C0(&binding, &definition, parameter, false, 0.0f);
    }
    {
        int parameter[2];
        parameter[0] = 3;
        parameter[1] = 0;
        u32 binding;
        u32 definition;
        definition = nlStringLowerHash("SendToSpeaker");
        binding = nlStringLowerHash("ControllerSpeaker3");
        fn_802F49C0(&binding, &definition, parameter, false, 0.0f);
    }
    {
        int parameter[2];
        parameter[0] = 4;
        parameter[1] = 0;
        u32 binding;
        u32 definition;
        definition = nlStringLowerHash("SendToSpeaker");
        binding = nlStringLowerHash("ControllerSpeaker4");
        fn_802F49C0(&binding, &definition, parameter, false, 0.0f);
    }

    FinishLoadingStep(manager);
}

extern "C" void fn_8011A570(AsyncLoadingManager* manager)
{
    manager->SetLoadingComment("InitializeGameStateInGameFE1");
    g_pOverlayManager->Push((SceneList)0x5A, SCREEN_NOTHING, false);
    g_pOverlayManager->Push(OVERLAY_HUD, SCREEN_NOTHING, false)->SetVisible(false);
    lbl_806E1058 += gAudioEnabled ? 2 : 0;
    LoadSoundBank(static_cast<GameAudio*>(g_pAudioSystem), 0x1C, 0xF, fn_80116988, (void*)"HUD_GEN");
    LoadSoundBank(static_cast<GameAudio*>(g_pAudioSystem), 0x1E, 0x10, fn_80116988, (void*)"HUD_PU");
    FEAudio::SetSoundCategory(0xF);
    FEMusic::SetInGame(true);
    if (GameInfoManager::Instance()->mIsInStrikers101Mode)
    {
        g_pOverlayManager->Push((SceneList)0x61, SCREEN_NOTHING, false);
        Presentation* presentation = GetPresentation();
        presentation->mLetterBoxEnabled = false;
        presentation->mLetterBoxDuration = 0.0f;
    }
    g_pOverlayManager->Push((SceneList)0x5F, SCREEN_NOTHING, false)->SetVisible(false);
    if (GameInfoManager::Instance()->mCurrentMode == GameInfoManager::GM_MODE_2)
    {
        g_pOverlayManager->Push((SceneList)0x60, SCREEN_NOTHING, false);
    }
    g_pOverlayManager->Push((SceneList)0x64, SCREEN_NOTHING, false)->SetVisible(false);
    g_pOverlayManager->Push((SceneList)0x65, SCREEN_NOTHING, false)->SetVisible(false);
    g_pOverlayManager->Push((SceneList)0x66, SCREEN_NOTHING, false)->SetVisible(false);
    BaseSceneHandler* scene = g_pOverlayManager->Push((SceneList)0x1E, SCREEN_NOTHING, false);
    FESceneManager::Instance()->SetTopMostScene(scene);
    SetPointerEnabled(false);
    FinishLoadingStep(manager);
}

extern "C" void fn_8011A800(AsyncLoadingManager* manager)
{
    manager->SetLoadingComment("GameStateFinalize");

    PauseMenuScene::mLastSelectedIndex = 0;
    InitializeElectricFence(GetLayerView(eCLV_ElectricFence));
    BeginFrameTask::s_FramerateLocked = false;
    TakeGameMemSnapshot::ResetTimers();
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
    manager->SetLoadingComment("DestroyGameState");

    nlPrintf("RL memory free: %dK\n", glGetCurrentResourcePool()->GetFreeMemory() >> 10);
    glGetCurrentResourcePool();

    manager->mLoadingState = 3;

    fn_80056EA8();

    if (fn_802BD63C())
    {
        fn_802BD718("FrameTime_GamePlay", "ms",
            pGamePlayTimeRegion->m_fThreshold / (float)pGamePlayTimeRegion->m_unk10);
        fn_802BD718("FrameTime_NIS", "ms",
            pNISTimeRegion->m_fThreshold / (float)pNISTimeRegion->m_unk10);
        fn_802BD718("FrameTime_AutoReplay", "ms",
            pAutoReplayTimeRegion->m_fThreshold / (float)pAutoReplayTimeRegion->m_unk10);
        g_FrameCounter.fn_802B80C4();
    }

    DestroyTimeRegions();
    FESceneManager::Instance()->ClearTopMostScene();
    g_pOverlayManager->PopEntireStack();
    if (g_pOverlayManager != 0)
    {
        delete g_pOverlayManager;
        g_pOverlayManager = 0;
    }
    FESceneManager::Instance()->ForceImmediateStackProcessing();

    FlareHandler::instance.Cleanup();
    NisPlayer::Instance()->StopNisCue();
    NisPlayer::Instance()->Reset();
    NisPlayer::Instance()->ReleaseCachedNisBuffers();
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
    g_pGame->mpWeatherManager->Stop(true);
    ShootToScoreMeter::instance.TurnOffMeter();
    delete g_pBall;
    g_pBall = 0;
    FakeBallWorld::Destroy();
    cCameraManager::Shutdown();
    gNPCManager->DestroyNPCs();
    ResetFlyingCameras();
    gNPCManager->UnloadTransientNPCTemplates();
    ParticleUpdateTask::sInstance->Shutdown();
    DestroyStadium();
    fn_80115FB4();
    GetFixedUpdateTask()->Reset();
    DestroyPhysicsEventQueue();
    DestroyStaticPhysicsPrimitives();
    FrontEnd::Destroy();
    Jumbotron::instance.Uninitialize();
    CrowdManager::instance.Uninitialize();
    ShutdownWarble(&gWarble);
    ShutdownWarbleRendering(&gWarbleEnabled);

    if (nlSingleton<TimedObjectManager>::s_pInstance != 0)
    {
        delete nlSingleton<TimedObjectManager>::s_pInstance;
        nlSingleton<TimedObjectManager>::s_pInstance = 0;
    }

    FreeElectricFence();
    DestroyGame();
    DestroyGameTweaks(&gGameTweaks);
    DestroyPhysicsWorld();

    fn_802B467C(&Detail::sTempStringAllocatorPool.allocator.pool);
    SlotPoolBase::BaseFreeBlocks(&Detail::sTempStringAllocatorPool.allocator.pool, 0x40);

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
            PrintAudioSystem(g_pAudioSystem);
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
    BlurManager::Shutdown();
    FreeImpostorLighting();
    fn_80183E4C();
    gCrowdModelCollection.Clear();
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
    cPN_ScaleBlender::m_ScaleBlenderSlotPool.FreeBlocks();
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
    FreePhysicsEventDataPools();
    fn_80111658(1);

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
    manager->SetLoadingComment("AsyncFEResourceLoadBeing");
    lbl_806E1068 = false;
    lbl_806E1069 = false;
    if (!lbl_806E1044)
    {
        if (g_pLocalization->m_CurrentLanguage == nlLocalization::LangJapanese)
        {
            LoadFEMiniBundle("art/fe/JPBootLoadingUI.res");
        }
        else
        {
            LoadFEMiniBundle("art/fe/BootLoadingUI.res");
        }
    }
    else
    {
        LoadFEMiniBundle("art/fe/LoadingUI.res");
    }
    if (GameSceneManager::s_pInstance == 0)
    {
        CreateInstance(GameSceneManager::s_pInstance);
        if (!lbl_806E1044)
        {
            if (g_pLocalization->m_CurrentLanguage == nlLocalization::LangJapanese)
            {
                lbl_806E1040 = GameSceneManager::Instance()->Push(SCENE_BOOT_LOADING_JPN, SCREEN_NOTHING, false);
            }
            else
            {
                lbl_806E1040 = GameSceneManager::Instance()->Push(SCENE_BOOT_LOADING, SCREEN_NOTHING, false);
            }
        }
        else
        {
            lbl_806E1040 = GameSceneManager::Instance()->Push(SCENE_ASYNC_LOADING, SCREEN_NOTHING, false);
        }
    }
    FEResourceManager::Instance()->LoadPermanentResourceBundle("art/fe/MainUI.Dmn", fn_8011B418);
    lbl_806E1068 = false;
    lbl_806E1069 = true;
    CreatePadBackends();
    EnableAutoPressed();
}

extern "C" void fn_8011B2E4(AsyncLoadingManager* manager)
{
    manager->SetLoadingComment("AsyncFEGameResourceLoadBegin");
    lbl_806E1068 = false;
    lbl_806E1069 = false;
    InitializeFunctionMemory();
    PushFunctionMemoryState();
    PushEventConnectionState();
    if (g_pOverlayManager == 0)
    {
        g_pOverlayManager = new (8, false) OverlayManager;
    }
    if (g_e3_Build)
    {
        LoadFEMiniBundle("art/fe/LoadingUIE3.res");
        lbl_806E1040 = g_pOverlayManager->Push(SCENE_WIDESCREEN_LOADING, SCREEN_NOTHING, false);
    }
    else
    {
        LoadFEMiniBundle("art/fe/GameLoadingUI.res");
        lbl_806E1040 = g_pOverlayManager->Push(SCENE_WIDESCREEN_LOADING, SCREEN_NOTHING, false);
    }
    FESceneManager::Instance()->SetTopMostScene(lbl_806E1040);
    FEResourceManager::Instance()->m_bPermanentBundleLoadInProgress = true;
    FEResourceManager::Instance()->LoadPermanentResourceBundle("art/fe/InGameUI.Res", fn_8011B418);
    FEResourceManager::Instance()->OpenOnDemandResourceBundle("art/fe/InGameUI.Dmn", fn_8011B424);
}

extern "C" void fn_8011B40C(AudioResourceLoadOwner*, void*)
{
    lbl_806E106A = true;
}

extern "C" void fn_8011B418()
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
        const char* name = GetCupPersonaTrophyNames()[cupPersona];
        nlSNPrintf(trophyName, sizeof(trophyName), "Trophy%s", name);
        gNPCManager->CreateNPCTemplate(trophyName, false);
    }
    else if (GameInfoManager::Instance()->IsInMode3())
    {
        int cup = CupManager::Instance()->GetCurrentMode();
        const char* name = GetCupTrophyNames()[cup];
        nlSNPrintf(trophyName, sizeof(trophyName), "Trophy%s", name);
        gNPCManager->CreateNPCTemplate(trophyName, false);
    }
}

extern "C" void fn_8011B6E8(AsyncLoadingManager* manager)
{
    manager->mLoadingState = 2;
    manager->SetLoadingComment("InitializeStadiumViewer");
    fn_80111654(1);
    ClearTweakRegistryReset();
    GLMemoryRequirement requirements[] = {
        { GLM_Header, 0x200000 },
        { GLM_TextureData, 0xB00000 },
    };
    glSetCurrentResourcePool(glCreateResourcePool(requirements, 2, "StadiumViewer"));
    manager->mUnidentified4C = (void*)glGetCurrentResourcePool()->MarkResource();
    CreatePadBackends();
    NisPlayer::Instance()->fn_8027BD60();
    GameInfoManager::Instance()->SetMode(0, 0);
    GameInfoManager::Instance()->mCurrentDifficulty[0] = 2;
    GameInfoManager::Instance()->mCurrentDifficulty[1] = 2;
    GameInfoManager::Instance()->SetupGameFromConfig();
    ReplayManager::Instance()->Initialize();
    CreateInstance(nlSingleton<TimedObjectManager>::s_pInstance);
    lbl_806E1050 = new (8, false) WorldNPCManager;
    manager->mUnidentified50 = (void*)glGetCurrentResourcePool()->MarkResource();
    FinishLoadingStep(manager);
}

GLResourcePool* AsyncLoadingManager::GetPersistentResourcePool()
{
    return sPersistentResourcePool;
}

UnidentifiedOwnerConnection::~UnidentifiedOwnerConnection()
{
    if (mOwner != 0 && ((mOwner->mFlags >> 30) & 1) != 0)
    {
        mOwner->mTarget->Release(this);
    }
}

PersistentResourceRequirements gPersistentResourceRequirements = {
    { { GLM_Header, 0x3C00 }, { GLM_TextureData, 0x390800 } },
};
