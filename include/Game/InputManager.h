#ifndef GAME_NETWORK_INPUT_MANAGER_H
#define GAME_NETWORK_INPUT_MANAGER_H

#include "Game/NetworkInputMessages.h"
#include "Game/InputFrameProvider.h"
#include "NL/nlSlotPool.h"
#include "types.h"

class NetworkInputManager
{
public:
    void Reset();
    void SetFrameProvider(InputFrameProvider* frameProvider);
    int GetUpdateCount();
    void CaptureInputs();
    bool PrepareUpdate();

    /* 0x00 */ InputFrameProvider* mFrameProvider;
    /* 0x04 */ bool mEnabled;
    /* 0x05 */ u8 mPadding05[3];
    /* 0x08 */ u32 mCongestionStartTicker;
    /* 0x0C */ float mLastCongestionMilliseconds;
    /* 0x10 */ int mCongestionCount;
    /* 0x14 */ float mTotalCongestionMilliseconds;
    /* 0x18 */ bool mQueueOverflowReported;
    /* 0x19 */ u8 mPadding19[3];
}; // size: 0x1C

extern NetworkInputManager* gInputManager;

bool IsNetworkOrRecordedGame();
void InitializeInputManager();

#endif // GAME_NETWORK_INPUT_MANAGER_H
