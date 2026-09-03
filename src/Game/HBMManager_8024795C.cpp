#include "Game/HBMManager_8024795C.h"

#include "Game/BaseGameSceneManager.h"
#include "Game/Event.h"
#include "Game/EventDataTypes.h"
#include "Game/GameInfo.h"
#include "Game/ResetTask.h"
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

typedef float Mtx44[4][4];

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
    void GXClearVtxDesc();
    void GXSetVtxAttrFmt(int format, int attribute, int componentCount,
        int componentType, unsigned char fractionalBits);
    void GXSetVtxDesc(int attribute, int type);
    void GXSetViewport(float x, float y, float width, float height,
        float nearZ, float farZ);
    void GXSetScissor(unsigned int x, unsigned int y, unsigned int width,
        unsigned int height);
    void GXSetNumChans(unsigned char count);
    void GXSetNumTexGens(unsigned char count);
    void GXSetNumTevStages(unsigned char count);
    void GXSetTevOrder(int stage, int coord, int map, int colour);
    void GXSetTevOp(int stage, int mode);
    void GXSetBlendMode(int mode, int source, int destination, int operation);
    void GXSetZMode(bool enable, int function, bool updateEnable);
    void GXSetCurrentMtx(unsigned int matrix);
    void GXSetProjection(const Mtx44 projection, int type);
    void GXSetChanCtrl(int channel, bool enable, int ambientSource,
        int materialSource, unsigned int lights, int diffuse,
        int attenuation);
    void C_MTXOrtho(Mtx44 projection, float top, float bottom, float left,
        float right, float nearZ, float farZ);
}

extern MemoryAllocator* AllocatorStack[16];
extern unsigned int AllocatorStackDepth;

struct UnidentifiedHBMGameState
{
    u8 mPad00[0x40];
    bool mBlocked;
};

struct UnidentifiedHBMDisplayState
{
    u8 mPad00[0x0C];
    bool mEnabled;
};

struct UnidentifiedPadUpdateState
{
    u8 mPad00[0x2F4];
    int mControllerTypes[4];
};

extern UnidentifiedHBMGameState* lbl_806E0C94;
extern BaseGameSceneManager* lbl_806E1838;
extern BaseGameSceneManager* lbl_806E1860;
extern void* lbl_806E2020;
extern UnidentifiedPadUpdateState* lbl_806E2478;

extern "C"
{
    UnidentifiedHBMDisplayState* fn_80271960();
    void fn_802719A0(UnidentifiedHBMDisplayState* state);
    void fn_80271A00(UnidentifiedHBMDisplayState* state);
    bool fn_80273B00();
    void* fn_80284A58();
    bool fn_80285E20(void* presentation);
    void fn_801FC444();
    void fn_801FC454();
    void fn_8035BE04(void* audio);
    void fn_8035BE74(void* audio);
    void* fn_80375EC8(UnidentifiedPadUpdateState* state, int index);
    void* fn_80375ED4(UnidentifiedPadUpdateState* state, int index);
    void* fn_80375EE0(UnidentifiedPadUpdateState* state, int index);
    void* fn_80253E18();
    void fn_80253E24(void* manager);
}

class UnidentifiedHBMScene : public BaseSceneHandler
{
public:
    virtual void UnidentifiedVirtual2C();
};

class UnidentifiedHBMHideEvent
    : public UnidentifiedStaticEvent<UnidentifiedEventData00, 8>
{
public:
    UnidentifiedHBMHideEvent()
        : UnidentifiedStaticEvent<UnidentifiedEventData00, 8>("HBMHide", -1)
    {
    }

    virtual ~UnidentifiedHBMHideEvent() { }
};

static UnidentifiedHBMHideEvent sHBMHideEvent;

UnidentifiedHBMManager* lbl_806E18D8;

void UnidentifiedHBMManager::fn_8024795C(
    void* data, unsigned long, void* userData)
{
    *(void**)userData = data;

    UnidentifiedHBMManager* manager = lbl_806E18D8;
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
    GXSetVtxAttrFmt(4, 9, 0, 4, 0);
    GXSetVtxAttrFmt(4, 11, 0, 1, 0);
    GXSetVtxDesc(9, 1);
    GXSetVtxDesc(11, 1);
    GXSetViewport(0.0f, 0.0f, 640.0f, 480.0f, 0.0f, 1.0f);
    GXSetScissor(0, 0, 640, 480);
    GXSetNumChans(1);
    GXSetNumTexGens(0);
    GXSetNumTevStages(1);
    GXSetTevOrder(0, 255, 255, 4);
    GXSetTevOp(0, 4);
    GXSetBlendMode(0, 0, 0, 0);
    GXSetZMode(true, 3, true);
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
    GXSetProjection(projection, 1);
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
        fn_802719A0(fn_80271960());
    }

    HBMCreate(&mDataInfo);
    HBMSetAdjustFlag(fn_80273B00());
    mActive = true;
    fn_8035BE04(lbl_806E2020);
    HBMCreateSound((const char*)mSoundData, mSoundWork, 0x19000);
    HBMInit();

    mPreviousTaskState = nlTaskManager::m_pInstance->mCurrentState;
    if (mPreviousTaskState != 1)
    {
        fn_800EC868();
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
        switch (lbl_806E2478->mControllerTypes[i])
        {
        case 0:
            mControllerData.wiiCon[i].kpad = 0;
            break;
        case 1:
            mControllerData.wiiCon[i].kpad
                = (KPADStatus*)((u8*)fn_80375EC8(lbl_806E2478, i) + 0x2C);
            break;
        case 2:
            mControllerData.wiiCon[i].kpad
                = (KPADStatus*)((u8*)fn_80375ED4(lbl_806E2478, i) + 0x34);
            break;
        case 3:
            mControllerData.wiiCon[i].kpad
                = (KPADStatus*)((u8*)fn_80375EE0(lbl_806E2478, i) + 0x38);
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
        if (lbl_806E18D8->mActive)
        {
            lbl_806E18D8->mActive = false;
            fn_8035BE74(lbl_806E2020);
            if (lbl_806E18D8->mPreviousTaskState != 1)
            {
                fn_800ECB50();
            }
            else
            {
                fn_801FC454();
            }
            fn_80271A00(fn_80271960());
            gxInit();
            GXSetChanCtrl(4, false, 0, 1, 0xFF, 2, 1);
            GXSetChanCtrl(5, false, 0, 1, 0xFF, 2, 1);
            sHBMHideEvent.UnidentifiedDeliver(
                (UnidentifiedEventData00*)&sHBMHideEvent);
        }
        nlTaskManager::SetNextState(mPreviousTaskState);
        break;

    case HBM_SELECTION_BTN1:
        HBMDeleteSound();
        HBMDelete();
        if (lbl_806E18D8->mActive)
        {
            lbl_806E18D8->mActive = false;
            fn_80271A00(fn_80271960());
            gxInit();
            GXSetChanCtrl(4, false, 0, 1, 0xFF, 2, 1);
            GXSetChanCtrl(5, false, 0, 1, 0xFF, 2, 1);
            sHBMHideEvent.UnidentifiedDeliver(
                (UnidentifiedEventData00*)&sHBMHideEvent);
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
        if (lbl_806E18D8->mActive)
        {
            lbl_806E18D8->mActive = false;
            fn_80271A00(fn_80271960());
            gxInit();
            GXSetChanCtrl(4, false, 0, 1, 0xFF, 2, 1);
            GXSetChanCtrl(5, false, 0, 1, 0xFF, 2, 1);
            sHBMHideEvent.UnidentifiedDeliver(
                (UnidentifiedEventData00*)&sHBMHideEvent);
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
    if (lbl_806E0C94 != 0 && lbl_806E0C94->mBlocked)
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

    if ((state & 4) == 0 && !fn_80285E20(fn_80284A58()))
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
        if (lbl_806E1860 != 0)
        {
            scene = lbl_806E1860->GetScene((SceneList)25);
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
        void* manager = fn_80253E18();
        if (manager != 0)
        {
            fn_80253E24(manager);
        }
    }
}
