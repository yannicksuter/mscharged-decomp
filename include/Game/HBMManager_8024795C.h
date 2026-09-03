#ifndef GAME_HBM_MANAGER_8024795C_H
#define GAME_HBM_MANAGER_8024795C_H

#include "types.h"

struct KPADStatus;
struct TPLPalette;

struct UnidentifiedHBMDataInfo
{
    /* 0x00 */ void* layoutBuf;
    /* 0x04 */ void* spkSeBuf;
    /* 0x08 */ void* msgBuf;
    /* 0x0C */ void* configBuf;
    /* 0x10 */ void* mem;
    /* 0x14 */ bool (*sound_callback)(int, int);
    /* 0x18 */ int backFlag;
    /* 0x1C */ int region;
    /* 0x20 */ int cursor;
    /* 0x24 */ int messageFlag;
    /* 0x28 */ unsigned int memSize;
    /* 0x2C */ float frameDelta;
    /* 0x30 */ float adjust[2];
    /* 0x38 */ void* pAllocator;
}; // size 0x3C

struct UnidentifiedHBMKPadData
{
    /* 0x00 */ KPADStatus* kpad;
    /* 0x04 */ float pos[2];
    /* 0x0C */ unsigned int use_devtype;
}; // size 0x10

struct UnidentifiedHBMControllerData
{
    UnidentifiedHBMKPadData wiiCon[4];
}; // size 0x40

class UnidentifiedHBMManager
{
public:
    UnidentifiedHBMManager();
    virtual ~UnidentifiedHBMManager();

    static void fn_8024795C(void* data, unsigned long size, void* userData);
    void fn_80247B3C();
    static void fn_80247EB0();
    void fn_80248008();
    void fn_802480EC();
    static void fn_8024891C();
    bool fn_80248940();
    void fn_802489F8();

    /* 0x004 */ UnidentifiedHBMDataInfo mDataInfo;
    /* 0x040 */ UnidentifiedHBMControllerData mControllerData;
    /* 0x080 */ TPLPalette* mIconPalette;
    /* 0x084 */ void* mSoundData;
    /* 0x088 */ void* mSoundWork;
    /* 0x08C */ unsigned int mLoadedFileCount;
    /* 0x090 */ bool mReady;
    /* 0x091 */ bool mActive;
    /* 0x092 */ bool mBlocked;
    /* 0x093 */ u8 mPad93;
    /* 0x094 */ unsigned int mPreviousTaskState;
}; // size 0x98

extern UnidentifiedHBMManager* lbl_806E18D8;

#endif // GAME_HBM_MANAGER_8024795C_H
