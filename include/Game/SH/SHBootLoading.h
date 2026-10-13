#ifndef GAME_SH_SH_BOOT_LOADING_H
#define GAME_SH_SH_BOOT_LOADING_H

#include "Game/BaseSceneHandler.h"

class TLComponentInstance;
class TLImageInstance;

class BootLoadingScene : public BaseSceneHandler
{
public:
    enum BootPhase
    {
        PhaseRatings = 0,
        PhaseStrap = 1,
        PhaseNunchuk = 2,
        PhaseDeveloperLogo = 3,
        PhaseFinished = 4,
    };

    BootLoadingScene();
    virtual ~BootLoadingScene();
    virtual void Update(float fDeltaT);
    virtual void SceneCreated();
    virtual void ShowHomeButtonWarning();

    void SetPhaseSlide();
#if defined(VERSION_R4QE01) || defined(VERSION_R4QJ01) || defined(VERSION_R4QK01)
    bool IsBootScreenPending();
#endif

    /* 0x1C */ float mElapsedTime;
#if defined(VERSION_R4QK01)
    /* 0x20 */ float mUnknown20;
#endif
    /* 0x20 (Korea: 0x24) */ float mStrapAlpha;
    /* 0x24 (Korea: 0x28) */ bool mStrapDismissed;
    /* 0x25 (Korea: 0x29) */ u8 mPadding25[3];
    /* 0x28 (Korea: 0x2C) */ int mPhase;
    /* 0x2C (Korea: 0x30) */ TLImageInstance* mStrapImage;
#if defined(VERSION_R4QK01)
    /* 0x34 */ TLImageInstance* mAlternateStrapImage;
#endif
    /* 0x30 (Korea: 0x38) */ TLComponentInstance* mHomeButtonWarning;
    /* 0x34 (Korea: 0x3C) */ bool mHomeButtonWarningActive;
    /* 0x35 (Korea: 0x3D) */ bool mWidescreen;
    /* 0x36 (Korea: 0x3E) */ u8 mPadding36[2];
}; // size 0x38 (Korea: 0x40)

#endif // GAME_SH_SH_BOOT_LOADING_H
