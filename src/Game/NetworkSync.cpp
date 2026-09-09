#include "Game/NetworkSync.h"

#include "Game/DebugWriteCache.h"
#include "Game/TweakValue.h"
#include "NL/nlDebug.h"
#include "NL/nlMemory.h"
#include "Game/InputManager.h"

NetworkSyncState* gNetworkSyncState;
bool g_bBreakOnSyncError;

void InitializeNetworkSyncState()
{
    gNetworkSyncState = new (8, false) NetworkSyncState;
}

void NetworkSyncState::Reset(bool resetCache)
{
    mFrameMismatchFrame = -1;
    mChecksumMismatchFrame = -1;
    mRandomSeedMismatchFrame = -1;
    mTriggered = false;
    mCaptureEnabled = true;
    mMachine = -1;
    mMachineCount = -1;

    if (resetCache)
    {
        mEnabled = true;
        mWriteCache = 0;
    }
    else if (mWriteCache != 0)
    {
        mWriteCache->Reset();
    }
}

DebugWriteCache* NetworkSyncState::GetWriteCache()
{
    return mWriteCache;
}

void NetworkSyncState::OnFrameMismatch(int value)
{
    if (mCaptureEnabled)
    {
        if (mFrameMismatchFrame == -1)
        {
            mFrameMismatchFrame = value;
        }
        if (!mTriggered)
        {
            if (g_bBreakOnSyncError)
            {
                nlBreak();
            }
            gInputManager->mFrameProvider->OnSyncError();
            mTriggered = true;
        }
    }
}

void NetworkSyncState::OnChecksumMismatch(int value)
{
    if (mCaptureEnabled)
    {
        if (mChecksumMismatchFrame == -1)
        {
            mChecksumMismatchFrame = value;
        }
        if (!mTriggered)
        {
            if (g_bBreakOnSyncError)
            {
                nlBreak();
            }
            gInputManager->mFrameProvider->OnSyncError();
            mTriggered = true;
        }
    }
}

void NetworkSyncState::OnRandomSeedMismatch(int value)
{
    if (mCaptureEnabled)
    {
        if (mRandomSeedMismatchFrame == -1)
        {
            mRandomSeedMismatchFrame = value;
        }
        if (!mTriggered)
        {
            if (g_bBreakOnSyncError)
            {
                nlBreak();
            }
            gInputManager->mFrameProvider->OnSyncError();
            mTriggered = true;
        }
    }
}

void NetworkSyncState::SetMachineInfo(int machine, int machineCount)
{
    mMachine = machine;
    mMachineCount = machineCount;
}

void NetworkSyncState::DebugDraw()
{
}

static TweakBoolBinding sBreakOnSyncErrorTweak(
    "g_bBreakOnSyncError", "Network", &g_bBreakOnSyncError, true);
