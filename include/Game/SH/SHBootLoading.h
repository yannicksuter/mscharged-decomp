#ifndef GAME_SH_SH_BOOT_LOADING_H
#define GAME_SH_SH_BOOT_LOADING_H

#include "Game/BaseSceneHandler.h"

class TLComponentInstance;
class TLImageInstance;

class BootLoadingScene : public BaseSceneHandler
{
public:
    BootLoadingScene();
    virtual ~BootLoadingScene();
    virtual void Update(float fDeltaT);
    virtual void SceneCreated();
    virtual void ShowHomeButtonWarning();

    void SetPhaseSlide();
    bool IsBootScreenPending();

    /* 0x1C */ float mElapsedTime;
    /* 0x20 */ float mStrapAlpha;
    /* 0x24 */ bool mStrapDismissed;
    /* 0x25 */ u8 mPadding25[3];
    /* 0x28 */ int mPhase;
    /* 0x2C */ TLImageInstance* mStrapImage;
    /* 0x30 */ TLComponentInstance* mHomeButtonWarning;
    /* 0x34 */ bool mHomeButtonWarningActive;
    /* 0x35 */ bool mWidescreen;
    /* 0x36 */ u8 mPadding36[2];
}; // size 0x38

#endif // GAME_SH_SH_BOOT_LOADING_H
