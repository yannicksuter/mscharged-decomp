#ifndef GAME_DEBUG_FRAME_COUNTER_H
#define GAME_DEBUG_FRAME_COUNTER_H

#include "types.h"

class FrameCounter
{
public:
    void StartTimer(int timer);
    void FinishTiming();

private:
    u8 mData[0x1078];
};

extern FrameCounter g_FrameCounter;

#endif // GAME_DEBUG_FRAME_COUNTER_H
