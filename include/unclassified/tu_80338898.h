#ifndef UNCLASSIFIED_TU_80338898_H
#define UNCLASSIFIED_TU_80338898_H

#include "types.h"

class DebugWriteCache;

struct UnidentifiedNetworkSyncState
{
    UnidentifiedNetworkSyncState()
    {
        mValue0 = -1;
        mValue1 = -1;
        mValue2 = -1;
        mTriggered = false;
        mCaptureEnabled = true;
        mMachine = -1;
        mMachineCount = -1;
        mEnabled = true;
        mWriteCache = 0;
    }

    /* 0x00 */ int mValue0;
    /* 0x04 */ int mValue1;
    /* 0x08 */ int mValue2;
    /* 0x0C */ bool mTriggered;
    /* 0x0D */ bool mEnabled;
    /* 0x0E */ u8 mPadding0E[2];
    /* 0x10 */ int mMachine;
    /* 0x14 */ int mMachineCount;
    /* 0x18 */ bool mCaptureEnabled;
    /* 0x19 */ u8 mPadding19[3];
    /* 0x1C */ DebugWriteCache* mWriteCache;
}; // size: 0x20

extern UnidentifiedNetworkSyncState* lbl_806E2168;
extern bool g_bBreakOnSyncError;

extern "C" void fn_80338898();
extern "C" void fn_80338900(
    UnidentifiedNetworkSyncState* state, bool resetCache);
extern "C" DebugWriteCache* fn_80338950(
    UnidentifiedNetworkSyncState* state);
extern "C" void fn_80338958(
    UnidentifiedNetworkSyncState* state, int value);
extern "C" void fn_803389D8(
    UnidentifiedNetworkSyncState* state, int value);
extern "C" void fn_80338A58(
    UnidentifiedNetworkSyncState* state, int value);
extern "C" void fn_80338AD8(
    UnidentifiedNetworkSyncState* state, int machine, int machineCount);
extern "C" void fn_80338AE4(UnidentifiedNetworkSyncState* state);

#endif // UNCLASSIFIED_TU_80338898_H
