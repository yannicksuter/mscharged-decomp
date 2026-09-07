#include "Game/Audio/AudioBackend.h"
#include "NL/plat/PlatPadManager.h"
#include "Game/HBMManager_8024795C.h"

#include "unclassified/tu_80284A58.h"

#include "Game/BaseGameSceneManager.h"
#include "Game/Event.h"
#include "Game/EventDataTypes.h"
#include "Game/GameInfo.h"
#include "Game/Render/tu_80271960.h"
#include "Game/Task/ResetTask.h"
#include "Game/Sys/audio.h"
#include "Game/Sys/movie.h"
#include "NL/MemAlloc.h"
#include "NL/glx/glxGX.h"
#include "NL/nlFile.h"
#include "NL/nlFormat.h"
#include "NL/nlLocalization.h"
#include "NL/nlMemory.h"
#include "NL/nlTask.h"

#include <string.h>

enum UnidentifiedHBMSelectBtnNum
{
    HBM_SELECTION_NULL = -1,
    HBM_SELECTION_HOME = 0,
    HBM_SELECTION_BTN1 = 1,
    HBM_SELECTION_BTN2 = 2,
};

extern "C"
{
    unsigned char SCGetLanguage();
    void TPLBind(TPLPalette* palette);
    void HBMCreate(const UnidentifiedHBMDataInfo* info);
    void HBMDelete();
    void HBMInit();
    int HBMCalc(const UnidentifiedHBMControllerData* controllerData);
    void HBMDraw();
    int HBMGetSelectBtnNum();
    void HBMSetAdjustFlag(bool flag);
    void HBMCreateSound(const char* soundData, void* work, unsigned int size);
    void HBMDeleteSound();
    void HBMUpdateSound();
}

extern MemoryAllocator* AllocatorStack[16];
extern unsigned int AllocatorStackDepth;

struct UnidentifiedHBMGameState
{
    u8 mPad00[0x40];
    bool mBlocked;
};

class TU80252180Scene;

extern UnidentifiedHBMGameState* g_pGame;

extern "C"
{
    bool fn_80273B00();
    bool IsIdleAndNoShotInProgress(void* presentation);
    void fn_801FC444();
    void fn_801FC454();
    TU80252180Scene* fn_80253E18();
    void fn_80253E24(TU80252180Scene* scene);
}

class UnidentifiedHBMScene : public BaseSceneHandler
{
public:
    virtual void UnidentifiedVirtual2C();
};

class UnidentifiedHBMHideEvent
    : public UnidentifiedStaticEvent<UnidentifiedEventNoData, 8>
{
public:
    UnidentifiedHBMHideEvent()
        : UnidentifiedStaticEvent<UnidentifiedEventNoData, 8>("HBMHide", -1)
    {
    }

    virtual ~UnidentifiedHBMHideEvent() { }
};

static UnidentifiedHBMHideEvent sHBMHideEvent;

UnidentifiedHBMManager* gpHBMManager;

void UnidentifiedHBMManager::fn_8024795C(
    void* data, unsigned long, void* userData)
{
    *(void**)userData = data;

    UnidentifiedHBMManager* manager = gpHBMManager;
    ++manager->mLoadedFileCount;
    if (manager->mLoadedFileCount >= 6)
    {
        manager->mDataInfo.sound_callback = 0;
        manager->mDataInfo.backFlag = 1;
        manager->mDataInfo.cursor = 0;
        manager->mDataInfo.adjust[0] = 832.0f / 608.0f;
        manager->mDataInfo.adjust[1] = 1.0f;
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

UnidentifiedHBMManager::UnidentifiedHBMManager()
    : mSoundWork(0)
    , mLoadedFileCount(0)
    , mReady(false)
    , mActive(false)
    , mBlocked(false)
{
    for (int i = 0; i < 4; ++i)
    {
        mControllerData.wiiCon[i].pos[0] = 0.0f;
        mControllerData.wiiCon[i].pos[1] = 0.0f;
        mControllerData.wiiCon[i].use_devtype = 0;
        mControllerData.wiiCon[i].kpad = 0;
    }
}

UnidentifiedHBMManager::~UnidentifiedHBMManager()
{
    delete[] (u8*)mDataInfo.mem;
}

void UnidentifiedHBMManager::fn_80247B3C()
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
    nlLoadEntireFileAsync(path, fn_8024795C, &mDataInfo.layoutBuf, 32,
        AllocateStart, 0, 0, &VirtualAllocator);

    nlSNPrintf(path, sizeof(path), "%s%s", "HomeButton2", "/SpeakerSe.arc");
    nlLoadEntireFileAsync(path, fn_8024795C, &mDataInfo.spkSeBuf, 32,
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
    nlLoadEntireFileAsync(path, fn_8024795C, &mDataInfo.msgBuf, 32,
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
    nlLoadEntireFileAsync(path, fn_8024795C, &mDataInfo.configBuf, 32,
        AllocateStart, configBuffer, configBufferSize, 0);

    nlSNPrintf(path, sizeof(path), "%s%s", "HomeButton2", "/homeBtnIcon.tpl");
    nlLoadEntireFileAsync(path, fn_8024795C, &mIconPalette, 32,
        AllocateStart, 0, 0, &VirtualAllocator);

    nlSNPrintf(path, sizeof(path), "%s%s", "HomeButton2", "/HomeButtonSe.brsar");
    nlLoadEntireFileAsync(path, fn_8024795C, &mSoundData, 32,
        AllocateStart, 0, 0, &VirtualAllocator);
}

void UnidentifiedHBMManager::fn_80247EB0()
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

    if (fn_80273B00())
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

void UnidentifiedHBMManager::fn_80248008()
{
    if (!mReady || mActive)
    {
        return;
    }

    if (fn_80248940())
    {
        fn_802489F8();
        return;
    }

    if (!fn_80271960()->mEnabled)
    {
        fn_80271960()->fn_802719A0();
    }

    HBMCreate(&mDataInfo);
    HBMSetAdjustFlag(fn_80273B00());
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
        fn_801FC444();
    }

    MovieStop();
    nlTaskManager::m_pInstance->mLocked = false;
    nlTaskManager::SetNextState(0x02000000);
}

void UnidentifiedHBMManager::fn_802480EC()
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
                = &fn_80375EC8(g_pPlatPadManager, i)->kpad;
            break;
        case 2:
            mControllerData.wiiCon[i].kpad
                = &fn_80375ED4(g_pPlatPadManager, i)->kpad;
            break;
        case 3:
            mControllerData.wiiCon[i].kpad
                = &fn_80375EE0(g_pPlatPadManager, i)->kpad;
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
    case HBM_SELECTION_HOME:
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
                fn_801FC454();
            }
            fn_80271960()->fn_80271A00();
            gxInit();
            GXSetChanCtrl(GX_COLOR0A0, false, GX_SRC_REG, GX_SRC_VTX, (GXLightID)0xFF, GX_DF_CLAMP, GX_AF_SPOT);
            GXSetChanCtrl(GX_COLOR1A1, false, GX_SRC_REG, GX_SRC_VTX, (GXLightID)0xFF, GX_DF_CLAMP, GX_AF_SPOT);
            sHBMHideEvent.UnidentifiedDeliver();
        }
        nlTaskManager::SetNextState(mPreviousTaskState);
        break;

    case HBM_SELECTION_BTN1:
        HBMDeleteSound();
        HBMDelete();
        if (gpHBMManager->mActive)
        {
            gpHBMManager->mActive = false;
            fn_80271960()->fn_80271A00();
            gxInit();
            GXSetChanCtrl(GX_COLOR0A0, false, GX_SRC_REG, GX_SRC_VTX, (GXLightID)0xFF, GX_DF_CLAMP, GX_AF_SPOT);
            GXSetChanCtrl(GX_COLOR1A1, false, GX_SRC_REG, GX_SRC_VTX, (GXLightID)0xFF, GX_DF_CLAMP, GX_AF_SPOT);
            sHBMHideEvent.UnidentifiedDeliver();
        }
        ResetTask::s_ResetMode = 3;
        if (ResetTask::s_ResetState == RS_RUNNING)
        {
            ResetTask::s_ResetState = RS_STARTRESET;
        }
        break;

    case HBM_SELECTION_BTN2:
        HBMDeleteSound();
        HBMDelete();
        if (gpHBMManager->mActive)
        {
            gpHBMManager->mActive = false;
            fn_80271960()->fn_80271A00();
            gxInit();
            GXSetChanCtrl(GX_COLOR0A0, false, GX_SRC_REG, GX_SRC_VTX, (GXLightID)0xFF, GX_DF_CLAMP, GX_AF_SPOT);
            GXSetChanCtrl(GX_COLOR1A1, false, GX_SRC_REG, GX_SRC_VTX, (GXLightID)0xFF, GX_DF_CLAMP, GX_AF_SPOT);
            sHBMHideEvent.UnidentifiedDeliver();
        }
        ResetTask::s_ResetMode = 0;
        if (ResetTask::s_ResetState == RS_RUNNING)
        {
            ResetTask::s_ResetState = RS_STARTRESET;
        }
        break;
    }
}

void UnidentifiedHBMManager::fn_8024891C()
{
    fn_80247EB0();
    HBMDraw();
}

bool UnidentifiedHBMManager::fn_80248940()
{
    if (g_pGame != 0 && g_pGame->mBlocked)
    {
        return true;
    }

    unsigned int state = nlTaskManager::m_pInstance->mCurrentState;
    if ((state & 0x18) != 0 || (state & 0x01F80000) != 0)
    {
        return true;
    }

    if (GameInfoManager::Instance()->unknown_0x120)
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

void UnidentifiedHBMManager::fn_802489F8()
{
    unsigned int state = nlTaskManager::m_pInstance->mCurrentState;
    BaseSceneHandler* scene;

    if ((state & 0x00080000) != 0)
    {
        if (lbl_806E1838 != 0)
        {
            if (g_pLocalization->m_CurrentLanguage
                == nlLocalization::LangJapanese)
            {
                scene = lbl_806E1838->GetScene((SceneList)19);
            }
            else
            {
                scene = lbl_806E1838->GetScene((SceneList)18);
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
        if (lbl_806E1838 != 0)
        {
            scene = lbl_806E1838->GetScene((SceneList)25);
            if (scene != 0)
            {
                ((UnidentifiedHBMScene*)scene)->UnidentifiedVirtual2C();
            }
        }
    }
    else
    {
        TU80252180Scene* scene = fn_80253E18();
        if (scene != 0)
        {
            fn_80253E24(scene);
        }
    }
}
