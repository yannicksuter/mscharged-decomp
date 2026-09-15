#ifndef GAME_SH_SH_STRIKER_TIMES_CHALLENGE_H
#define GAME_SH_SH_STRIKER_TIMES_CHALLENGE_H

#include "Game/SH/SHStrikerTimesBase.h"
#include "Game/FE/feBackButton.h"

class SHStrikerTimesChallenge : public SHStrikerTimesBase
{
public:
    SHStrikerTimesChallenge();
    virtual ~SHStrikerTimesChallenge();
    virtual void Update(float dt);
    virtual void SceneCreated();
    virtual void SetDisplayMode(unsigned int transition);
    virtual void OnDoneTransitionComplete();
    virtual void OnBackTransitionComplete();

    static void OnSelectNewChallenge();
    static void OnSelectMainMenu();
    static void OnRestartChallenge();

    /* 0x5D4 */ FEBackButton mBackButton;
    /* 0x6AC */ TLComponentInstance* mBackButtonInstance;
    /* 0x6B0 */ bool mLoadingChallengeSettings;
    /* 0x6B1 */ bool mNewUnlock;
}; // size 0x6B4

#endif // GAME_SH_SH_STRIKER_TIMES_CHALLENGE_H
