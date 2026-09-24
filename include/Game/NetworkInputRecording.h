#ifndef GAME_NETWORK_INPUT_RECORDING_H
#define GAME_NETWORK_INPUT_RECORDING_H

#include "NL/nlBufferedWriter.h"
#include "NL/nlAsyncFileBuffer.h"
#include "types.h"

struct DetermDataEvent;
struct PackedDetInput;

struct NetworkInputRecording
{
    void Reset(bool constructing);
    void StartNetworkInputRecording(int localMachine, int machineCount, u32 randomSeed, const void* config, int configSize);
    bool ReadNetworkInputRecordingHeader();
    int GetNetworkInputPlaybackExtraUpdates();
    void WriteNetworkInputPacketHeader(s8 machine, u16 tick, u32 checksum, u32 frame, u32 randomSeed, u16 eventCount, u32 value);
    void WriteNetworkInputEvent(const DetermDataEvent* event);
    void WriteData(const void* data, int size);
    void WriteNetworkInputRecord(s8 machine, const PackedDetInput* record, u8 connected);
    void Flush();
    bool ReadNetworkInputPacketHeader(s8 machine, u16* tick, u32* checksum, u32* frame, u32* randomSeed, u32* eventCount, u32* value);
    bool ReadNetworkInputEvent(DetermDataEvent* event);
    bool ReadNetworkInputData(int size, void* data);
    bool ReadNetworkInputRecord(s8 machine, PackedDetInput* record, u8* connected);

    /* 0x00 */ bool mRecordingEnabled;
    /* 0x01 */ bool mRecording;
    /* 0x02 */ bool mUnidentified02;
    /* 0x03 */ bool mPlaybackEnabled;
    /* 0x04 */ bool mPlaybackReady;
    /* 0x05 */ char mFileName[0x64];
    /* 0x69 */ u8 mPadding069[3];
    /* 0x6C */ void* mDebugFile;
    /* 0x70 */ nlBufferedWriter mWriter;
    /* 0x88 */ void* mFile;
    /* 0x8C */ nlAsyncFileBuffer mReader;
    /* 0xCC */ u32 mConfigSize;
    /* 0xD0 */ void* mConfig;
    /* 0xD4 */ u32 mRandomSeed;
    /* 0xD8 */ int mMachineCount;
    /* 0xDC */ int mLocalMachine;
    /* 0xE0 */ int mPlayerCounts[4];
}; // size: 0xF0

extern int g_numPacketPlaybackTurbo;
extern NetworkInputRecording* gNetworkInputRecording;

void InitializeNetworkInputRecording();

#endif // GAME_NETWORK_INPUT_RECORDING_H
