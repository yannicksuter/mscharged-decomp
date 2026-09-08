#ifndef GAME_FE_FETIMER_H
#define GAME_FE_FETIMER_H

#include "NL/nlFunction.h"

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
    /* 0x18 */ FETimer* m_next;
}; // size 0x1C

#endif // GAME_FE_FETIMER_H
