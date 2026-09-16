#include "Game/NetworkInput.h"
#include "Game/NetworkDiagnostics.h"

#include <string.h>

#include "Game/NetworkSession.h"
#include "Game/TweakValue.h"
#include "NL/globalpad.h"
#include "NL/nlDebugFile.h"
#include "NL/nlFile.h"
#include "NL/nlMemory.h"
#include "NL/nlPrint.h"
#include "NL/nlString.h"
#include "Game/InputRouter.h"

struct NetworkRecordedFrameHeader
{
    u32 mChecksum;
    u32 mFrame;
    u32 mRandomSeed;
    u16 mTick;
    u16 mEventCount;
    u32 mValue;
};

struct NetworkRecordingHeader
{
    int mType;
    u32 mConfigSize;
    u32 mRandomSeed;
    int mLocalMachine;
    int mMachineCount;
    int mPlayerCounts[4];
};

int g_numPacketPlaybackTurbo;
NetworkInputRecording* gNetworkInputRecording;

static TweakIntBinding sPacketPlaybackTurboTweak(
    "g_numPacketPlaybackTurbo", "Network", &g_numPacketPlaybackTurbo);

NetworkPeerChannel* NetworkPeer::GetNetworkPeerChannel(int channel)
{
    return &mChannels[channel];
}

s8 NetworkPeer::GetNetworkPeerMachineId()
{
    return mMachineId;
}

NetworkPeer::NetworkPeer()
    : mMachineId(-1)
    , mPlayerCount(0)
{
}

void NetworkPeer::ResetNetworkPeerInputs()
{
    for (int channel = 0; channel < (int)mPlayerCount; ++channel)
    {
        NetworkPeerChannel* entry = &mChannels[channel];
        for (int input = 0; input < 4; ++input)
        {
            new (&entry->mInputs[input]) DetInput;
        }
        entry->mInputs[0].m_pMyUser = entry;
        entry->mInputs[1].m_pMyUser = entry;
        entry->mInputs[2].m_pMyUser = entry;
        entry->mInputs[3].m_pMyUser = entry;
        entry->mInputs[1].m_pPrevInput = &entry->mInputs[0];
        entry->mInputs[3].m_pPrevInput = &entry->mInputs[2];
    }
}

NetworkPeerChannel::NetworkPeerChannel()
    : mPeer(0)
    , mChannelIndex(-1)
    , mGlobalPadIndex(0)
    , mUnidentified00C(false)
{
    for (int input = 0; input < 4; ++input)
    {
        new (&mInputs[input]) DetInput;
    }
    mInputs[0].m_pMyUser = this;
    mInputs[1].m_pMyUser = this;
    mInputs[2].m_pMyUser = this;
    mInputs[3].m_pMyUser = this;
    mInputs[1].m_pPrevInput = &mInputs[0];
    mInputs[3].m_pPrevInput = &mInputs[2];
}

void NetworkPeerChannel::Initialize(NetworkPeer* peer, s8 channelIndex, int globalPadIndex)
{
    mPeer = peer;
    mChannelIndex = channelIndex;
    mGlobalPadIndex = globalPadIndex;
    mUnidentified00C = false;
}

DetInput* NetworkPeerChannel::GetNetworkPeerChannelInput()
{
    return &mInputs[1];
}

s8 NetworkPeerChannel::GetNetworkPeerChannelId()
{
    return mPeer->mMachineId * 4 + mChannelIndex;
}

cGlobalPad* NetworkPeerChannel::GetLocalChannelPad()
{
    if (mGlobalPadIndex == -1)
    {
        return 0;
    }
    cGlobalPad* pad;
    if (mPeer == g_pNetworkSessionBase->GetLocalPeer())
        pad = g_pPadManager->GetPad(mGlobalPadIndex);
    else
        pad = 0;
    return pad;
}

void NetworkPeerChannel::CaptureNetworkPeerChannelInput()
{
    mInputs[2].CopyState(mInputs[3]);
    cGlobalPad* pad = this->GetLocalChannelPad();
    if (pad != 0 && !gInputManager->mFrameProvider->IsInPauseMenu())
    {
        mInputs[3].ReadFromPad(pad);
    }
    else
    {
        new (&mInputs[3]) DetInput;
    }
    mInputs[3].m_aRemapAngle
        = gInputManager->mFrameProvider->GetInputRemapAngle();
}

void NetworkPeerChannel::ApplyNetworkPeerChannelInput(PackedDetInput* record, u16 tick, u8 connected)
{
    mInputs[0].CopyState(mInputs[1]);
    UnpackDetInput(record, &mInputs[1]);
    mInputs[1].m_aRemapAngle = tick;
    mInputs[1].m_nConnected = connected;
    mInputs[1].UpdatePolarAnalog();
    mInputs[1].UpdateButtonStateTicks();
}

void NetworkPeerChannel::PackNetworkPeerChannelInput(PackedDetInput* record)
{
    PackDetInput(record, &mInputs[3]);
}

u16 NetworkPeerChannel::GetNetworkPeerChannelRemapAngle()
{
    return mInputs[3].m_aRemapAngle;
}

u8 NetworkPeerChannel::GetNetworkPeerChannelConnectionStatus()
{
    return mInputs[3].GetConnectionStatus();
}

s8 GetNetworkPlayerId(s8 player, s8 machine)
{
    return machine * 4 + player;
}

NetMessageInput::NetMessageInput()
{
    mUnidentified00C = 0;
    mUnidentified010 = 0;
    mUnidentified014 = 0;
    mUnidentified00A = 0;
    memset(mUnidentified018, 0, sizeof(mUnidentified018));
    mUnidentified05C = 0;
    mUnidentified008 = 0;
    mUnidentified009 = 0;
    memset(mUnidentified01C, 0, sizeof(mUnidentified01C));
}

void NetMessageInput::Reset(bool clearInputs, bool clearHeader)
{
    mUnidentified00C = 0;
    mUnidentified010 = 0;
    mUnidentified014 = 0;
    mUnidentified00A = 0;
    if (clearHeader)
    {
        memset(mUnidentified018, 0,
            sizeof(mUnidentified018));
        mUnidentified05C = 0;
        mUnidentified008 = 0;
    }
    else
    {
        mUnidentified008 &= 1;
    }
    if (clearInputs)
    {
        mUnidentified009 = 0;
        memset(mUnidentified01C, 0,
            sizeof(mUnidentified01C));
    }
}

void NetMessageInput::CopyFrom(const NetMessageInput* source)
{
    mUnidentified008 = source->mUnidentified008;
    mUnidentified009 = source->mUnidentified009;
    mUnidentified00A = source->mUnidentified00A;
    mUnidentified00C = source->mUnidentified00C;
    mUnidentified010 = source->mUnidentified010;
    mUnidentified014 = source->mUnidentified014;
    mUnidentified05C = source->mUnidentified05C;
    memcpy(mUnidentified018, source->mUnidentified018, 4);
    memcpy(mUnidentified01C, source->mUnidentified01C, 0x40);
    memcpy(mDetermData, source->mDetermData, 0x90);
}

void NetMessageInput::SetNetworkInputMessagePlayerState(s8 player, u8 state)
{
    u8 shift = (player & 0x7F) * 2;
    u32 mask = 3 << shift;
    u8 inverse = ~mask;
    u8 value = mUnidentified00A & inverse;
    mUnidentified00A = value | (state << shift);
}

u8 NetMessageInput::GetNetworkInputMessagePlayerState(s8 player)
{
    return (mUnidentified00A >> ((player & 0x7F) * 2)) & 3;
}

void NetMessageInput::SetNetworkInputMessageRecord(s8 player, const PackedDetInput* record)
{
    const u8* source = (const u8*)record;
    u8* previous = mUnidentified01C[player].mData;
    u8& changes = mUnidentified018[player];

    if (*(const u16*)(source + 0) != *(u16*)(previous + 0))
        changes |= 2;
    if ((s8)source[12] != (s8)previous[12]
        || (s8)source[13] != (s8)previous[13])
        changes |= 4;
    if ((s8)source[14] != (s8)previous[14]
        || (s8)source[15] != (s8)previous[15])
        changes |= 8;
    if ((s8)source[3] != (s8)previous[3]
        || (s8)source[4] != (s8)previous[4]
        || (s8)source[5] != (s8)previous[5])
        changes |= 0x10;
    if ((s8)source[6] != (s8)previous[6]
        || (s8)source[7] != (s8)previous[7]
        || (s8)source[8] != (s8)previous[8])
        changes |= 0x20;

    if (changes != 0)
        mUnidentified008 |= 0x10 << player;
    memcpy(previous, source, 0x10);
}

void NetMessageInput::ApplyNetworkInputMessageRecord(s8 player, PackedDetInput* record)
{
    u8* output = (u8*)record;
    const u8* previous = mUnidentified01C[player].mData;
    u8 changes = mUnidentified018[player];
    if (changes & 2)
        *(u16*)(output + 0) = *(const u16*)(previous + 0);
    if (changes & 4)
    {
        output[12] = previous[12];
        output[13] = previous[13];
    }
    if (changes & 8)
    {
        output[14] = previous[14];
        output[15] = previous[15];
    }
    if (changes & 0x10)
    {
        output[3] = previous[3];
        output[4] = previous[4];
        output[5] = previous[5];
    }
    if (changes & 0x20)
    {
        output[6] = previous[6];
        output[7] = previous[7];
        output[8] = previous[8];
    }
}

void NetMessageInput::SetNetworkInputMessageRemapAngle(u16 tick)
{
    u8 value = tick >> 8;
    if (mUnidentified009 != value)
    {
        mUnidentified009 = value;
        mUnidentified008 |= 1;
    }
}

void NetMessageInput::GetNetworkInputMessageRemapAngle(u16* tick)
{
    if (mUnidentified008 & 1)
        *tick = mUnidentified009 << 8;
}

void NetMessageInput::SetNetworkInputMessageSyncData(u32 checksum, u32 frame, u32 randomSeed)
{
    mUnidentified00C = checksum;
    mUnidentified010 = frame;
    mUnidentified014 = randomSeed;
    mUnidentified008 |= 4;
}

void NetMessageInput::SetNetworkInputMessageCongested(bool congested)
{
    if (congested)
        mUnidentified008 |= 8;
    else
        mUnidentified008 &= ~8;
}

void NetMessageInput::AddNetworkInputMessageEvent(const DetermDataEvent* event)
{
    if (mUnidentified05C < 4)
    {
        mDetermData[mUnidentified05C] = *event;
        ++mUnidentified05C;
    }
}

DetermDataEvent* NetMessageInput::GetNetworkInputMessageEvent(int index)
{
    return &mDetermData[index];
}

void NetMessageInput::Serialize(
    NetworkMessageSerializer* serializer)
{
    serializer->Transfer(&mUnidentified008, 1);
    serializer->Transfer(&mUnidentified00A, 1);
    if (mUnidentified008 & 1)
        serializer->Transfer(&mUnidentified009, 1);
    if (mUnidentified008 & 4)
    {
        serializer->Transfer(&mUnidentified00C, 4);
        serializer->Transfer(&mUnidentified010, 4);
        serializer->Transfer(&mUnidentified014, 4);
    }
    else if (mUnidentified008 & 2)
    {
        serializer->Transfer(&mUnidentified010, 4);
    }
    serializer->Transfer(&mUnidentified05C, 1);
    for (int i = 0; i < mUnidentified05C; ++i)
        mDetermData[i].Serialize(serializer);
    for (s8 i = 0; i < 4; ++i)
    {
        if (mUnidentified008 & (0x10 << i))
            serializer->Transfer(&mUnidentified018[i], 1);
    }
    for (s8 i = 0; i < 4; ++i)
    {
        u8* data = mUnidentified01C[i].mData;
        u8 changes = mUnidentified018[i];
        if (changes & 2)
            serializer->Transfer(data + 0, 2);
        if (changes & 4)
        {
            serializer->Transfer(data + 12, 1);
            serializer->Transfer(data + 13, 1);
        }
        if (changes & 8)
        {
            serializer->Transfer(data + 14, 1);
            serializer->Transfer(data + 15, 1);
        }
        if (changes & 0x10)
            serializer->Transfer(data + 3, 3);
        if (changes & 0x20)
            serializer->Transfer(data + 6, 3);
    }
}

void NetMessageInputBundle::Serialize(
    NetworkMessageSerializer* serializer)
{
    mMessage0.Serialize(serializer);
    mMessage1.Serialize(serializer);
}

int NetMessageInputBundle::GetType()
{
    return 1;
}

int NetMessageInput::GetType()
{
    return 0;
}

NetMessageAllInputs::NetMessageAllInputs()
{
    mUnidentified008[0] = 0;
    memset(mPadding3CC, 0, sizeof(mPadding3CC));
}

void NetMessageAllInputs::Serialize(
    NetworkMessageSerializer* serializer)
{
    serializer->Transfer(&mUnidentified008[0], 1);
    for (int i = 0; i < 4; ++i)
    {
        if (mUnidentified008[0] & (1 << i))
            mMessages[i].Serialize(serializer);
    }
    if (mUnidentified008[0] & 0x80)
    {
        serializer->Transfer(mPadding3CC + 0, 4);
        serializer->Transfer(mPadding3CC + 4, 4);
        serializer->Transfer(mPadding3CC + 8, 4);
    }
}

void NetMessageAllInputsBundle::Serialize(
    NetworkMessageSerializer* serializer)
{
    mMessage0.Serialize(serializer);
    mMessage1.Serialize(serializer);
}

int NetMessageAllInputsBundle::GetType()
{
    return 9;
}

int NetMessageAllInputs::GetType()
{
    return 8;
}

void InitializeNetworkInputRecording()
{
    NetworkInputRecording* state
        = (NetworkInputRecording*)nlMalloc(
            sizeof(NetworkInputRecording), 8, false);
    if (state != 0)
    {
        nlBufferedWriterInitialize(&state->mWriter);
        nlAsyncFileBufferInitialize(&state->mReader);
        state->mConfigSize = 0;
        state->mConfig = 0;
        state->mRandomSeed = 0;
        state->mMachineCount = 0;
        state->mLocalMachine = 0;
        state->mPlayerCounts[0] = 0;
        state->mPlayerCounts[1] = 0;
        state->mPlayerCounts[2] = 0;
        state->mPlayerCounts[3] = 0;
        state->Reset(true);
    }
    gNetworkInputRecording = state;
}

void NetworkInputRecording::Reset(bool constructing)
{
    if (constructing)
    {
        mUnidentified02 = 0;
        mRecordingEnabled = false;
        mRecording = false;
        mPlaybackEnabled = false;
        mPlaybackReady = false;
        mFileName[0] = '\0';
        mDebugFile = 0;
        mFile = 0;
    }
    else
    {
        nlBufferedWriterFinish(&mWriter);
        if (nlDebugFileIsValid(mDebugFile))
        {
            nlCloseFileDebug(mDebugFile);
            mDebugFile = 0;
        }
        nlAsyncFileBufferFinish(&mReader);
        if (mFile != 0)
        {
            nlClose((nlFile*)mFile);
            mFile = 0;
        }
        if (mConfig != 0)
        {
            nlFree(mConfig);
            mConfig = 0;
        }
        if (mRecordingEnabled)
            mFileName[0] = '\0';
        mRecording = false;
        mPlaybackReady = false;
        mPlaybackEnabled = false;
    }

    mConfigSize = 0;
    mConfig = 0;
    mRandomSeed = 0;
    mMachineCount = 0;
    mLocalMachine = 0;
    mPlayerCounts[0] = 0;
    mPlayerCounts[1] = 0;
    mPlayerCounts[2] = 0;
    mPlayerCounts[3] = 0;
}

void NetworkInputRecording::StartNetworkInputRecording(s8 localMachine, int machineCount, u32 randomSeed, const void* config, int configSize)
{
    mRecording = true;
    if ((s8)mFileName[0] == 0)
    {
        char suffix[100];
        FormatNetworkTimestamp(suffix, sizeof(suffix), 0);
        nlSNPrintf(mFileName, sizeof(mFileName),
            "netpackrec_%s__%d_%d.bin", suffix, localMachine, machineCount);
    }

    char path[256];
    nlStrNCpy(path, "GameLog/", sizeof(path));
    nlStrNCat(path, path, mFileName, sizeof(path));
    mDebugFile = nlOpenFileDebug(path, true, false);
    nlBufferedWriterAttach(&mWriter, mDebugFile,
        mUnidentified02, 2000, 1800);

    NetworkRecordingHeader header;
    header.mType = 13;
    header.mConfigSize = configSize;
    header.mRandomSeed = randomSeed;
    header.mLocalMachine = localMachine;
    header.mMachineCount = machineCount;
    for (int machine = 0; machine < 4; ++machine)
    {
        if (machine < machineCount)
            header.mPlayerCounts[machine]
                = g_pNetworkSessionBase->GetPeer((s8)machine)->mPlayerCount;
        else
            header.mPlayerCounts[machine] = 0;
    }
    nlBufferedWriterWrite(&mWriter, &header, sizeof(header));
    nlBufferedWriterWrite(&mWriter, config, configSize);
    nlBufferedWriterFlushIfNeeded(&mWriter);
}

bool NetworkInputRecording::ReadNetworkInputRecordingHeader()
{
    NetworkRecordingHeader header;
    nlAsyncFileBufferRead(&mReader, &header, sizeof(header));
    if (header.mType != 13)
        return false;

    mRandomSeed = header.mRandomSeed;
    mMachineCount = header.mMachineCount;
    mLocalMachine = header.mLocalMachine;
    for (int machine = 0; machine < 4; ++machine)
        mPlayerCounts[machine] = header.mPlayerCounts[machine];
    mConfigSize = header.mConfigSize;
    mConfig = nlMalloc(header.mConfigSize, 8, false);
    nlAsyncFileBufferRead(&mReader, mConfig, header.mConfigSize);
    mPlaybackReady = true;
    return true;
}

int NetworkInputRecording::GetNetworkInputPlaybackExtraUpdates()
{
    return mPlaybackReady ? g_numPacketPlaybackTurbo : 0;
}

void NetworkInputRecording::WriteNetworkInputPacketHeader(s8 machine, u16 tick, u32 checksum, u32 frame, u32 randomSeed, u16 eventCount, u32 value)
{
    NetworkRecordedFrameHeader header;
    header.mChecksum = checksum;
    header.mFrame = frame;
    header.mRandomSeed = randomSeed;
    header.mTick = tick;
    header.mEventCount = eventCount;
    header.mValue = value;
    nlBufferedWriterWrite(&mWriter, &header, sizeof(header));
}

void NetworkInputRecording::WriteNetworkInputEvent(const DetermDataEvent* event)
{
    nlBufferedWriterWrite(&mWriter, &event->mSize, 1);
    nlBufferedWriterWrite(&mWriter, event->mData, event->mSize);
}

void NetworkInputRecording::WriteData(const void* data, int size)
{
    if (size > 0)
        nlBufferedWriterWrite(&mWriter, data, size);
}

void NetworkInputRecording::WriteNetworkInputRecord(s8 machine, const PackedDetInput* record, u8 connected)
{
    struct RecordedInput
    {
        u8 mRecord[0x10];
        u8 mConnected;
        u8 mPadding[3];
    } input;
    memset(&input, 0, sizeof(input));
    memcpy(input.mRecord, record, sizeof(input.mRecord));
    input.mConnected = connected;
    nlBufferedWriterWrite(&mWriter, &input, sizeof(input));
}

void NetworkInputRecording::Flush()
{
    nlBufferedWriterFlushIfNeeded(&mWriter);
}

bool NetworkInputRecording::ReadNetworkInputPacketHeader(s8 machine, u16* tick, u32* checksum, u32* frame, u32* randomSeed, u32* eventCount, u32* value)
{
    if (nlAsyncFileBufferGetRemaining(&mReader) < sizeof(NetworkRecordedFrameHeader))
        return false;
    NetworkRecordedFrameHeader header;
    nlAsyncFileBufferRead(&mReader, &header, sizeof(header));
    *checksum = header.mChecksum;
    *frame = header.mFrame;
    *randomSeed = header.mRandomSeed;
    *tick = header.mTick;
    *eventCount = header.mEventCount;
    *value = header.mValue;
    return true;
}

bool NetworkInputRecording::ReadNetworkInputEvent(DetermDataEvent* event)
{
    if ((u32)nlAsyncFileBufferGetRemaining(&mReader) < 1)
        return false;
    nlAsyncFileBufferRead(&mReader, &event->mSize, 1);
    if (nlAsyncFileBufferGetRemaining(&mReader) < event->mSize)
        return false;
    nlAsyncFileBufferRead(&mReader, event->mData, event->mSize);
    return true;
}

bool NetworkInputRecording::ReadNetworkInputData(int size, void* data)
{
    if (size == 0)
        return true;
    if (nlAsyncFileBufferGetRemaining(&mReader) < size)
        return false;
    nlAsyncFileBufferRead(&mReader, data, size);
    return true;
}

bool NetworkInputRecording::ReadNetworkInputRecord(s8 machine, PackedDetInput* record, u8* connected)
{
    struct RecordedInput
    {
        PackedDetInput mRecord;
        u8 mConnected;
        u8 mPadding[3];
    } input;
    if (nlAsyncFileBufferGetRemaining(&mReader) < sizeof(input))
        return false;
    nlAsyncFileBufferRead(&mReader, &input, sizeof(input));
    *record = input.mRecord;
    *connected = input.mConnected;
    return true;
}

typedef char VerifyNetworkPeerChannelSize[
    sizeof(NetworkPeerChannel) == 0x240 ? 1 : -1];
typedef char VerifyNetworkPeerSize[
    sizeof(NetworkPeer) == 0x908 ? 1 : -1];
typedef char VerifyNetGameStateSize[
    sizeof(NetworkInputRecording) == 0xF0 ? 1 : -1];
