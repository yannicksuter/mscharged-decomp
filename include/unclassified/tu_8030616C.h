#ifndef UNCLASSIFIED_TU_8030616C_H
#define UNCLASSIFIED_TU_8030616C_H

#include "NL/nlFunction.h"

class UnidentifiedTimer_8030616C
{
public:
    UnidentifiedTimer_8030616C(
        float duration, const Function<FnVoidVoid>& callback);

    /* 0x00 */ Function<FnVoidVoid> mCallback;
    /* 0x08 */ bool mEnabled;
    /* 0x09 */ u8 mPadding09[3];
    /* 0x0C */ float mDuration;
    /* 0x10 */ float mElapsed;
    /* 0x14 */ int mUnidentified14;
    /* 0x18 */ int mUnidentified18;
}; // size 0x1C

#endif // UNCLASSIFIED_TU_8030616C_H
