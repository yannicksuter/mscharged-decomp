#ifndef GAME_SH_SHSTRIKERCUPAWARDS_H
#define GAME_SH_SHSTRIKERCUPAWARDS_H

#include "Game/BaseSceneHandler.h"
#include "Game/FE/feScrollText.h"
#include "Game/FE/feScrollBar.h"
#include "Game/FE/fePointerButton.h"
#include "Game/FE/feBackButton.h"

class TLComponentInstance;
class FEPageControls;

class StrikerCupAwardsScene : public BaseSceneHandler
{
public:
    StrikerCupAwardsScene(int cupPage);
    virtual ~StrikerCupAwardsScene();
    virtual void Update(float fDeltaT);
    virtual void SceneCreated();

    void UpdateAwards();
    void InitializePointerButtons();
    void OnButtonPointerPress(unsigned int index, void* context);
    void OnTeamPointerEnter(unsigned int index, void* context);
    void OnTeamPointerLeave(unsigned int index, void* context);
    bool PopulateTeamRow(int row, int teamPosition);
    void OnButtonPointerEnter(unsigned int index, void* context);
    void OnButtonPointerLeave(unsigned int index, void* context);

    /* 0x01C */ TLComponentInstance* mTeamRows[4];
    /* 0x02C */ int mScrollOffset;
    /* 0x030 */ int mAwardTeamIndices[10];
    /* 0x058 */ int mCupPage;
    /* 0x05C */ u16 mStatText[4][4];
    /* 0x07C */ unsigned char mPadding07C[0x80];
    /* 0x0FC */ bool mPointerButtonsInitialized;
    /* 0x0FD */ bool mPlayButtonPressed;
    /* 0x0FE */ bool mPreviousPagePressed;
    /* 0x0FF */ bool mNextPagePressed;
    /* 0x100 */ int mPointerHoverCounts[4];
    /* 0x110 */ int mUserTeamPosition;
    /* 0x114 */ FEScrollBar mScrollBar;
    /* 0x2C8 */ FEBackButton mBackButton;
    /* 0x3A0 */ FEPageControls* mPageControls;
    /* 0x3A4 */ FEPointerButton mPlayButton;
    /* 0x458 */ FEPointerButton mTeamButtons[4];
    /* 0x728 */ FEScrollText mDescriptionScroller;
    /* 0x768 */ TLComponentInstance* mUserTeamHighlight;
    /* 0x76C */ TLComponentInstance* mPlayButtonInstance;
    /* 0x770 */ int mTransitionState;
}; // size 0x774

#endif // GAME_SH_SHSTRIKERCUPAWARDS_H
