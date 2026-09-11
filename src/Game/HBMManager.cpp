#include "Game/Audio/AudioBackend.h"
#include "Game/Render/RLViewLayers.h"
#include "Game/SH/SHNavigation.h"
#include "Game/FE/feMusic.h"
#include "NL/plat/PlatPadManager.h"
#include "Game/HBMManager.h"

#include "unclassified/tu_80284A58.h"

#include "Game/BaseGameSceneManager.h"
#include "Game/GameSceneManager.h"
#include "Game/Event.h"
#include "Game/EventDataTypes.h"
#include "Game/GameInfo.h"
#include "Game/Game.h"
#include "Game/Render/HomeButtonFade.h"
#include "Game/Task/ResetTask.h"
#include "Game/Sys/audio.h"
#include "Game/Audio/UnidentifiedRegistryPools.h"
#include "Game/Sys/movie.h"
#include "NL/MemAlloc.h"
#include "NL/glx/glxGX.h"
#include "NL/nlFile.h"
#include "NL/nlFormat.h"
#include "NL/nlLocalization.h"
#include "NL/nlMemory.h"
#include "NL/nlTask.h"

#include <revolution/hbm/HBMCommon.h>
#include <revolution/sc.h>
#include <revolution/tpl.h>
#include <string.h>

extern BaseGameSceneManager* g_pGameSceneManager;
extern BaseGameSceneManager* g_pOverlayManager;

extern MemoryAllocator* AllocatorStack[16];
extern unsigned int AllocatorStackDepth;

class UnidentifiedHBMScene : public BaseSceneHandler
{
public:
    virtual void UnidentifiedVirtual2C();
};

class HBMHideEvent
    : public UnidentifiedStaticEvent<UnidentifiedEventNoData, 8>
{
public:
    HBMHideEvent()
        : UnidentifiedStaticEvent<UnidentifiedEventNoData, 8>("HBMHide", -1)
    {
    }

    virtual ~HBMHideEvent() { }
};

static HBMHideEvent sHBMHideEvent;

HBMManager* gpHBMManager;

void HBMManager::OnFileLoaded(
    void* data, unsigned long, void* userData)
{
    *(void**)userData = data;

    HBMManager* manager = gpHBMManager;
    ++manager->mLoadedFileCount;
    if (manager->mLoadedFileCount >= 6)
    {
        manager->mDataInfo.sound_callback = 0;
        manager->mDataInfo.backFlag = 1;
        manager->mDataInfo.cursor = 0;
        manager->mDataInfo.adjust.x = 832.0f / 608.0f;
        manager->mDataInfo.adjust.y = 1.0f;
        manager->mDataInfo.frameDelta = 1.0f;

        TPLBind(manager->mIconPalette);

        manager->mDataInfo.mem = nlMalloc(0x80000, 8, false);
        manager->mDataInfo.memSize = 0x80000;
        manager->mDataInfo.pAllocator = 0;

        CurrentAllocator = &VirtualAllocator;
        AllocatorStack[AllocatorStackDepth++] = &VirtualAllocator;
        manager->mSoundWork = nlMalloc(0x19000, 8, false);
        --AllocatorStackDepth;
        AllocatorStack[AllocatorStackDepth] = 0;
        CurrentAllocator = AllocatorStack[AllocatorStackDepth - 1];

        manager->mReady = true;
    }
}

HBMManager::HBMManager()
    : mSoundWork(0)
    , mLoadedFileCount(0)
    , mReady(false)
    , mActive(false)
    , mBlocked(false)
{
    for (int i = 0; i < 4; ++i)
    {
        mControllerData.wiiCon[i].pos.x = 0.0f;
        mControllerData.wiiCon[i].pos.y = 0.0f;
        mControllerData.wiiCon[i].use_devtype = 0;
        mControllerData.wiiCon[i].kpad = 0;
    }
}

HBMManager::~HBMManager()
{
    delete[] (u8*)mDataInfo.mem;
}

void HBMManager::LoadResources()
{
    const char* archiveName;
    char path[32];

    mDataInfo.region = SCGetLanguage();
    switch (mDataInfo.region)
    {
    case 0:
        archiveName = "/homeBtn.arc";
        break;
    case 1:
        archiveName = "/homeBtn_ENG.arc";
        break;
    case 2:
        archiveName = "/homeBtn_GER.arc";
        break;
    case 3:
        archiveName = "/homeBtn_FRA.arc";
        break;
    case 4:
        archiveName = "/homeBtn_SPA.arc";
        break;
    case 5:
        archiveName = "/homeBtn_ITA.arc";
        break;
    case 6:
        archiveName = "/homeBtn_NED.arc";
        break;
    default:
        mDataInfo.region = 0;
        archiveName = "/homeBtn.arc";
        break;
    }

    nlSNPrintf(path, sizeof(path), "%s%s", "HomeButton2", archiveName);
    nlLoadEntireFileAsync(path, OnFileLoaded, &mDataInfo.layoutBuf, 32,
        AllocateStart, 0, 0, &VirtualAllocator);

    nlSNPrintf(path, sizeof(path), "%s%s", "HomeButton2", "/SpeakerSe.arc");
    nlLoadEntireFileAsync(path, OnFileLoaded, &mDataInfo.spkSeBuf, 32,
        AllocateStart, 0, 0, &VirtualAllocator);

    nlSNPrintf(path, sizeof(path), "%s%s", "HomeButton2", "/home_nosave.csv");
    nlFile* file = nlOpen(path);
    unsigned int messageSize = 0;
    nlFileSize(file, &messageSize);
    nlClose(file);
    ++messageSize;

    AllocatorStack[AllocatorStackDepth++] = &VirtualAllocator;
    CurrentAllocator = &VirtualAllocator;
    unsigned int messageBufferSize;
    void* messageBuffer = nlMalloc(messageSize, 32, false);
    messageBufferSize = messageSize;
    --AllocatorStackDepth;
    AllocatorStack[AllocatorStackDepth] = 0;
    CurrentAllocator = AllocatorStack[AllocatorStackDepth - 1];

    memset(messageBuffer, 0, messageBufferSize);
    nlLoadEntireFileAsync(path, OnFileLoaded, &mDataInfo.msgBuf, 32,
        AllocateStart, messageBuffer, messageBufferSize, 0);

    nlSNPrintf(path, sizeof(path), "%s%s", "HomeButton2", "/config.txt");
    file = nlOpen(path);
    unsigned int configSize = 0;
    nlFileSize(file, &configSize);
    nlClose(file);
    ++configSize;

    AllocatorStack[AllocatorStackDepth++] = &VirtualAllocator;
    CurrentAllocator = &VirtualAllocator;
    unsigned int configBufferSize;
    void* configBuffer = nlMalloc(configSize, 32, false);
    configBufferSize = configSize;
    --AllocatorStackDepth;
    AllocatorStack[AllocatorStackDepth] = 0;
    CurrentAllocator = AllocatorStack[AllocatorStackDepth - 1];

    memset(configBuffer, 0, configBufferSize);
    nlLoadEntireFileAsync(path, OnFileLoaded, &mDataInfo.configBuf, 32,
        AllocateStart, configBuffer, configBufferSize, 0);

    nlSNPrintf(path, sizeof(path), "%s%s", "HomeButton2", "/homeBtnIcon.tpl");
    nlLoadEntireFileAsync(path, OnFileLoaded, &mIconPalette, 32,
        AllocateStart, 0, 0, &VirtualAllocator);

    nlSNPrintf(path, sizeof(path), "%s%s", "HomeButton2", "/HomeButtonSe.brsar");
    nlLoadEntireFileAsync(path, OnFileLoaded, &mSoundData, 32,
        AllocateStart, 0, 0, &VirtualAllocator);
}

void HBMManager::SetupGX()
{
    Mtx44 projection;

    GXClearVtxDesc();
    GXSetVtxAttrFmt(GX_VTXFMT4, GX_VA_POS, GX_POS_XY, GX_F32, 0);
    GXSetVtxAttrFmt(GX_VTXFMT4, GX_VA_CLR0, GX_CLR_RGB, GX_RGB8, 0);
    GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
    GXSetVtxDesc(GX_VA_CLR0, GX_DIRECT);
    GXSetViewport(0.0f, 0.0f, 640.0f, 480.0f, 0.0f, 1.0f);
    GXSetScissor(0, 0, 640, 480);
    GXSetNumChans(1);
    GXSetNumTexGens(0);
    GXSetNumTevStages(1);
    GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR0A0);
    GXSetTevOp(GX_TEVSTAGE0, GX_PASSCLR);
    GXSetBlendMode(GX_BM_NONE, GX_BL_ZERO, GX_BL_ZERO, GX_LO_CLEAR);
    GXSetZMode(true, GX_LEQUAL, true);
    GXSetCurrentMtx(3);

    if (IsWidescreen())
    {
        C_MTXOrtho(projection, 228.0f, -228.0f, -416.0f, 416.0f,
            0.0f, 500.0f);
    }
    else
    {
        C_MTXOrtho(projection, 228.0f, -228.0f, -304.0f, 304.0f,
            0.0f, 500.0f);
    }
    GXSetProjection(projection, GX_ORTHOGRAPHIC);
}

void HBMManager::Show()
{
    if (!mReady || mActive)
    {
        return;
    }

    if (IsBlocked())
    {
        OnHomeButtonPressed();
        return;
    }

    if (!HomeButtonFade::Instance()->mEnabled)
    {
        HomeButtonFade::Instance()->FadeOut();
    }

    HBMCreate(&mDataInfo);
    HBMSetAdjustFlag(IsWidescreen());
    mActive = true;
    g_pAudioBackend->SuspendControllerSpeakers();
    HBMCreateSound((const char*)mSoundData, mSoundWork, 0x19000);
    HBMInit();

    mPreviousTaskState = nlTaskManager::m_pInstance->mCurrentState;
    if (mPreviousTaskState != 1)
    {
        PauseAllAudio();
    }
    else
    {
        FEMusic::PauseStream();
    }

    MovieStop();
    nlTaskManager::m_pInstance->mLocked = false;
    nlTaskManager::SetNextState(0x02000000);
}

void HBMManager::Update()
{
    for (int i = 0; i < 4; ++i)
    {
        switch (g_pPlatPadManager->type[i])
        {
        case 0:
            mControllerData.wiiCon[i].kpad = 0;
            break;
        case 1:
            mControllerData.wiiCon[i].kpad
                = &g_pPlatPadManager->GetRemoteStatus(i)->kpad;
            break;
        case 2:
            mControllerData.wiiCon[i].kpad
                = &g_pPlatPadManager->GetFreestyleStatus(i)->kpad;
            break;
        case 3:
            mControllerData.wiiCon[i].kpad
                = &g_pPlatPadManager->GetClassicStatus(i)->kpad;
            break;
        }
    }

    if (!mReady || !mActive)
    {
        return;
    }

    HBMCalc(&mControllerData);
    HBMUpdateSound();

    switch (HBMGetSelectBtnNum())
    {
    case HBM_SELECT_HOMEBTN:
        HBMDeleteSound();
        HBMDelete();
        if (gpHBMManager->mActive)
        {
            gpHBMManager->mActive = false;
            g_pAudioBackend->ResumeControllerSpeakers();
            if (gpHBMManager->mPreviousTaskState != 1)
            {
                ResumeAllAudio();
            }
            else
            {
                FEMusic::ResumeStream();
            }
            HomeButtonFade::Instance()->FadeIn();
            gxInit();
            GXSetChanCtrl(GX_COLOR0A0, false, GX_SRC_REG, GX_SRC_VTX, (GXLightID)0xFF, GX_DF_CLAMP, GX_AF_SPOT);
            GXSetChanCtrl(GX_COLOR1A1, false, GX_SRC_REG, GX_SRC_VTX, (GXLightID)0xFF, GX_DF_CLAMP, GX_AF_SPOT);
            sHBMHideEvent.Deliver();
        }
        nlTaskManager::SetNextState(mPreviousTaskState);
        break;

    case HBM_SELECT_BTN1:
        HBMDeleteSound();
        HBMDelete();
        if (gpHBMManager->mActive)
        {
            gpHBMManager->mActive = false;
            HomeButtonFade::Instance()->FadeIn();
            gxInit();
            GXSetChanCtrl(GX_COLOR0A0, false, GX_SRC_REG, GX_SRC_VTX, (GXLightID)0xFF, GX_DF_CLAMP, GX_AF_SPOT);
            GXSetChanCtrl(GX_COLOR1A1, false, GX_SRC_REG, GX_SRC_VTX, (GXLightID)0xFF, GX_DF_CLAMP, GX_AF_SPOT);
            sHBMHideEvent.Deliver();
        }
        ResetTask::s_ResetMode = 3;
        if (ResetTask::s_ResetState == RS_RUNNING)
        {
            ResetTask::s_ResetState = RS_STARTRESET;
        }
        break;

    case HBM_SELECT_BTN2:
        HBMDeleteSound();
        HBMDelete();
        if (gpHBMManager->mActive)
        {
            gpHBMManager->mActive = false;
            HomeButtonFade::Instance()->FadeIn();
            gxInit();
            GXSetChanCtrl(GX_COLOR0A0, false, GX_SRC_REG, GX_SRC_VTX, (GXLightID)0xFF, GX_DF_CLAMP, GX_AF_SPOT);
            GXSetChanCtrl(GX_COLOR1A1, false, GX_SRC_REG, GX_SRC_VTX, (GXLightID)0xFF, GX_DF_CLAMP, GX_AF_SPOT);
            sHBMHideEvent.Deliver();
        }
        ResetTask::s_ResetMode = 0;
        if (ResetTask::s_ResetState == RS_RUNNING)
        {
            ResetTask::s_ResetState = RS_STARTRESET;
        }
        break;
    }
}

void HBMManager::Render()
{
    SetupGX();
    HBMDraw();
}

bool HBMManager::IsBlocked()
{
    if (g_pGame != 0 && g_pGame->mbCaptainShotToScoreOn)
    {
        return true;
    }

    unsigned int state = nlTaskManager::m_pInstance->mCurrentState;
    if ((state & 0x18) != 0 || (state & 0x01F80000) != 0)
    {
        return true;
    }

    if (GameInfoManager::Instance()->mIsOnlineMode)
    {
        return true;
    }

    if ((state & 4) == 0 && !IsIdleAndNoShotInProgress(GetPresentation()))
    {
        return true;
    }

    if (mBlocked)
    {
        return true;
    }

    return false;
}

void HBMManager::OnHomeButtonPressed()
{
    unsigned int state = nlTaskManager::m_pInstance->mCurrentState;
    BaseSceneHandler* scene;

    if ((state & 0x00080000) != 0)
    {
        if (g_pGameSceneManager != 0)
        {
            if (g_pLocalization->m_CurrentLanguage
                == nlLocalization::LangJapanese)
            {
                scene = g_pGameSceneManager->GetScene((SceneList)19);
            }
            else
            {
                scene = g_pGameSceneManager->GetScene((SceneList)18);
            }
            if (scene != 0)
            {
                ((UnidentifiedHBMScene*)scene)->UnidentifiedVirtual2C();
            }
        }
    }
    else if ((state & 0x00200000) != 0)
    {
        if (g_pOverlayManager != 0)
        {
            scene = g_pOverlayManager->GetScene((SceneList)25);
            if (scene != 0)
            {
                ((UnidentifiedHBMScene*)scene)->UnidentifiedVirtual2C();
            }
        }
    }
    else if ((state & 0x00400000) != 0)
    {
        if (g_pGameSceneManager != 0)
        {
            scene = g_pGameSceneManager->GetScene((SceneList)25);
            if (scene != 0)
            {
                ((UnidentifiedHBMScene*)scene)->UnidentifiedVirtual2C();
            }
        }
    }
    else
    {
        SHNavigation* scene = GetNavigationScene();
        if (scene != 0)
        {
            scene->ShowHomeButtonWarning();
        }
    }
}
