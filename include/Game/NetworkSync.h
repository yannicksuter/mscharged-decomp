#ifndef GAME_NETWORK_SYNC_MONITOR_H
#define GAME_NETWORK_SYNC_MONITOR_H

#include "types.h"

class DebugWriteCache;

struct NetworkSyncState
{
    void Reset(bool resetCache);
    DebugWriteCache* GetWriteCache();
    void OnFrameMismatch(int value);
    void OnChecksumMismatch(int value);
    void OnRandomSeedMismatch(int value);
    void SetMachineInfo(int machine, int machineCount);
    void DebugDraw();

    NetworkSyncState()
    {
        mFrameMismatchFrame = -1;
        mChecksumMismatchFrame = -1;
        mRandomSeedMismatchFrame = -1;
        mTriggered = false;
        mCaptureEnabled = true;
        mMachine = -1;
        mMachineCount = -1;
        mEnabled = true;
        mWriteCache = 0;
    }

    /* 0x00 */ int mFrameMismatchFrame;
    /* 0x04 */ int mChecksumMismatchFrame;
    /* 0x08 */ int mRandomSeedMismatchFrame;
    /* 0x0C */ bool mTriggered;
    /* 0x0D */ bool mEnabled;
    /* 0x0E */ u8 mPadding0E[2];
    /* 0x10 */ int mMachine;
    /* 0x14 */ int mMachineCount;
    /* 0x18 */ bool mCaptureEnabled;
    /* 0x19 */ u8 mPadding19[3];
    /* 0x1C */ DebugWriteCache* mWriteCache;
}; // size: 0x20

extern NetworkSyncState* gNetworkSyncState;
extern bool g_bBreakOnSyncError;

void InitializeNetworkSyncState();

#endif // GAME_NETWORK_SYNC_MONITOR_H
