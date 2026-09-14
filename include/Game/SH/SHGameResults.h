#ifndef GAME_SH_SH_GAME_RESULTS_H
#define GAME_SH_SH_GAME_RESULTS_H

#include "Game/SH/SHStrikerTimesBase.h"
#include "unclassified/tu_80209584.h"

class TLTextInstance;

class BasicGameInfo;

struct UnidentifiedGameClock
{
    /* 0x000 */ u8 mUnidentified00[0x138];
    /* 0x138 */ int mUnidentified138;
    /* 0x13C */ int mUnidentified13C;
}; // size unknown

class GameResultsScene : public SHStrikerTimesBase
{
public:
    GameResultsScene();
    virtual ~GameResultsScene();
    virtual void Update(float dt);
    virtual void SceneCreated();
    virtual void OnDoneTransitionComplete();

    void fn_8020A494(BasicGameInfo* data, BaseSceneHandler* listener, UnidentifiedGameClock* clock);

    /* 0x5D4 */ TLTextInstance* mTitleText;
    /* 0x5D8 */ bool mUnidentified5D8;
    /* 0x5DC */ TU80209584Summary mSummary;
    /* 0xA10 */ BasicGameInfo* mUnidentifiedA10;
    /* 0xA14 */ BaseSceneHandler* mUnidentifiedA14;
    /* 0xA18 */ UnidentifiedGameClock* mUnidentifiedA18;
    /* 0xA1C */ u16 mTitleBuffer[0x20];
}; // size 0xA5C

#endif // GAME_SH_SH_GAME_RESULTS_H
