#ifndef NL_CALLBACK_TIMER_H
#define NL_CALLBACK_TIMER_H

#include "NL/nlFunction.h"

class FETimer
{
public:
    FETimer(
        float duration, const Function<FETimer*>& callback);

    void SetEnabled(bool enabled);
    void Update(float fDeltaT);

    /* 0x00 */ Function<FETimer*> mCallback;
    /* 0x08 */ bool mEnabled;
    /* 0x09 */ u8 mPadding09[3];
    /* 0x0C */ float mDuration;
    /* 0x10 */ float mElapsed;
    /* 0x14 */ int mExpirationCount;
    /* 0x18 */ int mUnidentified18;
}; // size 0x1C

#endif // NL_CALLBACK_TIMER_H
