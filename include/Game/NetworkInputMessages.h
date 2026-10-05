#ifndef GAME_NETWORK_INPUT_MESSAGES_H
#define GAME_NETWORK_INPUT_MESSAGES_H

#include "Game/DetermDataEvent.h"
#include "Game/NetworkMessages.h"

struct PackedDetInput;

struct NetworkMessageInputRecord
{
    NetworkMessageInputRecord()
    {
        memset(mData, 0, sizeof(mData));
    }

    u8 mData[0x10];
};

class NetMessageInput : public NetworkMessage
{
public:
    NetMessageInput();

    void Reset(bool clearInputs, bool clearHeader);
    void CopyFrom(const NetMessageInput* source);
    void SetNetworkInputMessagePlayerState(s8 player, u8 state);
    u8 GetNetworkInputMessagePlayerState(s8 player);
    void SetNetworkInputMessageRecord(s8 player, const PackedDetInput* record);
    void ApplyNetworkInputMessageRecord(s8 player, PackedDetInput* record);
    void SetNetworkInputMessageRemapAngle(u16 remapAngle);
    void GetNetworkInputMessageRemapAngle(u16* remapAngle);
    void SetNetworkInputMessageSyncData(u32 checksum, u32 frame, u32 randomSeed);
    void SetNetworkInputMessageCongested(bool congested);
    void AddNetworkInputMessageEvent(DetermDataEvent* event);
    DetermDataEvent* GetNetworkInputMessageEvent(int index);

    virtual void Serialize(NetworkMessageSerializer* serializer);
    virtual ~NetMessageInput() { }
    virtual int GetType();

    /* 0x008 */ u8 mFlags;
    /* 0x009 */ u8 mRemapAngleHigh;
    /* 0x00A */ u8 mPlayerStates;
    /* 0x00B */ u8 mPadding00B;
    /* 0x00C */ u32 mChecksum;
    /* 0x010 */ u32 mFrame;
    /* 0x014 */ u32 mRandomSeed;
    /* 0x018 */ u8 mRecordChanges[4];
    /* 0x01C */ NetworkMessageInputRecord mRecords[4];
    /* 0x05C */ u8 mEventCount;
    /* 0x05D */ DetermDataEvent mEvents[4];
    /* 0x0ED */ u8 mPadding0ED[3];
}; // size: 0xF0

class NetMessageInputBundle : public NetworkMessage
{
public:
    NetMessageInputBundle() { }

    virtual void Serialize(NetworkMessageSerializer* serializer);
    virtual ~NetMessageInputBundle();
    virtual int GetType();

    /* 0x008 */ NetMessageInput mMessage0;
    /* 0x0F8 */ NetMessageInput mMessage1;
}; // size: 0x1E8

class NetMessageAllInputs : public NetworkMessage
{
public:
    NetMessageAllInputs();

    virtual void Serialize(NetworkMessageSerializer* serializer);
    virtual ~NetMessageAllInputs() { }
    virtual int GetType();

    /* 0x008 */ u8 mFlags;
    /* 0x009 */ u8 mPadding009[3];
    /* 0x00C */ NetMessageInput mMessages[4];
    /* 0x3CC */ u32 mUnidentified3CC[3];
}; // size: 0x3D8

class NetMessageAllInputsBundle : public NetworkMessage
{
public:
    NetMessageAllInputsBundle() { }

    virtual void Serialize(NetworkMessageSerializer* serializer);
    virtual ~NetMessageAllInputsBundle();
    virtual int GetType();

    /* 0x008 */ NetMessageAllInputs mMessage0;
    /* 0x3E0 */ NetMessageAllInputs mMessage1;
}; // size: 0x7B8

#endif // GAME_NETWORK_INPUT_MESSAGES_H
