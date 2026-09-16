#ifndef GAME_SH_SH_CUP_KNOCKOUT_H
#define GAME_SH_SH_CUP_KNOCKOUT_H

#include "Game/BaseSceneHandler.h"
#include "Game/FE/feBackButton.h"
#include "Game/FE/fePointerButton.h"

class CupInterface;
class FEPageControls;
class TLComponentInstance;
class TLImageInstance;
class TLInstance;
struct BasicGameInfo;
struct NetworkTournamentGame;

class CupKnockoutScene : public BaseSceneHandler
{
public:
    CupKnockoutScene();
    virtual ~CupKnockoutScene();
    virtual void Update(float fDeltaT);
    virtual void SceneCreated();

    void PopulateMatchup(BasicGameInfo* game, TLInstance* group, int index);
    void PopulateNetworkMatchup(NetworkTournamentGame* tournamentGame,
        BasicGameInfo* game, TLInstance* group, int index);
    void SetTeamLogo(TLImageInstance* image, int team);
    void PopulateBracket();
    void UpdateRoundHighlight();
    void InitializePointerButtons();
    void OnBracketPointerEnter(unsigned int index, void* context);
    void OnBracketPointerLeave(unsigned int index, void* context);
    void OnBracketPointerPress(unsigned int index, void* context);
    void OnMatchupPointerEnter(unsigned int index, void* context);
    void OnMatchupPointerLeave(unsigned int index, void* context);
    void OnMatchupPointerPress(unsigned int index, void* context);

    /* 0x01C */ unsigned short mTitleText[64];
    /* 0x09C */ unsigned short mScoreText[7][2][4];
    /* 0x10C */ unsigned short mStartTimerText[8];
    /* 0x11C */ unsigned short mGameTimerText[7][32];
    /* 0x2DC */ bool mUnidentified2DC;
    /* 0x2E0 */ CupInterface* mTournament;
    /* 0x2E4 */ bool mNetworkTournament;
    /* 0x2E8 */ float mUnidentified2E8;
    /* 0x2EC */ int mStartSeconds;
    /* 0x2F0 */ bool mUnidentified2F0;
    /* 0x2F1 */ bool mUnidentified2F1;
    /* 0x2F2 */ bool mUnidentified2F2;
    /* 0x2F3 */ bool mUnidentified2F3;
    /* 0x2F4 */ bool mUnidentified2F4;
    /* 0x2F5 */ bool mUnidentified2F5;
    /* 0x2F6 */ bool mUnidentified2F6;
    /* 0x2F7 */ bool mUnidentified2F7;
    /* 0x2F8 */ bool mPointerButtonsInitialized;
    /* 0x2F9 */ bool mBracketPressed;
    /* 0x2FA */ bool mPreviousPagePressed;
    /* 0x2FB */ bool mNextPagePressed;
    /* 0x2FC */ int mPointerHoverCounts[4];
    /* 0x30C */ FEBackButton mBackButton;
    /* 0x3E4 */ FEPointerButton mMatchupButtons[7];
    /* 0x8D0 */ FEPointerButton mBracketButton;
    /* 0x984 */ FEPageControls* mPageControls;
    /* 0x988 */ TLComponentInstance* mMatchupInstances[7];
    /* 0x9A4 */ TLComponentInstance* mOutgoingMatchupInstances[7];
    /* 0x9C0 */ TLComponentInstance* mBracketButtonInstance;
    /* 0x9C4 */ int mTransitionState;
}; // size 0x9C8

#endif // GAME_SH_SH_CUP_KNOCKOUT_H
