#ifndef GAME_SH_SH_CUP_NEWS_H
#define GAME_SH_SH_CUP_NEWS_H

#include "Game/SH/SHStrikerTimesBase.h"

class CupNewsScene : public SHStrikerTimesBase
{
public:
    CupNewsScene();
    virtual ~CupNewsScene();
    virtual void Update(float dt);
    virtual void SceneCreated();
    virtual void SetDisplayMode(unsigned int transition);
    virtual void OnDoneTransitionComplete();

    /* 0x5D4 */ bool mShowAwardsOnClose;
    /* 0x5D5 */ bool mShowWinnerRewardsOnClose;
}; // size 0x5D8

#endif // GAME_SH_SH_CUP_NEWS_H
