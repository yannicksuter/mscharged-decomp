#ifndef GAME_SH_SH_CUP_FINAL_ROUNDS_H
#define GAME_SH_SH_CUP_FINAL_ROUNDS_H

#include "Game/BaseSceneHandler.h"
#include "Game/FE/fePointerButton.h"
#include "Game/FE/feBackButton.h"

class TLComponentInstance;
class TLInstance;
class FEPageControls;
struct BasicGameInfo;

class CupFinalRoundsScene : public BaseSceneHandler
{
public:
    CupFinalRoundsScene();
    virtual ~CupFinalRoundsScene();
    virtual void Update(float fDeltaT);
    virtual void SceneCreated();

    void InitializePointerButtons();
    void OnBracketPointerPress(unsigned int index, void* context);
    void OnMatchupPointerPress(unsigned int index, void* context);
    void PopulateMatchup(BasicGameInfo* game, TLInstance* group, int index);
    void UpdateRoundHighlight();
    void OnBracketPointerEnter(unsigned int index, void* context);
    void OnBracketPointerLeave(unsigned int index, void* context);
    void OnMatchupPointerEnter(unsigned int index, void* context);
    void OnMatchupPointerLeave(unsigned int index, void* context);

    /* 0x01C */ unsigned short mTitleText[64];
    /* 0x09C */ unsigned short mScoreText[3][2][4];
    /* 0x0CC */ bool mPointerButtonsInitialized;
    /* 0x0CD */ bool mBracketPressed;
    /* 0x0CE */ bool mPreviousPagePressed;
    /* 0x0CF */ bool mNextPagePressed;
    /* 0x0D0 */ int mPointerHoverCounts[4];
    /* 0x0E0 */ FEBackButton mBackButton;
    /* 0x1B8 */ FEPointerButton mMatchupButtons[3];
    /* 0x3D4 */ FEPageControls* mPageControls;
    /* 0x3D8 */ FEPointerButton mBracketButton;
    /* 0x48C */ TLComponentInstance* mBracketButtonInstance;
    /* 0x490 */ TLComponentInstance* mMatchupInstances[3];
    /* 0x49C */ int mTransitionState;
}; // size 0x4A0

#endif // GAME_SH_SH_CUP_FINAL_ROUNDS_H
