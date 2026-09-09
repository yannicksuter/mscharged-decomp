#ifndef GAME_HBM_MANAGER_H
#define GAME_HBM_MANAGER_H

#include <revolution/hbm/HBMTypes.h>

#include "types.h"

struct TPLPalette;

class HBMManager
{
public:
    HBMManager();
    virtual ~HBMManager();

    static void OnFileLoaded(void* data, unsigned long size, void* userData);
    void LoadResources();
    static void SetupGX();
    void Show();
    void Update();
    static void Render();
    void SetBlocked(bool blocked) { mBlocked = blocked; }
    bool IsBlocked();
    void OnHomeButtonPressed();

    /* 0x004 */ HBMDataInfo mDataInfo;
    /* 0x040 */ HBMControllerData mControllerData;
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

extern HBMManager* gpHBMManager;


#endif // GAME_HBM_MANAGER_H
