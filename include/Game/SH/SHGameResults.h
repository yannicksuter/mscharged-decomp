#ifndef GAME_SH_SH_GAME_RESULTS_H
#define GAME_SH_SH_GAME_RESULTS_H

#include "Game/SH/SHSceneBase.h"
#include "unclassified/tu_80209584.h"

class TLTextInstance;

struct UnidentifiedGameResultsData
{
    /* 0x00 */ u8 mUnidentified00[0x24];
    /* 0x24 */ TU80209584StatsSource mHome;
    /* 0x94 */ TU80209584StatsSource mAway;
}; // size 0x104

class UnidentifiedResultsListener
{
public:
    virtual void Virtual08() = 0;
    virtual void Virtual0C() = 0;
    virtual void Virtual10() = 0;
    virtual void Virtual14() = 0;
    virtual void Virtual18() = 0;
    virtual void Virtual1C() = 0;
    virtual void Virtual20() = 0;
    virtual void Virtual24() = 0;
    virtual void Virtual28(int value) = 0;
};

struct UnidentifiedGameClock
{
    /* 0x000 */ u8 mUnidentified00[0x138];
    /* 0x138 */ int mUnidentified138;
    /* 0x13C */ int mUnidentified13C;
}; // size unknown

class GameResultsScene : public UnidentifiedSHSceneBase
{
public:
    GameResultsScene();
    virtual ~GameResultsScene();
    virtual void Update(float dt);
    virtual void SceneCreated();
    virtual void SHSceneVirtual30();

    void fn_8020A494(UnidentifiedGameResultsData* data, UnidentifiedResultsListener* listener, UnidentifiedGameClock* clock);

    /* 0x5D4 */ TLTextInstance* mTitleText;
    /* 0x5D8 */ bool mUnidentified5D8;
    /* 0x5DC */ TU80209584Summary mSummary;
    /* 0xA10 */ UnidentifiedGameResultsData* mUnidentifiedA10;
    /* 0xA14 */ UnidentifiedResultsListener* mUnidentifiedA14;
    /* 0xA18 */ UnidentifiedGameClock* mUnidentifiedA18;
    /* 0xA1C */ u16 mTitleBuffer[0x20];
}; // size 0xA5C

#endif // GAME_SH_SH_GAME_RESULTS_H
