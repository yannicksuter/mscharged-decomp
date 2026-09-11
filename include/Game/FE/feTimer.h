#ifndef GAME_FE_FETIMER_H
#define GAME_FE_FETIMER_H

#include "NL/nlFunction.h"
#include "NL/nlList.h"

class FETimer
{
public:
    FETimer(
        float duration, const Function<FETimer*>& callback);

    void SetEnabled(bool enabled);
    void Update(float dt);

    /* 0x00 */ Function<FETimer*> mCallback;
    /* 0x08 */ bool mEnabled;
    /* 0x09 */ u8 mPadding09[3];
    /* 0x0C */ float mDuration;
    /* 0x10 */ float mElapsed;
    /* 0x14 */ int mTickCount;
    /* 0x18 */ FETimer* next;
}; // size 0x1C

class UnidentifiedTimerList_80306294
{
public:
    UnidentifiedTimerList_80306294();
    ~UnidentifiedTimerList_80306294();

    FETimer* fn_8030639C(
        float duration, const Function<FETimer*>& callback);
    void fn_8030648C(FETimer* timer);
    void fn_80306524();
    void fn_803065F0(float dt);

    /* 0x00 */ FETimer* mCurrent;
    /* 0x04 */ nlList<FETimer> mTimers;
}; // size 0x0C

#endif // GAME_FE_FETIMER_H
