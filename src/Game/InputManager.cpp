#include "Game/NetworkMessageRegistry.h"
#include "Game/InputManager.h"
#include "Game/Sys/debug.h"
#include "Game/InputRouter.h"
#include "Game/NetworkInput.h"

#include "Game/NetworkSession.h"
#include "NL/nlMemory.h"
#include "NL/nlTicker.h"

NetworkInputManager* gInputManager;

static NetworkMessageFactory<NetMessageInput> sFactoryType0;
static NetworkMessageFactory<NetMessageInputBundle> sFactoryType1;
static NetworkMessageFactory<NetMessageAllInputs> sFactoryType8;
static NetworkMessageFactory<NetMessageAllInputsBundle> sFactoryType9;


bool IsNetworkOrRecordedGame()
{
    NetworkSessionControl& online = *g_pNetworkSessionBase;
    int mode = online.GetSessionMode();
    if (mode == 1 || mode == 2)
    {
        return true;
    }

    if (gNetworkInputRecording->mRecording != 0
        || gNetworkInputRecording->mPlaybackReady != 0)
    {
        return true;
    }
    return false;
}

void InitializeInputManager()
{
    NetworkInputManager* manager
        = (NetworkInputManager*)nlMalloc(
            sizeof(NetworkInputManager), 8, false);
    if (manager != 0)
    {
        manager->mFrameProvider = 0;
        manager->mEnabled = false;

        gNetworkMessageRegistry->RegisterFactory(0, &sFactoryType0);
        gNetworkMessageRegistry->RegisterFactory(1, &sFactoryType1);
        gNetworkMessageRegistry->RegisterFactory(8, &sFactoryType8);
        gNetworkMessageRegistry->RegisterFactory(9, &sFactoryType9);

        manager->mCongestionStartTicker = 0;
        manager->mLastCongestionMilliseconds = 0.0f;
        manager->mCongestionCount = 0;
        manager->mTotalCongestionMilliseconds = 0.0f;
        manager->mQueueOverflowReported = false;
    }
    gInputManager = manager;
}

void NetworkInputManager::Reset()
{
    mCongestionStartTicker = 0;
    mLastCongestionMilliseconds = 0.0f;
    mCongestionCount = 0;
    mTotalCongestionMilliseconds = 0.0f;
    mQueueOverflowReported = false;
}

void NetworkInputManager::SetFrameProvider(
    InputFrameProvider* frameProvider)
{
    mFrameProvider = frameProvider;
}

int NetworkInputManager::GetUpdateCount()
{
    InputRouter* router = GetInputRouter();
    int inputCount = router->GetUpdateCount();
    return inputCount + gNetworkInputRecording->GetNetworkInputPlaybackExtraUpdates();
}

void NetworkInputManager::CaptureInputs()
{
    if (gNetworkInputRecording->mPlaybackReady != 0)
    {
        return;
    }

    InputRouter* router = GetInputRouter();
    if (!router->mOutgoingQueueOverflowed)
    {
        if (!router->CanCaptureInput())
        {
            return;
        }

        NetworkPeer* peer
            = g_pNetworkSessionBase->GetLocalPeer();
        int numControllers = peer->mPlayerCount;
        for (int i = 0; i < numControllers; ++i)
        {
            (peer->GetNetworkPeerChannel(i))->CaptureNetworkPeerChannelInput();
        }
        router->OnInputCaptured();
    }
    else if (!mQueueOverflowReported)
    {
        tDebugPrintManager::Print(DC_NETWORK, "Overflowed Queues just occured\n");
        mFrameProvider->OnInputQueueOverflow();
        mQueueOverflowReported = true;
    }
}

bool NetworkInputManager::PrepareUpdate()
{
    InputRouter* router = GetInputRouter();
    if (gNetworkInputRecording->mPlaybackReady != 0)
    {
        if (router->ProcessPlaybackFrame() != 0)
        {
            return true;
        }
    }
    else if (!router->mOutgoingQueueOverflowed)
    {
        router->CheckCongestion();
        if (router->HasInput())
        {
            router->mStarvedForInput = false;
            if (mCongestionStartTicker != 0)
            {
                mLastCongestionMilliseconds
                    = nlGetTickerDifference(mCongestionStartTicker, nlGetTicker());
                mCongestionStartTicker = 0;
                ++mCongestionCount;
                tDebugPrintManager::Print(DC_NETWORK,
                    "Congested for %dth time for %f ms at game frame %d\n",
                    mCongestionCount, mLastCongestionMilliseconds,
                    mFrameProvider->GetFrame());
                mTotalCongestionMilliseconds += mLastCongestionMilliseconds;
            }
            router->OnInputReady();
            return true;
        }

        router->mStarvedForInput = true;
        mCongestionStartTicker = nlGetTicker();
    }
    else if (!mQueueOverflowReported)
    {
        tDebugPrintManager::Print(DC_NETWORK, "Overflowed Queues just occured\n");
        mFrameProvider->OnInputQueueOverflow();
        mQueueOverflowReported = true;
    }
    return false;
}
