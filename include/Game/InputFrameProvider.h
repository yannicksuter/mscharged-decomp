#ifndef GAME_INPUT_FRAME_PROVIDER_H
#define GAME_INPUT_FRAME_PROVIDER_H

#include "types.h"

class InputFrameProvider
{
public:
    virtual int GetFrame() = 0;
    virtual float GetFixedUpdateMilliseconds() = 0;
    virtual u32 CalculateChecksum() = 0;
    virtual u32 WriteSyncLog() = 0;
    virtual void OnSyncError() = 0;
    virtual void OnInputQueueOverflow() = 0;
    virtual u16 GetInputRemapAngle() = 0;
    virtual bool IsInPauseMenu() = 0;
};

#endif // GAME_INPUT_FRAME_PROVIDER_H
