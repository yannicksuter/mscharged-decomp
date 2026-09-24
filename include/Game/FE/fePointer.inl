#ifndef GAME_FE_FEPOINTER_INL
#define GAME_FE_FEPOINTER_INL

#include "Game/FE/fePointer.h"

inline void FEPointerListener::Disable()
{
    mDisabled = true;
    FEPointerEvent event;
    for (int i = 0; i < 4; ++i)
        mPreviousEvents[i] = event;
}

#endif // GAME_FE_FEPOINTER_INL
