#include "unclassified/tu_80338898.h"

#include "Game/DebugWriteCache.h"
#include "Game/TweakValue.h"
#include "NL/nlDebug.h"
#include "NL/nlMemory.h"
#include "unclassified/tu_80332770.h"

UnidentifiedNetworkSyncState* lbl_806E2168;
bool g_bBreakOnSyncError;

extern "C" void fn_80338898()
{
    lbl_806E2168 = new (8, false) UnidentifiedNetworkSyncState;
}

extern "C" void fn_80338900(
    UnidentifiedNetworkSyncState* state, bool resetCache)
{
    state->mValue0 = -1;
    state->mValue1 = -1;
    state->mValue2 = -1;
    state->mTriggered = false;
    state->mCaptureEnabled = true;
    state->mMachine = -1;
    state->mMachineCount = -1;

    if (resetCache)
    {
        state->mEnabled = true;
        state->mWriteCache = 0;
    }
    else if (state->mWriteCache != 0)
    {
        fn_80338CC4(state->mWriteCache);
    }
}

extern "C" DebugWriteCache* fn_80338950(
    UnidentifiedNetworkSyncState* state)
{
    return state->mWriteCache;
}

extern "C" void fn_80338958(
    UnidentifiedNetworkSyncState* state, int value)
{
    if (state->mCaptureEnabled)
    {
        if (state->mValue0 == -1)
        {
            state->mValue0 = value;
        }
        if (!state->mTriggered)
        {
            if (g_bBreakOnSyncError)
            {
                nlBreak();
            }
            lbl_806E2138->mFrameProvider->UnidentifiedVirtual10();
            state->mTriggered = true;
        }
    }
}

extern "C" void fn_803389D8(
    UnidentifiedNetworkSyncState* state, int value)
{
    if (state->mCaptureEnabled)
    {
        if (state->mValue1 == -1)
        {
            state->mValue1 = value;
        }
        if (!state->mTriggered)
        {
            if (g_bBreakOnSyncError)
            {
                nlBreak();
            }
            lbl_806E2138->mFrameProvider->UnidentifiedVirtual10();
            state->mTriggered = true;
        }
    }
}

extern "C" void fn_80338A58(
    UnidentifiedNetworkSyncState* state, int value)
{
    if (state->mCaptureEnabled)
    {
        if (state->mValue2 == -1)
        {
            state->mValue2 = value;
        }
        if (!state->mTriggered)
        {
            if (g_bBreakOnSyncError)
            {
                nlBreak();
            }
            lbl_806E2138->mFrameProvider->UnidentifiedVirtual10();
            state->mTriggered = true;
        }
    }
}

extern "C" void fn_80338AD8(
    UnidentifiedNetworkSyncState* state, int machine, int machineCount)
{
    state->mMachine = machine;
    state->mMachineCount = machineCount;
}

extern "C" void fn_80338AE4(UnidentifiedNetworkSyncState*)
{
}

static TweakValueBoolImpl_804F4538 sBreakOnSyncErrorTweak(
    "g_bBreakOnSyncError", "Network", &g_bBreakOnSyncError, true);
