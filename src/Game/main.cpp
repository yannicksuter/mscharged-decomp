#include "Game/main.h"
#include "Game/TweakRegistry.h"
#include "Game/TweakConfig.h"

#include "Game/Task/FixedUpdateTask.h"

#include "Game/Task/BeginFrameTask.h"
#include "Game/Task/ComUpdateTask.h"
#include "Game/DB/StadiumInfo.h"
#include "Game/Debug/FrameCounter.h"
#include "Game/Effects/EmissionManager.h"
#include "Game/Task/FrontEndTask.h"
#include "Game/FE/feMusic.h"
#include "Game/FE/feHelpFuncs_decl.h"
#include "Game/Task/ResetTask.h"
#include "Game/Render/Wiper.h"
#include "Game/Sys/audio.h"
#include "Game/Sys/simpleparser.h"
#include "Game/Task/DispatchEventsTask.h"
#include "Game/Task/EndFrameTask.h"
#include "Game/Task/GameRenderTask.h"
#include "Game/Task/LoadingTask.h"
#include "Game/Task/MovieRenderTask.h"
#include "Game/Task/NetworkUpdateTask.h"
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
#include "NL/nlString.h"
#include "NL/nlTask.h"
#include "NL/gl/glState.h"
#include "NL/gl/tu_802A12E4.h"
#include "Game/FE/feDPD.h"
#include "NL/plat/nlFlash.h"
#include "NL/plat/tu_80372B4C.h"

#include <string.h>
#include "NL/nlstring_tmpl.h"
#include "NL/gl/glPlat.h"
#include "Game/Audio/UnidentifiedRegistryPools.h"
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

struct UnidentifiedWarbleSource
{
    u8 mPadding[0xBC];
    u32 mVertexCount;
};

struct UnidentifiedWarbleVertex
{
    UnidentifiedWarbleVertex* next;
    UnidentifiedWarbleVertex* prev;
    void* source;
};

struct UnidentifiedWarbleVertexList
{
    u8 mPadding[0x18];
    UnidentifiedWarbleVertex* first;
};

extern "C"
{
    u32 SCGetSimpleAddressID();
    void fn_801BF87C(int);
    void fn_801BFA84(int);
    void fn_802E22D8(void*, float*, void*, void*, int, int, int);
    void fn_802A8278(void*, int, int, void*);

    void fn_801BFB08();
    void fn_8013D7A0();
    void fn_8013D7E0();
    void fn_80272AB4();
    void fn_80184858();
    void OSYieldThread();
}

void nlRegHandleDVDMessageCB(const Function<void(int)>&);
void nlRegHandleDVDAllClearCB(const Function<void(int)>&);
void nlRegHandleDVDRetryingCB(const Function<void(int)>&);
void nlRegCheckForResetFromFSCB(const Function<FnVoidVoid>&);

extern bool g_bDisableWriteOut;
extern void* lbl_806E1C20;

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
extern "C" void fn_8011D3CC(void*, void*,
    UnidentifiedWarbleVertexList*, int, int, int);
extern "C" void fn_8011D5B0(void*, void*,
    UnidentifiedWarbleVertexList*, int, int, int);

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

extern "C" void fn_8011C4E8()
{
    const u32 state = nlTaskManager::m_pInstance->mCurrentState;
    if (state == 4 || state == 1)
    {
        EnableAutoPressed();
    }
}

void ConfigureTweakerButtons()
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
    fn_801BFB08();
    fn_8013D7A0();
    fn_8013D7E0();
    InitPads();

    if (!glStartup(PreInitFS))
    {
        nlBreak();
    }

    nlRegHandleDVDMessageCB(Function<void(int)>(fn_801BF87C));
    nlRegHandleDVDRetryingCB(Function<void(int)>(fn_801BF87C));
    nlRegHandleDVDAllClearCB(Function<void(int)>(fn_801BFA84));
    nlRegCheckForResetFromFSCB(Function<FnVoidVoid>(
        Bind<void>(MemFun<ResetTask, void>(&ResetTask::FSCheckForReset),
            &resetTask)));

    unsigned int stringSizes[4];
    stringSizes[0] = 0xA000;
    stringSizes[1] = 0;
    stringSizes[2] = 0x3000;
    stringSizes[3] = 0;
    InitializeTweakRegistry(0, 1, stringSizes);
    glplatInitializeMaterialPrograms();
    if (!glLoadTextureBundle("art/global.rlt", glGetCurrentResourcePool()))
    {
        nlBreak();
    }

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

    GetEmissionManager();
    EmissionManager::SetResourceBudget(1, 250);
    EmissionManager::ConfigureResource(3, "Character", 250);
    EmissionManager::ConfigureResource(2, "StadiumEffects", 250);

    nlTaskManager::Startup(0x10000);
    sLoadingTask.Start();
    InstallImageRenderCallback();
    g_pGameAudio = new (nlMalloc(sizeof(GameAudio), 8, false))
        GameAudio;
    g_pGameAudio->Initialize();
    FlickDetection::Initialize();
    networkUpdateTask.Initialize();
    gTransitionTask.Initialize();
    nlLocalization::Initialize();

    AddTasks();
    nlFlashInitialize();
    fn_803730D8();
    fn_80272AB4();
    Wiper::Instance().Initialize();
    fn_80184858();
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
    nlTaskManager::AddTask(fn_803733D4(), 3, (u32)-1);
    nlTaskManager::AddTask(&Wiper::Instance(), 13, (u32)-1);
}

extern "C" bool fn_8011D1BC(void* source, void*,
    UnidentifiedWarbleVertexList* vertices, int parameter0, int parameter1,
    int parameter2)
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

    u8 writer[0x18];
    if (sRenderWarbleToParticleView)
    {
        fn_8011D3CC(writer, source, vertices, parameter0, parameter1,
            parameter2);
    }
    else
    {
        fn_8011D5B0(writer, source, vertices, parameter0, parameter1,
            parameter2);
    }
    return true;
}

extern "C" void fn_8011D3CC(void* writer, void* source,
    UnidentifiedWarbleVertexList* vertices, int parameter0, int parameter1,
    int parameter2)
{
    UnidentifiedWarbleSource* warbleSource =
        static_cast<UnidentifiedWarbleSource*>(source);
    fn_802A8278(writer, warbleSource->mVertexCount * 4, 3, 0);

    UnidentifiedWarbleVertex* vertex = vertices->first;
    while (vertex != 0)
    {
        float transformed[20];
        fn_802E22D8(source, transformed, vertex->source, source, parameter0,
            parameter1, parameter2);
        vertex = vertex->next;
    }
}

extern "C" void fn_8011D5B0(void* writer, void* source,
    UnidentifiedWarbleVertexList* vertices, int parameter0, int parameter1,
    int parameter2)
{
    UnidentifiedWarbleSource* warbleSource =
        static_cast<UnidentifiedWarbleSource*>(source);
    static_cast<State_802A12E4*>(writer)->fn_802A1344(
        warbleSource->mVertexCount * 4, 3, 0);

    UnidentifiedWarbleVertex* vertex = vertices->first;
    while (vertex != 0)
    {
        float transformed[20];
        fn_802E22D8(source, transformed, vertex->source, source, parameter0,
            parameter1, parameter2);
        vertex = vertex->next;
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
