#ifndef UNCLASSIFIED_TU_80336B2C_H
#define UNCLASSIFIED_TU_80336B2C_H

#include "types.h"

class cGlobalPad;
class DetInput;
class UnidentifiedNetworkPeer;
class UnidentifiedNetworkPeerChannel;
class NetworkMessageType0_80533B7C;
struct DetermDataEvent;
struct InputRouterRecord;

struct UnidentifiedNetGameState
{
    /* 0x00 */ bool mRecordingEnabled;
    /* 0x01 */ bool mRecording;
    /* 0x02 */ u8 mUnidentified02;
    /* 0x03 */ bool mPlaybackEnabled;
    /* 0x04 */ bool mPlaybackReady;
    /* 0x05 */ char mFileName[0x64];
    /* 0x69 */ u8 mPadding069[3];
    /* 0x6C */ void* mDebugFile;
    /* 0x70 */ u8 mWriter[0x18];
    /* 0x88 */ void* mFile;
    /* 0x8C */ u8 mReader[0x40];
    /* 0xCC */ u32 mConfigSize;
    /* 0xD0 */ void* mConfig;
    /* 0xD4 */ u32 mRandomSeed;
    /* 0xD8 */ int mMachineCount;
    /* 0xDC */ int mLocalMachine;
    /* 0xE0 */ int mPlayerCounts[4];
}; // size: 0xF0

extern int g_numPacketPlaybackTurbo;
extern UnidentifiedNetGameState* lbl_806E2164;

extern "C" UnidentifiedNetworkPeerChannel* fn_80336B6C(
    UnidentifiedNetworkPeer* peer, int channel);
extern "C" s8 fn_80336B7C(UnidentifiedNetworkPeer* peer);
extern "C" void fn_80336BE0(UnidentifiedNetworkPeer* peer);
extern "C" void fn_80336D50(UnidentifiedNetworkPeerChannel* channel,
    UnidentifiedNetworkPeer* peer, s8 channelIndex, int globalPadIndex);
extern "C" DetInput* fn_80336D68(UnidentifiedNetworkPeerChannel* channel);
extern "C" s8 fn_80336D70(UnidentifiedNetworkPeerChannel* channel);
extern "C" cGlobalPad* fn_80336D90(UnidentifiedNetworkPeerChannel* channel);
extern "C" void fn_80336DF4(UnidentifiedNetworkPeerChannel* channel);
extern "C" void fn_80336ECC(UnidentifiedNetworkPeerChannel* channel,
    InputRouterRecord* record, u16 tick, u8 connected);
extern "C" void fn_80336F48(
    UnidentifiedNetworkPeerChannel* channel, InputRouterRecord* record);
extern "C" u16 fn_80336F58(UnidentifiedNetworkPeerChannel* channel);
extern "C" u8 fn_80336F60(UnidentifiedNetworkPeerChannel* channel);
extern "C" s8 fn_80336F68(s8 player, s8 machine);

extern "C" void fn_80337050(
    NetworkMessageType0_80533B7C* message, bool clearInputs, bool clearHeader);
extern "C" void fn_803370F0(NetworkMessageType0_80533B7C* destination,
    const NetworkMessageType0_80533B7C* source);
extern "C" void fn_8033718C(
    NetworkMessageType0_80533B7C* message, s8 player, u8 state);
extern "C" u8 fn_803371B8(
    NetworkMessageType0_80533B7C* message, s8 player);
extern "C" void fn_803371CC(NetworkMessageType0_80533B7C* message,
    s8 player, const InputRouterRecord* record);
extern "C" void fn_80337380(NetworkMessageType0_80533B7C* message,
    s8 player, InputRouterRecord* record);
extern "C" void fn_80337458(
    NetworkMessageType0_80533B7C* message, u16 tick);
extern "C" void fn_8033747C(
    NetworkMessageType0_80533B7C* message, u16* tick);
extern "C" void fn_80337498(NetworkMessageType0_80533B7C* message,
    u32 checksum, u32 frame, u32 randomSeed);
extern "C" void fn_803374B4(
    NetworkMessageType0_80533B7C* message, bool congested);
extern "C" void fn_803374DC(NetworkMessageType0_80533B7C* message,
    const DetermDataEvent* event);
extern "C" DetermDataEvent* fn_80337620(
    NetworkMessageType0_80533B7C* message, int index);

extern "C" void fn_80337F68();
extern "C" void fn_80337FF0(
    UnidentifiedNetGameState* state, bool constructing);
extern "C" void fn_803380F4(UnidentifiedNetGameState* state,
    s8 localMachine, int machineCount, u32 randomSeed, const void* config,
    int configSize);
extern "C" bool fn_80338284(UnidentifiedNetGameState* state);
extern "C" int fn_80338340(UnidentifiedNetGameState* state);
extern "C" void fn_8033835C(UnidentifiedNetGameState* state, s8 machine,
    u16 tick, u32 checksum, u32 frame, u32 randomSeed, u16 eventCount,
    u32 value);
extern "C" void fn_803383A0(
    UnidentifiedNetGameState* state, const DetermDataEvent* event);
extern "C" void fn_803383F0(
    UnidentifiedNetGameState* state, const void* data, int size);
extern "C" void fn_80338404(UnidentifiedNetGameState* state, s8 machine,
    const InputRouterRecord* record, u8 connected);
extern "C" void fn_803384E0(UnidentifiedNetGameState* state);
extern "C" bool fn_803384E8(UnidentifiedNetGameState* state, s8 machine,
    u16* tick, u32* checksum, u32* frame, u32* randomSeed, u32* eventCount,
    u32* value);
extern "C" bool fn_80338584(
    UnidentifiedNetGameState* state, DetermDataEvent* event);
extern "C" bool fn_80338618(
    UnidentifiedNetGameState* state, int size, void* data);
extern "C" bool fn_80338694(UnidentifiedNetGameState* state, s8 machine,
    InputRouterRecord* record, u8* connected);

#endif // UNCLASSIFIED_TU_80336B2C_H
