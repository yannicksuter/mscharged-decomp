#include "Game/NetworkInputRecording.h"
#include "Game/NetworkDiagnostics.h"

#include "Game/DetermDataEvent.h"
#include "Game/NetworkSession.h"
#include "Game/PackedDetInput.h"
#include "Game/TweakValue.h"
#include "Game/UnidentifiedStaticStorage.h"
#include "NL/nlDebugFile.h"
#include "NL/nlFile.h"
#include "NL/nlMemory.h"
#include "NL/nlPrint.h"
#include "NL/nlString.h"

struct NetworkRecordedFrameHeader
{
    u32 mChecksum;
    u32 mFrame;
    u32 mRandomSeed;
    u16 mRemapAngle;
    u16 mEventCount;
    u32 mDataSize;
};

struct NetworkRecordedInput
{
    PackedDetInput mRecord;
    u8 mConnected;
    u8 mPadding[3];
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
    "g_numPacketPlaybackTurbo", "Network", &g_numPacketPlaybackTurbo, true);

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
        mBufferedWrites = 0;
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

// Not static: this body is generated before StartNetworkInputRecording, which
// inlines it, so "GameLog/" precedes the file name format in .data as in
// retail. The linker strips the unused out-of-line copy.
void BuildNetworkRecordingPath(char* path, unsigned long size, const char* fileName)
{
    nlStrNCpy(path, "GameLog/", size);
    nlStrNCat(path, path, fileName, size);
}

void NetworkInputRecording::StartNetworkInputRecording(int localMachine, int machineCount, u32 randomSeed, const void* config, int configSize)
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
    BuildNetworkRecordingPath(path, sizeof(path), mFileName);
    mDebugFile = nlOpenFileDebug(path, true, false);
    nlBufferedWriterAttach(&mWriter, mDebugFile,
        mBufferedWrites, 2000, 1800);

    NetworkSessionData* session = g_pNetworkSessionBase;
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
                = session->GetPeer((s8)machine)->mPlayerCount;
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

void NetworkInputRecording::WriteNetworkInputPacketHeader(s8 machine, u16 remapAngle, u32 checksum, u32 frame, u32 randomSeed, u32 eventCount, u32 dataSize)
{
    NetworkRecordedFrameHeader header;
    header.mChecksum = checksum;
    header.mFrame = frame;
    header.mRandomSeed = randomSeed;
    header.mRemapAngle = remapAngle;
    header.mEventCount = eventCount;
    header.mDataSize = dataSize;
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

void NetworkInputRecording::WriteNetworkInputRecord(s8 player, const PackedDetInput* record, u8 connected)
{
    NetworkRecordedInput input;
    input.mRecord = *record;
    input.mConnected = connected;
    input.mPadding[0] = 0;
    input.mPadding[1] = 0;
    input.mPadding[2] = 0;
    nlBufferedWriterWrite(&mWriter, &input, sizeof(input));
}

void NetworkInputRecording::Flush()
{
    nlBufferedWriterFlushIfNeeded(&mWriter);
}

bool NetworkInputRecording::ReadNetworkInputPacketHeader(s8 machine, u16* remapAngle, u32* checksum, u32* frame, u32* randomSeed, u32* eventCount, u32* dataSize)
{
    if (nlAsyncFileBufferGetRemaining(&mReader) < sizeof(NetworkRecordedFrameHeader))
        return false;
    NetworkRecordedFrameHeader header;
    nlAsyncFileBufferRead(&mReader, &header, sizeof(header));
    *checksum = header.mChecksum;
    *frame = header.mFrame;
    *randomSeed = header.mRandomSeed;
    *remapAngle = header.mRemapAngle;
    *eventCount = header.mEventCount;
    *dataSize = header.mDataSize;
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

bool NetworkInputRecording::ReadNetworkInputRecord(s8 player, PackedDetInput* record, u8* connected)
{
    NetworkRecordedInput input;
    if (nlAsyncFileBufferGetRemaining(&mReader) < sizeof(input))
        return false;
    nlAsyncFileBufferRead(&mReader, &input, sizeof(input));
    *record = input.mRecord;
    *connected = input.mConnected;
    return true;
}

typedef char VerifyNetworkInputRecordingSize[
    sizeof(NetworkInputRecording) == 0xF0 ? 1 : -1];
