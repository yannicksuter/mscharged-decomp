#ifndef GAME_SH_ONLINE_INVITE_PREVIEW_H
#define GAME_SH_ONLINE_INVITE_PREVIEW_H

#include "Game/BaseSceneHandler.h"
#include "Game/FE/fePointerButton.h"
#include "Game/FE/feTimer.h"

class TLComponentInstance;

class SHOnlineInvitePreview : public BaseSceneHandler
{
public:
    SHOnlineInvitePreview();
    virtual ~SHOnlineInvitePreview();
    virtual void Update(float fDeltaT);
    virtual void SceneCreated();

    void OnPreviewDelayElapsed(FETimer* timer);
    void OnContinuePointerEnter(unsigned int index, void* context);
    void OnContinuePointerLeave(unsigned int index, void* context);
    void OnContinuePointerPress(unsigned int index, void* context);

    /* 0x01C */ FEPointerButton mContinueButton;
    /* 0x0D0 */ TLComponentInstance* mContinueButtonInstance;
    /* 0x0D4 */ bool mButtonInitialized;
    /* 0x0D5 */ u8 mPaddingD5;
    /* 0x0D6 */ unsigned short mBestSeriesText[0x40];
    /* 0x156 */ unsigned short mGameLimitText[0x40];
    /* 0x1D6 */ unsigned short mStadiumText[0x40];
    /* 0x256 */ unsigned short mEnvironmentCheatText[0x40];
    /* 0x2D6 */ unsigned short mPowerupCheatText[0x40];
    /* 0x356 */ unsigned short mPlayerCheatText[0x40];
    /* 0x3D6 */ bool mPreviewDelayComplete;
    /* 0x3D7 */ u8 mPadding3D7;
    /* 0x3D8 */ FETimer mPreviewDelayTimer;
    /* 0x3F4 */ int mState;
}; // size 0x3F8

#endif // GAME_SH_ONLINE_INVITE_PREVIEW_H
