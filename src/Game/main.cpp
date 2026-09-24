#include "Game/main.h"
#include "Game/TweakRegistry.h"
#include "Game/TweakConfig.h"

#include "Game/Task/FixedUpdateTask.h"

#include "Game/Task/BeginFrameTask.h"
#include "Game/Task/ComUpdateTask.h"
#include "Game/DB/StadiumInfo.h"
#include "Game/DB/GameProgress.h"
#include "Game/DB/StatsTracker.h"
#include "Game/Debug/FrameCounter.h"
#include "Game/Debug/ShapeRender.h"
#include "Game/Effects/EmissionManager.h"
#include "Game/Effects/ParticleSystem.h"
#include "Game/GL/GLTexturedColourMeshWriter.h"
#include "Game/Task/FrontEndTask.h"
#include "Game/FE/feMusic.h"
#include "Game/FE/feInput.h"
#include "Game/FE/feResourceManager.h"
#include "Game/FE/feSceneManager.h"
#include "Game/FE/LidOpenMessage.h"
#include "Game/FE/feHelpFuncs_decl.h"
#include "Game/Task/ResetTask.h"
#include "Game/Render/Wiper.h"
#include "Game/Render/ImpostorManager.h"
#include "Game/Render/Presentation.h"
#include "Game/Render/FrontEndPresentation.h"
#include "Game/Render/RenderShadow.h"
#include "Game/Render/RLViewLayers.h"
#include "Game/Render/depthoffield.h"
#include "Game/Render/FlareHandler.h"
#include "Game/Sys/clock.h"
#include "Game/Sys/debug.h"
#include "Game/GameInfo.h"
#include "Game/NisPlayer.h"
#include "Game/ReplayManager.h"
#include "Game/ReplayChoreo.h"
#include "Game/ExcitementSystem.h"
#include "Game/Transitions/ModelTransition.h"
#include "Game/Sys/audio.h"
#include "Game/Sys/simpleparser.h"
#include "Game/Task/DispatchEventsTask.h"
#include "Game/Task/EndFrameTask.h"
#include "Game/Task/GameRenderTask.h"
#include "Game/Task/LoadingTask.h"
#include "Game/Task/MovieRenderTask.h"
#include "Game/Task/NetworkUpdateTask.h"
#include "Game/Task/ParticleUpdateCallbacks.h"
#include "Game/Task/ParticleUpdateTask.h"
#include "Game/Task/PlatPadUpdateTask.h"
#include "Game/Task/ProfilerTask.h"
#include "Game/Task/TextWindowTask.h"
#include "Game/Task/TransitionTask.h"
#include "Game/Task/TweakerTask.h"
#include "Game/Task/WorldUpdateTask.h"
#include "Game/TweakValue.h"
#include "Game/PadActions.h"
#include "Game/Pad/FlickDetection.h"

#include "NL/MemAlloc.h"
#include "NL/gl/gl.h"
#include "NL/gl/glMemory.h"
#include "NL/gl/glMemoryInit.h"
#include "NL/globalpad.h"
#include "NL/plat/WiiPad.h"
#include "NL/nlBind.h"
#include "NL/nlConfig.h"
#include "NL/nlDebug.h"
#include "NL/nlFile.h"
#include "NL/nlFileGC.h"
#include "NL/nlFunction.h"
#include "NL/nlFunction.inl"
#include "NL/nlMemory.h"
#include "NL/nlDebugViews.h"
#include "NL/nlMain.h"
#include "NL/nlMath.h"
#include "NL/nlString.h"
#include "NL/nlTask.h"
#include "NL/gl/glState.h"
#include "NL/gl/tu_802A12E4.h"
#include "Game/FE/feDPD.h"
#include "NL/plat/nlFlash.h"
#include "NL/plat/nlFileCache.h"
#include "NL/plat/SwappablePad.h"
#include "NL/glx/glxSwap.h"

#include <revolution/os/OSTime_fwd.h>

#include <string.h>
#include "NL/nlstring_tmpl.h"
#include "NL/gl/glPlat.h"
#include "Game/Audio/RegistryPools.h"
#include "Game/UnidentifiedStaticStorage.h"

class AudioUpdateTask : public nlTask
{
public:
    virtual void Run(float dt);
    virtual const char* GetName() { return "Audio"; }
};

class UnidentifiedMemCheckTask : public nlTask
{
public:
    UnidentifiedMemCheckTask()
        : mAccumulatedDelta(0)
        , mSampleCount(0)
    {
    }

    virtual void Run(float dt);
    virtual const char* GetName() { return "Mem Check"; }

private:
    u32 mAccumulatedDelta;
    u32 mSampleCount;
}; // size 0x28

extern "C"
{
    u32 SCGetSimpleAddressID();
    u8 SCGetLanguage();
    u8 SCGetAspectRatio();
    void fn_8013D7A0();
    void fn_8013D7E0();
    void OSYieldThread();
    void StartupAIPads();
    void fn_80184ADC();
}

void nlRegHandleDVDMessageCB(const Function<void(int)>&);
void nlRegHandleDVDAllClearCB(const Function<void(int)>&);
void nlRegHandleDVDRetryingCB(const Function<void(int)>&);
void nlRegCheckForResetFromFSCB(const Function<FnVoidVoid>&);

extern bool g_bDisableWriteOut;
extern void* lbl_806E1C20;
extern u8 lbl_806E1458;

extern int lbl_806DF2E0;
extern int lbl_806DF2E4;
extern int lbl_806DF2E8;
extern int lbl_806DF2EC;
extern int lbl_806DF2F0;
extern int lbl_806DF2F4;
extern int lbl_806DF2F8;
extern int lbl_806DF2FC;


volatile int g_Region = 3;
static u32 sPreviousTaskState = 1;

static GLMemoryRequirement sGlobalResourceRequirements[3] = {
    { GLM_TextureData, 0 },
    { GLM_VertexData, 0 },
    { GLM_Header, 0 },
};

static u32 sCountryCode;
GameAudio* g_pGameAudio;
bool g_e3_Build;
bool lbl_806E1091;
nlLocalization::nlLanguage g_Language;
static u32 sLastVirtualFreeDelta;
static float sAverageVirtualFreeDelta;
static float sVirtualFreeMiB;
static float sVirtualLargestFreeMiB;
static float sStandardFreeMiB;
static float sStandardLargestFreeMiB;
static float sVirtualUsedMiB;
static float sStandardUsedMiB;
static float sSubsystemFreeMiB;
static float sSubsystemUsedMiB;
static float sSubsystemLargestFreeMiB;
static float sUnidentifiedMemoryMetric0;
static float sUnidentifiedMemoryMetric1;
static float sUnidentifiedMemoryMetric2;
int g_BuildNumber;
static u32 sPreviousVirtualFree;
static bool sDateTimeLoaded;
static u32 sWarbleTexture;
static bool sWarbleTextureCached;

FrameCounter g_FrameCounter("frame", "send");

static TweakBoolBinding sDisableWriteOutTweak(
    "g_bDisableWriteOut", "/General", &g_bDisableWriteOut, false);
static TweakBoolBinding sMemoryLowWaterMarkCheckingTweak(
    "g_bActivateMemoryLowWaterMarkChecking", "/General",
    &g_bActivateMemoryLowWaterMarkChecking, false);
static TweakBoolBinding sPrintMemoryLowWaterMarksTweak(
    "g_bPrintMemoryNewLowWaterMarks", "/General",
    &g_bPrintMemoryNewLowWaterMarks, false);

static ComUpdateTask comUpdateTask;
static UnidentifiedPingerUpdateTask pingerUpdateTask;
static NetworkUpdateTask networkUpdateTask;
static PlatPadUpdateTask platPadUpdateTask;
static FrontEndTask frontEndTask;
static WorldUpdateTask worldUpdateTask;
static GameRenderTask gameRenderTask;
static UnidentifiedMovieRenderTask movieRenderTask;
static ParticleUpdateTask particleUpdateTask;
static BeginFrameTask beginFrameTask;
static AudioUpdateTask audioUpdateTask;
static EndFrameTask endFrameTask;
static TweakerTask tweakerTask;
static ProfilerTask profilerTask;
static ResetTask resetTask;
static UnidentifiedMemCheckTask memCheckTask;
static TextWindowTask textWindowTask;
static FEDPDTask feDPDTask;
static FlashMemoryTask flashMemoryTask;

static TweakValueBool sAllowWarble(
    "sbAllowWarble", "/Rendering/Effects/Warble", true);
static TweakValueBool sRenderWarbleToParticleView(
    "sbRenderWarbleToParticleView", "/Rendering/Effects/Warble", false);
static TweakValueBool sUseCheckerTextureForWarble(
    "sbUseCheckerTextureForWarble", "/Rendering/Effects/Warble", false);

static void PreInitFS();
static void Initialize();
static void AddTasks();
extern "C" bool fn_8011D1BC(ParticleSystem*, GLView*,
    nlDLListSlotPool<Particle*>*, const nlVector3&, const nlVector3&,
    const nlMatrix4*);
extern "C" void fn_8011D3CC(GLTexturedColourMeshWriter*, ParticleSystem*,
    nlDLListSlotPool<Particle*>*, const nlVector3&, const nlVector3&,
    const nlMatrix4*);
extern "C" void fn_8011D5B0(State_802A12E4*, ParticleSystem*,
    nlDLListSlotPool<Particle*>*, const nlVector3&, const nlVector3&,
    const nlMatrix4*);

int GetRegion()
{
    int region = 0;
    if (g_Region != 3)
    {
        region = g_Region;
    }
    return region;
}

int GetOnlineRegion()
{
    int region = 0;
    if (g_Region != 3)
    {
        region = g_Region;
    }
    return region + 1;
}

bool IsAlternateOnlineCountryGroup()
{
    int country;
    if (sCountryCode != 0)
    {
        country = sCountryCode;
    }
    else
    {
        country = SCGetSimpleAddressID();
        country &= 0xFF000000;
        if (country == 0 || country == 0xFF000000)
        {
            country = 0;
        }
        else
        {
            country = (u32)country >> 24;
        }
    }

    if (country == 'A' || country == '_')
    {
        return true;
    }
    return false;
}

void UnidentifiedMemCheckTask::Run(float)
{
    const float bytesPerMiB = 1048576.0f;
    const u32 currentVirtualFree = VirtualAllocator.m_04;

    sLastVirtualFreeDelta = currentVirtualFree - sPreviousVirtualFree;
    sPreviousVirtualFree = currentVirtualFree;
    sVirtualFreeMiB = VirtualAllocator.TotalFreeMemory() / bytesPerMiB;
    sVirtualLargestFreeMiB =
        VirtualAllocator.LargestFreeBlock() / bytesPerMiB;
    sStandardFreeMiB = StandardAllocator.TotalFreeMemory() / bytesPerMiB;
    sStandardLargestFreeMiB =
        StandardAllocator.LargestFreeBlock() / bytesPerMiB;

    MemoryAllocator* subsystemAllocator =
        reinterpret_cast<MemoryAllocator*>(
            reinterpret_cast<u8*>(lbl_806E1C20) + 0x434);
    sSubsystemFreeMiB = subsystemAllocator->TotalFreeMemory() / bytesPerMiB;
    sSubsystemLargestFreeMiB =
        subsystemAllocator->LargestFreeBlock() / bytesPerMiB;

    sVirtualUsedMiB =
        (VirtualAllocator.m_14 - VirtualAllocator.m_10) / bytesPerMiB;
    sStandardUsedMiB =
        (StandardAllocator.m_14 - StandardAllocator.m_10) / bytesPerMiB;
    sSubsystemUsedMiB =
        (subsystemAllocator->m_14 - subsystemAllocator->m_10) / bytesPerMiB;

    if (nlTaskManager::m_pInstance->mCurrentState == 2 &&
        sPreviousTaskState == 2 && !g_bTweaking)
    {
        mAccumulatedDelta += sLastVirtualFreeDelta;
        ++mSampleCount;
        sAverageVirtualFreeDelta =
            static_cast<float>(mAccumulatedDelta / mSampleCount);
    }

    sPreviousTaskState = nlTaskManager::m_pInstance->mCurrentState;
}

static void PreInitFS()
{
    GLMemoryConfig config;
    config.mFrameMemSize1 = 0x80000;
    config.mFrameMemSize2 = 0x233333;
    config.mResourceRequirements = sGlobalResourceRequirements;
    config.mNumResourceRequirements = 3;
    config.mMaxTextures = 1000;
    if (!glInitMemory(&config))
    {
        nlBreak();
    }
}

extern "C" void fn_8011C4E8(int)
{
    const u32 state = nlTaskManager::m_pInstance->mCurrentState;
    if (state == 4 || state == 1)
    {
        EnableAutoPressed();
    }
}

void ConfigureTweakerButtons(int)
{
    cGlobalPad* pad = 0;
    for (int i = 0; i < 4; ++i)
    {
        pad = g_pPadManager->GetPad(i);
        if (pad->IsConnected())
        {
            break;
        }
    }

    const int classID = pad->mBackend->GetClassID();
    const bool tweakerEnabled =
        GetTweakBool("/user/Tweaker with just Z Button", false);
    const bool isWiiRemote =
        classID == gWiiRemotePadClassID || classID == gWiiFreestylePadClassID;

    lbl_806DF2E0 = 9;
    lbl_806DF2E4 = 10;
    lbl_806DF2E8 = 8;
    lbl_806DF2F0 = 11;
    lbl_806DF2F4 = 12;
    lbl_806DF2F8 = 13;
    lbl_806DF2FC = 14;
    if (!tweakerEnabled && !isWiiRemote)
    {
        lbl_806DF2EC = 23;
    }
    else
    {
        lbl_806DF2EC = -1;
    }
}

extern "C" void fn_8011C610(const char* buildInfo)
{
    char* copy;
    const u32 length = nlStrLen(buildInfo) + 1;
    copy = static_cast<char*>(nlMalloc(length, 8, false));
    memcpy(copy, buildInfo, nlStrLen(buildInfo) + 1);

    SimpleParser parser;
    parser.StartParsing(copy, length, " ");
    parser.NextToken(false);
    const char* buildNumber = parser.NextToken(false);
    g_BuildNumber = (int)atof(buildNumber);
    nlFree(copy);
}

extern "C" void fn_8011C70C(
    void* data, unsigned long size, void* destination)
{
    void* destinationCopy = destination;
    unsigned long sizeCopy = size;
    void* dataCopy = data;
    LoadTweakConfigBuffer(destinationCopy, static_cast<char*>(dataCopy), sizeCopy,
        static_cast<const char*>(destinationCopy));
    sDateTimeLoaded = true;
}

class Config;

extern "C" void fn_8011C748(Config*)
{
}

static void Initialize()
{
    // GQR setup follows the platform OSInitFastCast implementation.
    // clang-format off
    asm {
        li r3, 4
        oris r3, r3, 4
        mtspr 0x392, r3
        li r3, 5
        oris r3, r3, 5
        mtspr 0x393, r3
        li r3, 6
        oris r3, r3, 6
        mtspr 0x394, r3
        li r3, 7
        oris r3, r3, 7
        mtspr 0x395, r3
    }
    // clang-format on

    switch (GetRegion())
    {
    case 0:
        switch (SCGetLanguage())
        {
        case 3:
            g_Language = nlLocalization::LangNAFrench;
            break;
        case 4:
            g_Language = nlLocalization::LangNASpanish;
            break;
        default:
            g_Language = nlLocalization::LangEnglish;
            break;
        }
        break;
    case 1:
        switch (SCGetLanguage())
        {
        case 2:
            g_Language = nlLocalization::LangGerman;
            break;
        case 3:
            g_Language = nlLocalization::LangFrench;
            break;
        case 4:
            g_Language = nlLocalization::LangSpanish;
            break;
        case 5:
            g_Language = nlLocalization::LangItalian;
            break;
        default:
            g_Language = nlLocalization::LangUKEnglish;
            break;
        }
        break;
    case 2:
        g_Language = nlLocalization::LangJapanese;
        break;
    }

    nlInit();

    if (!glStartup(PreInitFS))
    {
        nlBreak();
    }

    nlRegHandleDVDMessageCB(Function<void(int)>(DisplayDVDMessageSebring));
    nlRegHandleDVDRetryingCB(Function<void(int)>(DisplayDVDMessageSebring));
    nlRegHandleDVDAllClearCB(Function<void(int)>(DVDAllClearSebring));
    nlRegCheckForResetFromFSCB(Function<FnVoidVoid>(
        Bind<void>(MemFun<ResetTask, void>(&ResetTask::FSCheckForReset),
            &resetTask)));

    DisplayLoadingMessageFast();
    fn_8013D7A0();
    fn_8013D7E0();
    InitPads();

    unsigned int stringSizes[4];
    stringSizes[1] = 0;
    stringSizes[3] = 0;
    stringSizes[2] = 0x3000;
    stringSizes[0] = 0xA000;
    gResetTweakValueStrings = true;
    InitializeTweakRegistry(0, 1, stringSizes);
    glplatInitializeMaterialPrograms();
    nlSetRandomSeed(OSGetTick(), &nlDefaultSeed);
    if (!glLoadTextureBundle("art/global.rlt", glGetCurrentResourcePool()))
    {
        nlBreak();
    }

    u8 temporaryState = lbl_806E1458;
    ParticleUpdateNoOp(&temporaryState);

    sDateTimeLoaded = false;
    Config::Global().LoadFromFileAsync(
        "ini/common.ini", Function<Config*>(fn_8011C748));
    nlLoadEntireFileAsync("ini/datetime.ini", fn_8011C70C,
        const_cast<char*>("/General/Build Info"), 0x20, AllocateStart, 0, 0,
        0);
    while (!sDateTimeLoaded)
    {
        nlServiceFileSystem();
        OSYieldThread();
    }

    EnableAllStadiums();
    const char* buildInfo =
        GetTweakString("/General/Build Info/BuildNumber", 0);
    if (buildInfo != 0)
    {
        fn_8011C610(buildInfo);
    }

    glxSetDrawSyncTimeout(1000.0f);
    fn_80115F10();
    nlTaskManager::Startup(0x10000);
    sLoadingTask.Start();
    GetEmissionManager();
    EmissionManager::SetResourceBudget(1, 250);
    EmissionManager::ConfigureResource(3, "Character", 250);
    EmissionManager::ConfigureResource(2, "StadiumEffects", 250);
    ImpostorManager::GetInstance();
    ClockManager::Initialize();
    InstallImageRenderCallback();
    GLResourcePool* resourcePool = glGetCurrentResourcePool();
    resourcePool->MarkResource();
    InitPlatPad();
    g_pGameAudio = new (nlMalloc(sizeof(GameAudio), 8, false))
        GameAudio;
    g_pGameAudio->Initialize();
    gSwappablePadChanged.Add(Function<void(int)>(ConfigureTweakerButtons), 0, -1);
    g_pPadManager->Update(0.0f);
    FEInput::Initialize();
    gSwappablePadChanged.Add(Function<void(int)>(fn_8011C4E8), 0, -1);
    FlickDetection::Initialize();
    networkUpdateTask.Initialize();
    StartupAIPads();
    gTransitionTask.Initialize();
    nlLocalization::Initialize();

    if (FEResourceManager::s_pInstance == 0)
    {
        FEResourceManager::s_pInstance = new (8, false) FEResourceManager;
    }
    FEResourceManager::s_pInstance->SetResourcePool(0);
    if (FESceneManager::s_pInstance == 0)
    {
        FESceneManager::s_pInstance = new (8, false) FESceneManager;
    }

    int countryCode;
    if (sCountryCode != 0)
    {
        countryCode = sCountryCode;
    }
    else
    {
        countryCode = SCGetSimpleAddressID();
        countryCode &= 0xFF000000;
        if (countryCode == 0 || countryCode == 0xFF000000)
        {
            countryCode = 0;
        }
        else
        {
            countryCode = (u32)countryCode >> 24;
        }
    }
    tDebugPrintManager::Print(DC_NETWORK, "CountryCode = %d\n", countryCode);

    if (GameInfoManager::s_pInstance == 0)
    {
        GameInfoManager::s_pInstance = new (8, false) GameInfoManager;
    }
    if (g_pCupManager == 0)
    {
        g_pCupManager = new (8, false) CupManager;
    }
    if (StatsTracker::s_pInstance == 0)
    {
        StatsTracker::s_pInstance = new (8, false) StatsTracker;
    }
    if (g_pStrikerChallenge == 0)
    {
        g_pStrikerChallenge = new (8, false) StrikerChallenge;
    }

    nlFlashInitialize();
    nlInitFileCache();
    ReplayChoreo::Instance();
    GetPresentation();
    FrontEndPresentation::GetInstance();
    ExcitementSystem::fn_80196644();
    AddTasks();
    SetupViews();
    fn_80273A30(eCLV_ScreenBlur);
    fn_80273A30(eCLV_ScreenBlur2);
    fn_80273A30(eCLV_ShadowVolume);
    fn_80273A30(eCLV_ShadowVolumeBlend);
    ParticleSystem::m_Callback = fn_8011D1BC;
    ModeledScreenTransition::s_3DView = GetLayerView(eCLV_Transitions3D);
    SetDebugFontView(GetLayerView(eCLV_Debug));
    SetDebugSquareView(GetLayerView(eCLV_DebugSquare));
    bool widescreen = true;
    if (SCGetAspectRatio() != 1)
    {
        widescreen = false;
    }
    rlSetWidescreen(widescreen);
    g_ShapeRenderer.Initialize(resourcePool);
    g_ShapeRenderer.m_eView = GetLayerView(eCLV_Characters);
    fn_80184ADC();
    SetCharacterShadowView(GetLayerView(eCLV_Characters));
    FESceneManager::s_pInstance->m_uDefaultRenderView =
        (unsigned long)GetLayerView(eCLV_Anark);
    NisPlayer::Instance();
    ReplayManager::Instance();
    Wiper::Instance().Initialize();
    DepthOfFieldManager::instance.Initialize();
    FlareHandler::instance.Initialize(GetLayerView(eCLV_Particles));
    BeginFrameTask::s_GameplaySkin = eModelSkin_Both;
    BeginFrameTask::s_ReplaySkin = eModelSkin_Blend;
    InitializeParticleUpdateCallbacks();
    Detail::sTempStringAllocatorPool.allocator.pool.PushState();
}

static void AddTasks()
{
    nlTaskManager::AddTask(&resetTask, 0, (u32)-1);
    nlTaskManager::AddTask(&beginFrameTask, 4, (u32)-1);
    nlTaskManager::AddTask(&sLoadingTask, 2, 0x01F80000);
    nlTaskManager::AddTask(gDispatchEventsTask, 0x18, 0xFE07FFFF);
    nlTaskManager::AddTask(&platPadUpdateTask, 5, (u32)-1);
    nlTaskManager::AddTask(GetFixedUpdateTask(), 8, 0xFE07FFDF);
    nlTaskManager::AddTask(&worldUpdateTask, 9, 0x0002001B);
    nlTaskManager::AddTask(&gameRenderTask, 11, 0x0002001B);
    nlTaskManager::AddTask(&movieRenderTask, 11, (u32)-1);
    nlTaskManager::AddTask(&frontEndTask, 13, (u32)-1);
    nlTaskManager::AddTask(&particleUpdateTask, 12, 0x0002001F);
    nlTaskManager::AddTask(&audioUpdateTask, 15, (u32)-1);
    nlTaskManager::AddTask(&endFrameTask, 16, (u32)-1);
    nlTaskManager::AddTask(&gTransitionTask, 1, (u32)-1);
    nlTaskManager::AddTask(&networkUpdateTask, 17, (u32)-1);
    nlTaskManager::AddTask(&feDPDTask, 13, 5);
    nlTaskManager::AddTask(
        &flashMemoryTask, 13, (u32)-1);
    nlTaskManager::AddTask(nlGetFileCache(), 3, (u32)-1);
    nlTaskManager::AddTask(&Wiper::Instance(), 13, (u32)-1);
}

extern "C" bool fn_8011D1BC(ParticleSystem* source, GLView*,
    nlDLListSlotPool<Particle*>* vertices, const nlVector3& viewRight,
    const nlVector3& viewUp, const nlMatrix4* pCoordSys)
{
    if (!sWarbleTextureCached)
    {
        sWarbleTexture = glGetTexture("effects/fx_warble");
        sWarbleTextureCached = true;
    }

    if (!sAllowWarble)
    {
        return true;
    }

    glGetTexture(sUseCheckerTextureForWarble
            ? "global/checkers"
            : "target/warbletexture");

    if (sRenderWarbleToParticleView)
    {
        GLTexturedColourMeshWriter writer;
        fn_8011D3CC(&writer, source, vertices, viewRight, viewUp, pCoordSys);
    }
    else
    {
        State_802A12E4 writer;
        fn_8011D5B0(&writer, source, vertices, viewRight, viewUp, pCoordSys);
    }
    return true;
}

extern "C" void fn_8011D3CC(GLTexturedColourMeshWriter* writer,
    ParticleSystem* source, nlDLListSlotPool<Particle*>* vertices,
    const nlVector3& viewRight, const nlVector3& viewUp,
    const nlMatrix4* pCoordSys)
{
    ParticleReturn ret;
    if (writer->Begin(source->mUnidentified0BC * 4, GLP_QuadList, 0))
    {
        nlDLListIterator<Particle*> iterator = vertices->Begin();
        while (iterator.hasNext())
        {
            Particle* pPart = *iterator;
            source->UpdateParticle(&ret, pPart, source->m_pTemplate,
                viewRight, viewUp, pCoordSys);
            for (int i = 0; i < 4; ++i)
            {
                writer->Texcoord(ret.texcoord[i]);
                writer->Colour(ret.c);
                writer->Vertex(ret.position[i]);
            }
            iterator.Step();
        }

        glTextureBinding* textureState =
            static_cast<glTextureBinding*>(
                writer->GetModel()->packets->materialParameters);
        textureState->textureIndex = source->mUnidentified09C;
        textureState->SetWrapS(false);
        textureState->SetWrapT(false);
        textureState->unknown07 = 0;
    }
}

extern "C" void fn_8011D5B0(State_802A12E4* writer,
    ParticleSystem* source, nlDLListSlotPool<Particle*>* vertices,
    const nlVector3& viewRight, const nlVector3& viewUp,
    const nlMatrix4* pCoordSys)
{
    ParticleReturn ret;
    if (writer->fn_802A1344(source->mUnidentified0BC * 4, 3, 0))
    {
        nlDLListIterator<Particle*> iterator = vertices->Begin();
        while (iterator.hasNext())
        {
            Particle* pPart = *iterator;
            source->UpdateParticle(&ret, pPart, source->m_pTemplate,
                viewRight, viewUp, pCoordSys);
            for (int i = 0; i < 4; ++i)
            {
                writer->Texcoord(ret.texcoord[i]);
                writer->Colour(ret.c);
                writer->Vertex(ret.position[i]);
            }
            iterator.Step();
        }

        glTextureBinding* textureState =
            static_cast<glTextureBinding*>(
                writer->GetModel()->packets->materialParameters);
        textureState->textureIndex = source->mUnidentified09C;
        textureState->SetWrapS(false);
        textureState->SetWrapT(false);
        textureState->unknown07 = 0;
    }
}

void AudioUpdateTask::Run(float dt)
{
    static_cast<GameAudio*>(g_pAudioSystem)->Update(dt);
}

int main()
{
    Initialize();

    while (nlAsyncReadsPending(0))
    {
        nlServiceFileSystem();
    }

    nlTaskManager::SetNextState(0x00100000);
    FEMusic::SetEnabled(true);

    for (;;)
    {
        nlTaskManager::RunAllTasks();
    }
}
