#ifndef GAME_SH_SHNAVIGATION_H
#define GAME_SH_SHNAVIGATION_H

#include "Game/BaseSceneHandler.h"
#include "Game/FE/fePageControls.h"
#include "NL/nlBasicString.h"

extern NLString gNextFETransition;

class TLComponentInstance;
class TLInstance;

class SHNavigation : public BaseSceneHandler
{
public:
    SHNavigation();
    virtual ~SHNavigation();
    virtual void Update(float fDeltaT);
    virtual void SceneCreated();

    void SetButtonVisibility(int mask, bool visible);
    void RestoreButtonVisibility();
    void HideButtons();
    void SetButtons(int value, bool enabled);
    TLComponentInstance* GetButton(int value);
    FEPageControls* GetPageControls()
    {
        return &mPageControls;
    }
    void ShowHomeButtonWarning();
    void SetPointerTeamColours();
    void ResetButtons(bool enabled);
    void SetPlayButtonText(int value);
    void SetDoneButtonText(int value);
    void SetBackButtonText(int value);
    void StartTransition();

    /* 0x01C */ TLComponentInstance* mPointerInstances[4];
    /* 0x02C */ TLComponentInstance* mPlusButton;
    /* 0x030 */ TLComponentInstance* mMinusButton;
    /* 0x034 */ TLComponentInstance* mBackButton;
    /* 0x038 */ TLComponentInstance* mBreadcrumbs;
    /* 0x03C */ TLComponentInstance* mPlayButton;
    /* 0x040 */ TLComponentInstance* mDoneButton;
    /* 0x044 */ TLComponentInstance* mLowerDoneButton;
    /* 0x048 */ TLComponentInstance* mProgressButton;
    /* 0x04C */ TLComponentInstance* mTransition;
    /* 0x050 */ TLComponentInstance* mHomeWarning;
    /* 0x054 */ TLInstance* mTimer;
    /* 0x058 */ FEPageControls mPageControls;
    /* 0x1E0 */ int mUnidentified1E0;
    /* 0x1E4 */ unsigned char mVisibleButtons;
    /* 0x1E5 */ bool mTransitionPlaying;
    /* 0x1E6 */ bool mTransitionPending;
    /* 0x1E7 */ bool mHomeWarningPlaying;
    /* 0x1E8 */ bool mIsWidescreen;
    /* 0x1E9 */ unsigned char mPadding1E9[3];
}; // size 0x1EC

void SetPointerEnabled(bool value);

SHNavigation* GetNavigationScene();

#endif // GAME_SH_SHNAVIGATION_H
