#include "unclassified/tu_80336B2C.h"

#include <string.h>

#include "Game/NetworkSession.h"
#include "Game/TweakValue.h"
#include "NL/globalpad.h"
#include "NL/nlDebugFile.h"
#include "NL/nlFile.h"
#include "NL/nlMemory.h"
#include "NL/nlPrint.h"
#include "NL/nlString.h"
#include "unclassified/tu_80332DC0.h"

struct UnidentifiedPacketHeader
{
    u32 mChecksum;
    u32 mFrame;
    u32 mRandomSeed;
    u16 mTick;
    u16 mEventCount;
    u32 mValue;
};

struct UnidentifiedGameRecordHeader
{
    int mType;
    u32 mConfigSize;
    u32 mRandomSeed;
    int mLocalMachine;
    int mMachineCount;
    int mPlayerCounts[4];
};

extern void* lbl_806E1E28;

extern "C" cGlobalPad* fn_802C082C(void* manager, int index);
extern "C" void fn_80331668(
    InputRouterRecord* record, const DetInput* input);
extern "C" void fn_803317E0(
    InputRouterRecord* record, DetInput* input);
extern "C" void fn_80331A34(
    DetermDataEvent* event, UnidentifiedMessageSerializer* serializer);
extern "C" void fn_802B77B0(void* writer);
extern "C" void fn_802B77D4(void* writer, void* file, bool buffered,
    unsigned int bufferSize, unsigned int flushThreshold);
extern "C" void fn_802B7848(void* writer);
extern "C" void fn_802B79C8(void* writer, const void* data, int size);
extern "C" void fn_802B7A64(void* writer);
extern "C" void* fn_802B41E4(void* reader);
extern "C" void fn_802B4218(void* reader);
extern "C" int fn_802B42B4(void* reader);
extern "C" void fn_802B42C4(void* reader, void* data, u32 size);
extern "C" void fn_803239A8(char* text, unsigned long size, int value);
int g_numPacketPlaybackTurbo;
UnidentifiedNetGameState* lbl_806E2164;

static TweakValueIntImpl_804FD898 lbl_80584B78(
    "g_numPacketPlaybackTurbo", "Network", &g_numPacketPlaybackTurbo);

extern "C" UnidentifiedNetworkPeerChannel* fn_80336B6C(
    UnidentifiedNetworkPeer* peer, int channel)
{
    return &peer->mChannels[channel];
}

extern "C" s8 fn_80336B7C(UnidentifiedNetworkPeer* peer)
{
    return peer->mMachineId;
}

UnidentifiedNetworkPeer::UnidentifiedNetworkPeer()
    : mMachineId(-1)
    , mUnidentified004(0)
{
}

extern "C" void fn_80336BE0(UnidentifiedNetworkPeer* peer)
{
    for (int channel = 0; channel < (int)peer->mUnidentified004; ++channel)
    {
        UnidentifiedNetworkPeerChannel* entry = &peer->mChannels[channel];
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

UnidentifiedNetworkPeerChannel::UnidentifiedNetworkPeerChannel()
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

extern "C" void fn_80336D50(UnidentifiedNetworkPeerChannel* channel,
    UnidentifiedNetworkPeer* peer, s8 channelIndex, int globalPadIndex)
{
    channel->mPeer = peer;
    channel->mChannelIndex = channelIndex;
    channel->mGlobalPadIndex = globalPadIndex;
    channel->mUnidentified00C = false;
}

extern "C" DetInput* fn_80336D68(UnidentifiedNetworkPeerChannel* channel)
{
    return &channel->mInputs[1];
}

extern "C" s8 fn_80336D70(UnidentifiedNetworkPeerChannel* channel)
{
    return channel->mPeer->mMachineId * 4 + channel->mChannelIndex;
}

extern "C" cGlobalPad* fn_80336D90(UnidentifiedNetworkPeerChannel* channel)
{
    if (channel->mGlobalPadIndex == -1)
    {
        return 0;
    }
    cGlobalPad* pad;
    if (channel->mPeer == fn_80338C0C(lbl_806E20D8))
        pad = fn_802C082C(lbl_806E1E28, channel->mGlobalPadIndex);
    else
        pad = 0;
    return pad;
}

extern "C" void fn_80336DF4(UnidentifiedNetworkPeerChannel* channel)
{
    channel->mInputs[2].fn_80332110(channel->mInputs[3]);
    cGlobalPad* pad = fn_80336D90(channel);
    if (pad != 0 && !lbl_806E2138->mFrameProvider->UnidentifiedVirtual1C())
    {
        channel->mInputs[3].fn_8033222C(pad);
    }
    else
    {
        new (&channel->mInputs[3]) DetInput;
    }
    channel->mInputs[3].m_aRemapAngle
        = lbl_806E2138->mFrameProvider->UnidentifiedVirtual18();
}

extern "C" void fn_80336ECC(UnidentifiedNetworkPeerChannel* channel,
    InputRouterRecord* record, u16 tick, u8 connected)
{
    channel->mInputs[0].fn_80332110(channel->mInputs[1]);
    fn_803317E0(record, &channel->mInputs[1]);
    channel->mInputs[1].m_aRemapAngle = tick;
    channel->mInputs[1].m_nConnected = connected;
    channel->mInputs[1].fn_80331D80();
    channel->mInputs[1].fn_80331DC8();
}

extern "C" void fn_80336F48(
    UnidentifiedNetworkPeerChannel* channel, InputRouterRecord* record)
{
    fn_80331668(record, &channel->mInputs[3]);
}

extern "C" u16 fn_80336F58(UnidentifiedNetworkPeerChannel* channel)
{
    return channel->mInputs[3].m_aRemapAngle;
}

extern "C" u8 fn_80336F60(UnidentifiedNetworkPeerChannel* channel)
{
    return channel->mInputs[3].fn_80331BE4();
}

extern "C" s8 fn_80336F68(s8 player, s8 machine)
{
    return machine * 4 + player;
}

NetworkMessageType0_80533B7C::NetworkMessageType0_80533B7C()
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

extern "C" void fn_80337050(
    NetworkMessageType0_80533B7C* message, bool clearInputs, bool clearHeader)
{
    message->mUnidentified00C = 0;
    message->mUnidentified010 = 0;
    message->mUnidentified014 = 0;
    message->mUnidentified00A = 0;
    if (clearHeader)
    {
        memset(message->mUnidentified018, 0,
            sizeof(message->mUnidentified018));
        message->mUnidentified05C = 0;
        message->mUnidentified008 = 0;
    }
    else
    {
        message->mUnidentified008 &= 1;
    }
    if (clearInputs)
    {
        message->mUnidentified009 = 0;
        memset(message->mUnidentified01C, 0,
            sizeof(message->mUnidentified01C));
    }
}

extern "C" void fn_803370F0(NetworkMessageType0_80533B7C* destination,
    const NetworkMessageType0_80533B7C* source)
{
    destination->mUnidentified008 = source->mUnidentified008;
    destination->mUnidentified009 = source->mUnidentified009;
    destination->mUnidentified00A = source->mUnidentified00A;
    destination->mUnidentified00C = source->mUnidentified00C;
    destination->mUnidentified010 = source->mUnidentified010;
    destination->mUnidentified014 = source->mUnidentified014;
    destination->mUnidentified05C = source->mUnidentified05C;
    memcpy(destination->mUnidentified018, source->mUnidentified018, 4);
    memcpy(destination->mUnidentified01C, source->mUnidentified01C, 0x40);
    memcpy(destination->mDetermData, source->mDetermData, 0x90);
}

extern "C" void fn_8033718C(
    NetworkMessageType0_80533B7C* message, s8 player, u8 state)
{
    int shift = (player & 0x7F) * 2;
    u32 mask = 3 << shift;
    u8 inverse = ~mask;
    u8 value = message->mUnidentified00A & inverse;
    message->mUnidentified00A = value | (state << shift);
}

extern "C" u8 fn_803371B8(
    NetworkMessageType0_80533B7C* message, s8 player)
{
    return (message->mUnidentified00A >> ((player & 0x7F) * 2)) & 3;
}

extern "C" void fn_803371CC(NetworkMessageType0_80533B7C* message,
    s8 player, const InputRouterRecord* record)
{
    const u8* source = record->mData;
    u8* previous = message->mUnidentified01C[player].mData;
    u8& changes = message->mUnidentified018[player];

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
        message->mUnidentified008 |= 0x10 << player;
    memcpy(previous, source, 0x10);
}

extern "C" void fn_80337380(NetworkMessageType0_80533B7C* message,
    s8 player, InputRouterRecord* record)
{
    u8* output = record->mData;
    const u8* previous = message->mUnidentified01C[player].mData;
    u8 changes = message->mUnidentified018[player];
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

extern "C" void fn_80337458(
    NetworkMessageType0_80533B7C* message, u16 tick)
{
    u8 value = tick >> 8;
    if (message->mUnidentified009 != value)
    {
        message->mUnidentified009 = value;
        message->mUnidentified008 |= 1;
    }
}

extern "C" void fn_8033747C(
    NetworkMessageType0_80533B7C* message, u16* tick)
{
    if (message->mUnidentified008 & 1)
        *tick = message->mUnidentified009 << 8;
}

extern "C" void fn_80337498(NetworkMessageType0_80533B7C* message,
    u32 checksum, u32 frame, u32 randomSeed)
{
    message->mUnidentified00C = checksum;
    message->mUnidentified010 = frame;
    message->mUnidentified014 = randomSeed;
    message->mUnidentified008 |= 4;
}

extern "C" void fn_803374B4(
    NetworkMessageType0_80533B7C* message, bool congested)
{
    if (congested)
        message->mUnidentified008 |= 8;
    else
        message->mUnidentified008 &= ~8;
}

extern "C" void fn_803374DC(NetworkMessageType0_80533B7C* message,
    const DetermDataEvent* event)
{
    if (message->mUnidentified05C < 4)
    {
        message->mDetermData[message->mUnidentified05C] = *event;
        ++message->mUnidentified05C;
    }
}

extern "C" DetermDataEvent* fn_80337620(
    NetworkMessageType0_80533B7C* message, int index)
{
    return &message->mDetermData[index];
}

void NetworkMessageType0_80533B7C::Serialize(
    UnidentifiedMessageSerializer* serializer)
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
        fn_80331A34(&mDetermData[i], serializer);
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

void NetworkMessageType1_80533B68::Serialize(
    UnidentifiedMessageSerializer* serializer)
{
    mMessage0.Serialize(serializer);
    mMessage1.Serialize(serializer);
}

int NetworkMessageType1_80533B68::GetType()
{
    return 1;
}

int NetworkMessageType0_80533B7C::GetType()
{
    return 0;
}

NetworkMessageType8_80533BA4::NetworkMessageType8_80533BA4()
{
    mUnidentified008[0] = 0;
    memset(mPadding3CC, 0, sizeof(mPadding3CC));
}

void NetworkMessageType8_80533BA4::Serialize(
    UnidentifiedMessageSerializer* serializer)
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

void NetworkMessageType9_80533B90::Serialize(
    UnidentifiedMessageSerializer* serializer)
{
    mMessage0.Serialize(serializer);
    mMessage1.Serialize(serializer);
}

int NetworkMessageType9_80533B90::GetType()
{
    return 9;
}

int NetworkMessageType8_80533BA4::GetType()
{
    return 8;
}

extern "C" void fn_80337F68()
{
    UnidentifiedNetGameState* state
        = (UnidentifiedNetGameState*)nlMalloc(
            sizeof(UnidentifiedNetGameState), 8, false);
    if (state != 0)
    {
        fn_802B77B0(state->mWriter);
        fn_802B41E4(state->mReader);
        state->mConfigSize = 0;
        state->mConfig = 0;
        state->mRandomSeed = 0;
        state->mMachineCount = 0;
        state->mLocalMachine = 0;
        state->mPlayerCounts[0] = 0;
        state->mPlayerCounts[1] = 0;
        state->mPlayerCounts[2] = 0;
        state->mPlayerCounts[3] = 0;
        fn_80337FF0(state, true);
    }
    lbl_806E2164 = state;
}

extern "C" void fn_80337FF0(
    UnidentifiedNetGameState* state, bool constructing)
{
    if (constructing)
    {
        state->mUnidentified02 = 0;
        state->mRecordingEnabled = false;
        state->mRecording = false;
        state->mPlaybackEnabled = false;
        state->mPlaybackReady = false;
        state->mFileName[0] = '\0';
        state->mDebugFile = 0;
        state->mFile = 0;
    }
    else
    {
        fn_802B7848(state->mWriter);
        if (nlDebugFileIsValid(state->mDebugFile))
        {
            nlCloseFileDebug(state->mDebugFile);
            state->mDebugFile = 0;
        }
        fn_802B4218(state->mReader);
        if (state->mFile != 0)
        {
            nlClose((nlFile*)state->mFile);
            state->mFile = 0;
        }
        if (state->mConfig != 0)
        {
            nlFree(state->mConfig);
            state->mConfig = 0;
        }
        if (state->mRecordingEnabled)
            state->mFileName[0] = '\0';
        state->mRecording = false;
        state->mPlaybackReady = false;
        state->mPlaybackEnabled = false;
    }

    state->mConfigSize = 0;
    state->mConfig = 0;
    state->mRandomSeed = 0;
    state->mMachineCount = 0;
    state->mLocalMachine = 0;
    state->mPlayerCounts[0] = 0;
    state->mPlayerCounts[1] = 0;
    state->mPlayerCounts[2] = 0;
    state->mPlayerCounts[3] = 0;
}

extern "C" void fn_803380F4(UnidentifiedNetGameState* state,
    s8 localMachine, int machineCount, u32 randomSeed, const void* config,
    int configSize)
{
    state->mRecording = true;
    if ((s8)state->mFileName[0] == 0)
    {
        char suffix[100];
        fn_803239A8(suffix, sizeof(suffix), 0);
        nlSNPrintf(state->mFileName, sizeof(state->mFileName),
            "netpackrec_%s__%d_%d.bin", suffix, localMachine, machineCount);
    }

    char path[256];
    nlStrNCpy(path, "GameLog/", sizeof(path));
    nlStrNCat(path, path, state->mFileName, sizeof(path));
    state->mDebugFile = nlOpenFileDebug(path, true, false);
    fn_802B77D4(state->mWriter, state->mDebugFile,
        state->mUnidentified02, 2000, 1800);

    UnidentifiedGameRecordHeader header;
    header.mType = 13;
    header.mConfigSize = configSize;
    header.mRandomSeed = randomSeed;
    header.mLocalMachine = localMachine;
    header.mMachineCount = machineCount;
    for (int machine = 0; machine < 4; ++machine)
    {
        if (machine < machineCount)
            header.mPlayerCounts[machine]
                = fn_80338BF8(lbl_806E20D8, (s8)machine)->mUnidentified004;
        else
            header.mPlayerCounts[machine] = 0;
    }
    fn_802B79C8(state->mWriter, &header, sizeof(header));
    fn_802B79C8(state->mWriter, config, configSize);
    fn_802B7A64(state->mWriter);
}

extern "C" bool fn_80338284(UnidentifiedNetGameState* state)
{
    UnidentifiedGameRecordHeader header;
    fn_802B42C4(state->mReader, &header, sizeof(header));
    if (header.mType != 13)
        return false;

    state->mRandomSeed = header.mRandomSeed;
    state->mMachineCount = header.mMachineCount;
    state->mLocalMachine = header.mLocalMachine;
    for (int machine = 0; machine < 4; ++machine)
        state->mPlayerCounts[machine] = header.mPlayerCounts[machine];
    state->mConfigSize = header.mConfigSize;
    state->mConfig = nlMalloc(header.mConfigSize, 8, false);
    fn_802B42C4(state->mReader, state->mConfig, header.mConfigSize);
    state->mPlaybackReady = true;
    return true;
}

extern "C" int fn_80338340(UnidentifiedNetGameState* state)
{
    return state->mPlaybackReady ? g_numPacketPlaybackTurbo : 0;
}

extern "C" void fn_8033835C(UnidentifiedNetGameState* state, s8,
    u16 tick, u32 checksum, u32 frame, u32 randomSeed, u16 eventCount,
    u32 value)
{
    UnidentifiedPacketHeader header;
    header.mChecksum = checksum;
    header.mFrame = frame;
    header.mRandomSeed = randomSeed;
    header.mTick = tick;
    header.mEventCount = eventCount;
    header.mValue = value;
    fn_802B79C8(state->mWriter, &header, sizeof(header));
}

extern "C" void fn_803383A0(
    UnidentifiedNetGameState* state, const DetermDataEvent* event)
{
    fn_802B79C8(state->mWriter, &event->mSize, 1);
    fn_802B79C8(state->mWriter, event->mData, event->mSize);
}

extern "C" void fn_803383F0(
    UnidentifiedNetGameState* state, const void* data, int size)
{
    if (size > 0)
        fn_802B79C8(state->mWriter, data, size);
}

extern "C" void fn_80338404(UnidentifiedNetGameState* state, s8,
    const InputRouterRecord* record, u8 connected)
{
    struct RecordedInput
    {
        u8 mRecord[0x10];
        u8 mConnected;
        u8 mPadding[3];
    } input;
    memset(&input, 0, sizeof(input));
    memcpy(input.mRecord, record->mData, sizeof(input.mRecord));
    input.mConnected = connected;
    fn_802B79C8(state->mWriter, &input, sizeof(input));
}

extern "C" void fn_803384E0(UnidentifiedNetGameState* state)
{
    fn_802B7A64(state->mWriter);
}

extern "C" bool fn_803384E8(UnidentifiedNetGameState* state, s8,
    u16* tick, u32* checksum, u32* frame, u32* randomSeed, u32* eventCount,
    u32* value)
{
    if (fn_802B42B4(state->mReader) < sizeof(UnidentifiedPacketHeader))
        return false;
    UnidentifiedPacketHeader header;
    fn_802B42C4(state->mReader, &header, sizeof(header));
    *checksum = header.mChecksum;
    *frame = header.mFrame;
    *randomSeed = header.mRandomSeed;
    *tick = header.mTick;
    *eventCount = header.mEventCount;
    *value = header.mValue;
    return true;
}

extern "C" bool fn_80338584(
    UnidentifiedNetGameState* state, DetermDataEvent* event)
{
    if ((u32)fn_802B42B4(state->mReader) < 1)
        return false;
    fn_802B42C4(state->mReader, &event->mSize, 1);
    if (fn_802B42B4(state->mReader) < event->mSize)
        return false;
    fn_802B42C4(state->mReader, event->mData, event->mSize);
    return true;
}

extern "C" bool fn_80338618(
    UnidentifiedNetGameState* state, int size, void* data)
{
    if (size == 0)
        return true;
    if (fn_802B42B4(state->mReader) < size)
        return false;
    fn_802B42C4(state->mReader, data, size);
    return true;
}

extern "C" bool fn_80338694(UnidentifiedNetGameState* state, s8,
    InputRouterRecord* record, u8* connected)
{
    struct RecordedInput
    {
        u8 mRecord[0x10];
        u8 mConnected;
        u8 mPadding[3];
    } input;
    memset(&input, 0, sizeof(input));
    if (fn_802B42B4(state->mReader) < sizeof(input))
        return false;
    fn_802B42C4(state->mReader, &input, sizeof(input));
    memcpy(record->mData, input.mRecord, sizeof(input.mRecord));
    *connected = input.mConnected;
    return true;
}

typedef char VerifyNetworkPeerChannelSize[
    sizeof(UnidentifiedNetworkPeerChannel) == 0x240 ? 1 : -1];
typedef char VerifyNetworkPeerSize[
    sizeof(UnidentifiedNetworkPeer) == 0x908 ? 1 : -1];
typedef char VerifyNetGameStateSize[
    sizeof(UnidentifiedNetGameState) == 0xF0 ? 1 : -1];
