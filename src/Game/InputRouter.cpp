#include "Game/NetworkEvents.h"
#include "Game/InputRouter.h"
#include "Game/NetworkDebug.h"
#include "Game/Sys/debug.h"
#include "Game/EventDispatcher.inl"
#include "Game/NetworkInput.h"
#include "Game/NetworkMessageRegistry.h"
#include "Game/NetworkSync.h"

#include <string.h>

#include "Game/MathHelpers.h"
#include "Game/TweakValue.h"
#include "Game/UnidentifiedStaticStorage.h"
#include "NL/gl/glFont.h"
#include "NL/nlDebugViews.h"
#include "NL/nlMath.h"
#include "NL/nlPrint.h"
#include "Game/TweakValue.inl"
#include "NL/nlFunction.inl"

int g_TransmitSyncDataEvery = 4;
int g_nTicksPerPacket = 2;
int g_nOverrideTickDelayTarget = -1;
float g_fCSInputQHighwaterMarkMin = 4.0f;
float g_fCSInputQHighwaterMarkDef = 4.0f;
float g_fCSInputQHighwaterMarkMax = 7.0f;
float g_fCSInputQHighwaterMarkInc = 0.8f;
float g_fCSInputQHighwaterMarkDec = 0.03f;
int g_nCSHostDelayMin = 1;
int g_nCSHostDelayMax = 10;
float fMinCongestionMultiplier = 2.0f;
float fDefCongestionMultiplier = 4.0f;
float fMaxCongestionMultiplier = 10.0f;
float fCongestionMultiplierIncrement = 0.3f;
float fCongestionMultiplierDecrement = 0.06f;
int g_nDumpSyncTextAt = -1;
nlColour g_SyncMismatchColour = { 0xFF, 0x00, 0x00, 0xFF };
nlColour g_SyncTextColour = { 0xFF, 0xFF, 0xFF, 0xFF };

InputRouter* gSimpleInputRouter;
NetworkInputRouter* gNetworkInputRouter;

static EventDispatcher sDetermDataDispatcher;
static UnidentifiedQueuedEvent<DetermDataEvent> sDetermDataEventQueue(
    &sDetermDataDispatcher, "DetermDataEventQueue", -1);

inline void FreeDetermDataEvent(DetermDataEvent* event);

void InitializeInputRouters()
{
    gSimpleInputRouter = new SimpleInputRouter;
    gNetworkInputRouter = new NetworkInputRouter;
}

InputRouter* GetInputRouter()
{
    if (g_pNetworkSessionBase->GetSessionMode() == 0)
    {
        return gSimpleInputRouter;
    }
    return gNetworkInputRouter;
}

InputRouter::~InputRouter()
{
}

void InputRouter::Reset(int)
{
    mSession = g_pNetworkSessionBase;

    int machineCount = mSession->GetNumMachines();
    for (int machine = 0; machine < machineCount; ++machine)
    {
        mSession->GetPeer(machine)->ResetNetworkPeerInputs();
    }

    for (int input = 0; input < 16; ++input)
    {
        memset(&mInputRecords[input], 0, sizeof(PackedDetInput));
        mInputStates[input] = 2;
    }

    for (int machine = 0; machine < 4; ++machine)
    {
        mRemapAngles[machine] = 0;
        mNetworkCRCs[machine] = 0;
        mRemoteTicks[machine] = 0;
        mRandomSeeds[machine] = 0;
    }

    mCurrentCRC = 0;
    mLastGameFrame = -1;
    m_OutgoingCustomDetermDataQ.mHead = 0;
    m_OutgoingCustomDetermDataQ.mCount = 0;

    sDetermDataDispatcher.Clear();
    sDetermDataDispatcher.FreeBlocks();

    sDetermDataEventQueue.RemoveAll();
    BasicSlotPool<DLListEntry<UnidentifiedListener<DetermDataEvent> > >*
        listenerPool = &sDetermDataEventQueue.mListeners.m_Allocator;
    listenerPool->FreeBlocks();

    SlotPoolBase::BaseFreeBlocks(
        &gDetermDataEventPool, sizeof(DetermDataEvent));

    mSyncMismatch = false;
    mSyncMismatchReported = false;
    mQueueOverflowed = false;
    mStarvedForInput = false;
}

void InputRouter::CheckSyncMismatch()
{
    bool mismatch = false;
    int gameFrame = gInputManager->mFrameProvider->GetFrame();
    u32 seed = GetNetworkRandomSeed();
    int machineCount = mSession->GetNumMachines();

    for (s8 machine = 0; machine < machineCount; ++machine)
    {
        if (mRemoteTicks[machine] != gameFrame)
        {
            if (gNetworkSyncState->mFrameMismatchFrame == -1)
            {
                tDebugPrintManager::Print(DC_NETWORK,
                    "NetworkSync ***PLAYBACK***: Network ticks don't match in playback from recording (Machine %d, Recorded Tick %d, Actual Tick %d\n",
                    machine, mRemoteTicks[machine], gameFrame);
            }
            gNetworkSyncState->OnFrameMismatch(gameFrame);
            mismatch = true;
        }

        if (mNetworkCRCs[machine] != mCurrentCRC)
        {
            if (gNetworkSyncState->mChecksumMismatchFrame == -1)
            {
                tDebugPrintManager::Print(DC_NETWORK,
                    "NetworkSync ***PLAYBACK***: Network CRCs don't match in playback from recording (Machine %d, Recorded CRC %x, Actual CRC %x, NetworkTick %d)\n",
                    machine, mNetworkCRCs[machine], mCurrentCRC, gameFrame);
            }
            gNetworkSyncState->OnChecksumMismatch(gameFrame);
            mismatch = true;
        }

        if (mRandomSeeds[machine] != seed)
        {
            if (gNetworkSyncState->mRandomSeedMismatchFrame == -1)
            {
                tDebugPrintManager::Print(DC_NETWORK,
                    "NetworkSync ***PLAYBACK***: Network Random seeds don't match in playback from recording (Machine %d, Recorded Seed %x, Actual Seed %x, NetworkTick %d)\n",
                    machine, mRandomSeeds[machine], seed, gameFrame);
            }
            gNetworkSyncState->OnRandomSeedMismatch(gameFrame);
            mismatch = true;
        }
    }

    if (mismatch && !mSyncMismatch)
    {
        mSyncMismatch = true;
    }
}

bool InputRouter::ProcessPlaybackFrame()
{
    int frame = gInputManager->mFrameProvider->GetFrame();
    if (mLastGameFrame != frame)
    {
        mCurrentCRC = gInputManager->mEnabled
            ? gInputManager->mFrameProvider->CalculateChecksum()
            : gInputManager->mFrameProvider->WriteSyncLog();
        mLastGameFrame = frame;
    }

    int machineCount = mSession->GetNumMachines();
    for (s8 machine = 0; machine < machineCount; ++machine)
    {
        NetworkPeer* peer = mSession->GetPeer(machine);
        int eventCount = 0;
        u32 dataSize = 0;
        if (!gNetworkInputRecording->ReadNetworkInputPacketHeader(machine,
                &mRemapAngles[machine], &mNetworkCRCs[machine],
                (u32*)&mRemoteTicks[machine], &mRandomSeeds[machine],
                (u32*)&eventCount, &dataSize))
        {
            return false;
        }

        for (int eventIndex = 0; eventIndex < eventCount; ++eventIndex)
        {
            DetermDataEvent* event = new DetermDataEvent();
            if (!gNetworkInputRecording->ReadNetworkInputEvent(event))
            {
                delete event;
                return false;
            }

            Function<DetermDataEvent*> disposer(FreeDetermDataEvent);
            sDetermDataEventQueue.Queue(event, disposer);
        }

        u8 data[300];
        if (!gNetworkInputRecording->ReadNetworkInputData(dataSize, data))
        {
            return false;
        }

        int playerCount = peer->mPlayerCount;
        for (s8 player = 0; player < playerCount; ++player)
        {
            NetworkPeerChannel* channel = peer->GetNetworkPeerChannel(player);
            s8 playerId = GetNetworkPlayerId(player, machine);
            if (!gNetworkInputRecording->ReadNetworkInputRecord(
                    player, &mInputRecords[playerId], &mInputStates[playerId]))
            {
                return false;
            }
            channel->ApplyNetworkPeerChannelInput(
                &mInputRecords[playerId], mRemapAngles[machine],
                mInputStates[playerId]);
        }
    }

    CheckSyncMismatch();
    return true;
}

void InputRouter::MarkSyncMismatchReported()
{
    if (!mSyncMismatchReported)
    {
        mSyncMismatchReported = true;
    }
}

void InputRouter::QueueDetermData(const void* data, u32 size)
{
    DetermDataEvent* event = new DetermDataEvent(data, size);

    if (!m_OutgoingCustomDetermDataQ.IsFull())
    {
        m_OutgoingCustomDetermDataQ.Push(event);
    }
    else
    {
        tDebugPrintManager::Print(DC_NETWORK, "m_OutgoingCustomDetermDataQ overflowed\n");
        mQueueOverflowed = true;
    }
}

void DispatchDetermDataEvents()
{
    sDetermDataDispatcher.Dispatch(true);
}

UnidentifiedQueuedEvent<DetermDataEvent>* GetDetermDataEventQueue()
{
    return &sDetermDataEventQueue;
}

SimpleInputRouter::~SimpleInputRouter()
{
}

void SimpleInputRouter::Reset(int resetQueues)
{
    InputRouter::Reset(resetQueues);
}

void SimpleInputRouter::OnInputCaptured()
{
    int frame = gInputManager->mFrameProvider->GetFrame();
    if (mLastGameFrame != frame)
    {
        if (IsNetworkOrRecordedGame())
        {
            mCurrentCRC = gInputManager->mEnabled
                ? gInputManager->mFrameProvider->CalculateChecksum()
                : gInputManager->mFrameProvider->WriteSyncLog();
        }
        else
        {
            mCurrentCRC = 0;
        }
        mLastGameFrame = frame;
    }
}

void SimpleInputRouter::OnInputReady()
{
    NetworkPeer* peer = mSession->GetLocalPeer();
    s8 machine = mSession->GetLocalMachineId();
    int playerCount = peer->mPlayerCount;

    mRemapAngles[machine]
        = peer->GetNetworkPeerChannel(0)->GetNetworkPeerChannelRemapAngle();

    int eventCount = m_OutgoingCustomDetermDataQ.GetCount();
    if (gNetworkInputRecording->mRecording)
    {
        int frame = gInputManager->mFrameProvider->GetFrame();
        NetworkInputRecording* recording = gNetworkInputRecording;
        u32 seed = GetNetworkRandomSeed();
        recording->WriteNetworkInputPacketHeader(machine,
            mRemapAngles[machine], mCurrentCRC, frame, seed, eventCount, 0);
    }

    for (int i = 0; i < eventCount; ++i)
    {
        DetermDataEvent* event = m_OutgoingCustomDetermDataQ.Pop();
        if (gNetworkInputRecording->mRecording)
        {
            gNetworkInputRecording->WriteNetworkInputEvent(event);
        }

        Function<DetermDataEvent*> disposer(FreeDetermDataEvent);
        sDetermDataEventQueue.Queue(event, disposer);
    }

    for (s8 player = 0; player < playerCount; ++player)
    {
        NetworkPeerChannel* channel = peer->GetNetworkPeerChannel(player);
        s8 playerId = GetNetworkPlayerId(player, machine);

        channel->PackNetworkPeerChannelInput(&mInputRecords[playerId]);
        mInputStates[playerId]
            = channel->GetNetworkPeerChannelConnectionStatus();
        const PackedDetInput* input = &mInputRecords[playerId];
        channel->ApplyNetworkPeerChannelInput(
            input, mRemapAngles[machine], mInputStates[playerId]);

        if (gNetworkInputRecording->mRecording)
        {
            gNetworkInputRecording->WriteNetworkInputRecord(
                player, input, mInputStates[playerId]);
        }
    }

    if (gNetworkInputRecording->mRecording)
    {
        gNetworkInputRecording->Flush();
    }
}

NetworkInputRouter::~NetworkInputRouter()
{
}

void NetworkInputRouter::Reset(int resetQueues)
{
    InputRouter::Reset(resetQueues);
    mCongested = false;
    mCongestionMultiplier = fDefCongestionMultiplier;
    mWasCongested = false;
    mCurrentMessage.Reset(true, true);
    mMessageHeld = 0;
    mBundledMessageCount = 0;

    for (int machine = 0; machine < 4; ++machine)
    {
        m_InputQueue[machine].mHead = 0;
        m_InputQueue[machine].mCount = 0;
    }
    mQueueCursor = 0;
    mQueueLimit = 4;
}

void NetworkInputRouter::CheckCongestion()
{
    mCongested = false;
    if (mQueueLimit > GetQueueCursor())
    {
        mCongested = false;
    }
    else
    {
        int machineCount = mSession->GetNumMachines();
        for (s8 machine = 0; machine < machineCount; ++machine)
        {
            if (m_InputQueue[machine].mCount <= 1)
            {
                mCongested = true;
            }
        }
    }
    if (mCongested)
    {
        mWasCongested = mCongested;
    }
}

bool NetworkInputRouter::HasInput()
{
    if (mQueueLimit > GetQueueCursor())
    {
        return true;
    }

    int machineCount = mSession->GetNumMachines();
    for (s8 machine = 0; machine < machineCount; ++machine)
    {
        if (m_InputQueue[machine].mCount == 0)
        {
            return false;
        }
    }
    return true;
}

void NetworkInputRouter::OnInputCaptured()
{
    int frame = gInputManager->mFrameProvider->GetFrame();
    if (mLastGameFrame != frame)
    {
        mCurrentCRC = gInputManager->mEnabled
            ? gInputManager->mFrameProvider->CalculateChecksum()
            : gInputManager->mFrameProvider->WriteSyncLog();
        mLastGameFrame = frame;
    }

    mCurrentMessage.Reset(false, !mMessageHeld);
    mMessageHeld = false;

    NetworkPeer* peer = mSession->GetLocalPeer();
    for (s8 player = 0; player < (int)peer->mPlayerCount; ++player)
    {
        NetworkPeerChannel* channel = peer->GetNetworkPeerChannel(player);
        PackedDetInput input;
        channel->PackNetworkPeerChannelInput(&input);
        mCurrentMessage.SetNetworkInputMessageRecord(player, &input);
        mCurrentMessage.SetNetworkInputMessageRemapAngle(
            channel->GetNetworkPeerChannelRemapAngle());
        mCurrentMessage.SetNetworkInputMessagePlayerState(
            player, channel->GetNetworkPeerChannelConnectionStatus());
    }

    frame = gInputManager->mFrameProvider->GetFrame();
    if (frame % g_TransmitSyncDataEvery == 0)
    {
        mCurrentMessage.SetNetworkInputMessageSyncData(
            mCurrentCRC, frame, GetNetworkRandomSeed());
    }

    while (m_OutgoingCustomDetermDataQ.GetCount() > 0)
    {
        DetermDataEvent* event = m_OutgoingCustomDetermDataQ.Pop();
        mCurrentMessage.AddNetworkInputMessageEvent(event);
        delete event;
    }

    mCurrentMessage.SetNetworkInputMessageCongested(mWasCongested);
    mWasCongested = false;

    if (mQueueLimit < GetQueueCursor() && (frame & 1) != 0)
    {
        mMessageHeld = true;
        --mQueueCursor;
        return;
    }

    switch (g_nTicksPerPacket)
    {
    case 1:
    {
        u8 buffer[200];
        int size = gNetworkMessageRegistry->Serialize(
            &mCurrentMessage, buffer, sizeof(buffer));
        int machines = mSession->GetNumMachines();
        for (s8 machine = 0; machine < machines; ++machine)
        {
            g_pNetworkSessionBase->Send(machine, buffer, size, true);
        }
        break;
    }
    case 2:
    {
        if (mBundledMessageCount == 0)
        {
            mBundledMessage.mMessage0.CopyFrom(&mCurrentMessage);
            ++mBundledMessageCount;
        }
        else if (mBundledMessageCount == 1)
        {
            mBundledMessage.mMessage1.CopyFrom(&mCurrentMessage);
            u8 buffer[400];
            int size = gNetworkMessageRegistry->Serialize(
                &mBundledMessage, buffer, sizeof(buffer));
            int machines = mSession->GetNumMachines();
            for (s8 machine = 0; machine < machines; ++machine)
            {
                g_pNetworkSessionBase->Send(machine, buffer, size, true);
            }
            mBundledMessageCount = 0;
        }
        break;
    }
    }
}

inline void NetworkInputRouter::RecordEmptyInputHeader(s8 machine, int frame)
{
    NetworkInputRecording* recording = gNetworkInputRecording;
    u32 seed = GetNetworkRandomSeed();
    recording->WriteNetworkInputPacketHeader(machine,
        mRemapAngles[machine], mCurrentCRC, frame, seed, 0, 0);
}

void NetworkInputRouter::OnInputReady()
{
    bool congested = false;

    if (mQueueLimit > GetQueueCursor())
    {
        ++mQueueCursor;
        int machineCount = mSession->GetNumMachines();
        for (s8 machine = 0; machine < machineCount; ++machine)
        {
            NetworkPeer* peer = mSession->GetPeer(machine);
            if (gNetworkInputRecording->mRecording)
            {
                RecordEmptyInputHeader(
                    machine, gInputManager->mFrameProvider->GetFrame());
            }

            for (s8 player = 0; player < (int)peer->mPlayerCount; ++player)
            {
                NetworkPeerChannel* channel
                    = peer->GetNetworkPeerChannel(player);
                s8 playerId = GetNetworkPlayerId(player, machine);
                channel->ApplyNetworkPeerChannelInput(
                    &mInputRecords[playerId], mRemapAngles[machine],
                    mInputStates[playerId]);

                if (gNetworkInputRecording->mRecording)
                {
                    gNetworkInputRecording->WriteNetworkInputRecord(
                        player, &mInputRecords[playerId],
                        mInputStates[playerId]);
                }
            }
        }
    }
    else
    {
        int machineCount = mSession->GetNumMachines();
        for (s8 machine = 0; machine < machineCount; ++machine)
        {
            NetworkPeer* peer = mSession->GetPeer(machine);
            NetMessageInput* message = &m_InputQueue[machine].Pop();

            message->GetNetworkInputMessageRemapAngle(
                &mRemapAngles[machine]);
            if ((message->mFlags & 4) != 0)
            {
                mNetworkCRCs[machine] = message->mChecksum;
                mRemoteTicks[machine] = message->mFrame;
                mRandomSeeds[machine] = message->mRandomSeed;
            }
            if ((message->mFlags & 8) != 0)
            {
                congested = true;
            }

            int eventCount = message->mEventCount;
            u8 serializedData[300];
            NetworkMessageSerializer serializer(
                1, serializedData, sizeof(serializedData));
            message->Serialize(&serializer);
            u32 serializedLength = serializer.GetLength();

            if (gNetworkInputRecording->mRecording)
            {
                int frame = gInputManager->mFrameProvider->GetFrame();
                NetworkInputRecording* recording = gNetworkInputRecording;
                u32 seed = GetNetworkRandomSeed();
                recording->WriteNetworkInputPacketHeader(machine,
                    mRemapAngles[machine], mCurrentCRC, frame, seed,
                    eventCount, serializedLength);
            }

            for (int eventIndex = 0; eventIndex < eventCount; ++eventIndex)
            {
                DetermDataEvent* event = new DetermDataEvent(
                    message->GetNetworkInputMessageEvent(eventIndex));
                if (gNetworkInputRecording->mRecording)
                {
                    gNetworkInputRecording->WriteNetworkInputEvent(event);
                }

                Function<DetermDataEvent*> disposer(FreeDetermDataEvent);
                sDetermDataEventQueue.Queue(event, disposer);
            }

            if (gNetworkInputRecording->mRecording)
            {
                gNetworkInputRecording->WriteData(
                    serializer.mBuffer, serializedLength);
            }

            int playerCount = peer->mPlayerCount;
            for (s8 player = 0; player < playerCount; ++player)
            {
                NetworkPeerChannel* channel
                    = peer->GetNetworkPeerChannel(player);
                s8 playerId = GetNetworkPlayerId(player, machine);
                message->ApplyNetworkInputMessageRecord(
                    player, &mInputRecords[playerId]);
                mInputStates[playerId]
                    = message->GetNetworkInputMessagePlayerState(player);
                channel->ApplyNetworkPeerChannelInput(
                    &mInputRecords[playerId], mRemapAngles[machine],
                    mInputStates[playerId]);

                if (gNetworkInputRecording->mRecording)
                {
                    gNetworkInputRecording->WriteNetworkInputRecord(
                        player, &mInputRecords[playerId],
                        mInputStates[playerId]);
                }
            }
        }

        if (congested)
        {
            mCongestionMultiplier = nlMinEquals(fMaxCongestionMultiplier,
                mCongestionMultiplier + fCongestionMultiplierIncrement);
        }
        else
        {
            mCongestionMultiplier = nlMaxEquals(fMinCongestionMultiplier,
                mCongestionMultiplier - fCongestionMultiplierDecrement);
        }

        if (g_nOverrideTickDelayTarget >= 0)
        {
            mQueueLimit = g_nOverrideTickDelayTarget;
        }
        else
        {
            u32 queueLimit = (u32)mCongestionMultiplier;
            mQueueLimit = queueLimit < 20 ? queueLimit : 20;
            if (mQueueLimit < 1)
            {
                mQueueLimit = 1;
            }
        }
    }

    if (gNetworkInputRecording->mRecording)
    {
        gNetworkInputRecording->Flush();
    }
    CheckPeerSynchronization();
}

void NetworkInputRouter::CheckPeerSynchronization()
{
    bool mismatch = false;
    s8 localMachine = mSession->GetLocalMachineId();
    int machineCount = mSession->GetNumMachines();

    for (s8 machine = 0; machine < machineCount; ++machine)
    {
        if (machine == localMachine)
        {
            continue;
        }

        if (mRemoteTicks[localMachine] != mRemoteTicks[machine])
        {
            if (gNetworkSyncState->mFrameMismatchFrame == -1)
            {
                tDebugPrintManager::Print(DC_NETWORK,
                    "NetworkSync:Network ticks don't match (Machine %d, Tick %d)<-->(Machine %d, Tick %d) at frame %d\n",
                    localMachine, mRemoteTicks[localMachine], machine,
                    mRemoteTicks[machine],
                    gInputManager->mFrameProvider->GetFrame());
            }
            gNetworkSyncState->OnFrameMismatch(mRemoteTicks[localMachine]);
            mismatch = true;
        }

        if (mNetworkCRCs[localMachine] != mNetworkCRCs[machine])
        {
            if (gNetworkSyncState->mChecksumMismatchFrame == -1)
            {
                tDebugPrintManager::Print(DC_NETWORK,
                    "NetworkSync:Network CRCs don't match (Machine %d, CRC %x)<-->(Machine %d, CRC %x) at frame %d\n",
                    localMachine, mNetworkCRCs[localMachine], machine,
                    mNetworkCRCs[machine],
                    gInputManager->mFrameProvider->GetFrame());
            }
            gNetworkSyncState->OnChecksumMismatch(mRemoteTicks[localMachine]);
            mismatch = true;
        }

        if (mRandomSeeds[localMachine] != mRandomSeeds[machine])
        {
            if (gNetworkSyncState->mRandomSeedMismatchFrame == -1)
            {
                tDebugPrintManager::Print(DC_NETWORK,
                    "NetworkSync:Network Random seeds don't match (Machine %d, Seed %x)<-->(Machine %d, Seed %x) at frame %d\n",
                    localMachine, mRandomSeeds[localMachine], machine,
                    mRandomSeeds[machine],
                    gInputManager->mFrameProvider->GetFrame());
            }
            gNetworkSyncState->OnRandomSeedMismatch(
                mRemoteTicks[localMachine]);
            mismatch = true;
        }
    }

    if (mismatch)
    {
        MarkSyncMismatchReported();
    }
}

void NetworkInputRouter::ReceiveInput(
    s8 machine, NetMessageInput* message)
{
    if (!m_InputQueue[machine].IsFull())
    {
        m_InputQueue[machine].PushSlot()->CopyFrom(message);
    }
    else
    {
        tDebugPrintManager::Print(DC_NETWORK, "m_InputQueue[%d] overflowed\n", machine);
        mQueueOverflowed = true;
    }
}

void NetworkInputRouter::DebugDraw(int column, int* row)
{
    int machineCount = mSession->GetNumMachines();
    if (g_bDisplayNetworkVerbose)
    {
        glFontPrintf(GetDebugFontView(), column, (*row)++, "I am %d",
            mSession->GetLocalMachineId());
    }

    char output[100];
    for (int machine = 0; machine < machineCount; ++machine)
    {
        int inputCount = m_InputQueue[machine].GetCount();
        char queueText[100];
        int maxTextLength = nlMin(inputCount, 99);
        int queueTextLength = 0;
        for (; queueTextLength < maxTextLength; ++queueTextLength)
        {
            queueText[queueTextLength] = '*';
        }
        queueText[queueTextLength] = '\0';

        nlSNPrintf(output, sizeof(output), "M%d: %d %s\n",
            machine, inputCount, queueText);
        glFontPrintf(GetDebugFontView(), column, *row,
            inputCount > 0 ? g_SyncTextColour : g_SyncMismatchColour,
            output);
        ++*row;
    }

    if (mStarvedForInput)
    {
        glFontPrintf(GetDebugFontView(), column, (*row)++,
            g_SyncMismatchColour, "StarvedForInput");
    }
}

NetMessageAllInputsBundle::~NetMessageAllInputsBundle()
{
}

NetworkInputMessageQueue::NetworkInputMessageQueue()
    : CircularQueueBase<NetMessageInput>(mStorage, 60)
{
}

NetworkInputMessageQueue::~NetworkInputMessageQueue()
{
}

static TweakIntBinding sTransmitSyncDataEvery(
    "g_TransmitSyncDataEvery", "Network/InputMan", &g_TransmitSyncDataEvery, true);
static TweakIntBinding sTicksPerPacket(
    "g_nTicksPerPacket", "Network/InputMan", &g_nTicksPerPacket, true);
static TweakFloatBinding sInputQHighwaterMarkMin(
    "g_fCSInputQHighwaterMarkMin", "Network/InputMan",
    &g_fCSInputQHighwaterMarkMin, true);
static TweakFloatBinding sInputQHighwaterMarkDef(
    "g_fCSInputQHighwaterMarkDef", "Network/InputMan",
    &g_fCSInputQHighwaterMarkDef, true);
static TweakFloatBinding sInputQHighwaterMarkMax(
    "g_fCSInputQHighwaterMarkMax", "Network/InputMan",
    &g_fCSInputQHighwaterMarkMax, true);
static TweakFloatBinding sInputQHighwaterMarkInc(
    "g_fCSInputQHighwaterMarkInc", "Network/InputMan",
    &g_fCSInputQHighwaterMarkInc, true);
static TweakFloatBinding sInputQHighwaterMarkDec(
    "g_fCSInputQHighwaterMarkDec", "Network/InputMan",
    &g_fCSInputQHighwaterMarkDec, true);
static TweakIntBinding sHostDelayMin(
    "g_nCSHostDelayMin", "Network/InputMan", &g_nCSHostDelayMin, true);
static TweakIntBinding sHostDelayMax(
    "g_nCSHostDelayMax", "Network/InputMan", &g_nCSHostDelayMax, true);
static TweakFloatBinding sCongestionMultiplierMin(
    "fMinCongestionMultiplier", "Network/InputMan",
    &fMinCongestionMultiplier, true);
static TweakFloatBinding sCongestionMultiplierDef(
    "fDefCongestionMultiplier", "Network/InputMan",
    &fDefCongestionMultiplier, true);
static TweakFloatBinding sCongestionMultiplierMax(
    "fMaxCongestionMultiplier", "Network/InputMan",
    &fMaxCongestionMultiplier, true);
static TweakFloatBinding sCongestionMultiplierIncrement(
    "fCongestionMultiplierIncrement", "Network/InputMan",
    &fCongestionMultiplierIncrement, true);
static TweakFloatBinding sCongestionMultiplierDecrement(
    "fCongestionMultiplierDecrement", "Network/InputMan",
    &fCongestionMultiplierDecrement, true);
static TweakIntBinding sOverrideTickDelayTarget(
    "g_nOverrideTickDelayTarget", "Network/InputMan",
    &g_nOverrideTickDelayTarget, true);
static TweakIntBinding sDumpSyncTextAt(
    "g_nDumpSyncTextAt", "Network", &g_nDumpSyncTextAt, true);

typedef char VerifyDetermDataEventSize[(sizeof(DetermDataEvent) == 0x24) ? 1 : -1];
typedef char VerifyNetworkMessageType0Size[
    (sizeof(NetMessageInput) == 0xF0) ? 1 : -1];
typedef char VerifyNetworkMessageType8Size[
    (sizeof(NetMessageAllInputs) == 0x3D8) ? 1 : -1];
typedef char VerifyInputRouterSize[
    (sizeof(InputRouter) == 0x198) ? 1 : -1];
typedef char VerifyInputQueueSize[
    (sizeof(NetworkInputMessageQueue) == 0x3850) ? 1 : -1];
typedef char VerifyNetworkInputRouterSize[
    (sizeof(NetworkInputRouter) == 0xE5C8) ? 1 : -1];

#include "Game/InputRouter.inl"
#include "NL/nlBind_impl.h"
