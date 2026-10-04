#ifndef GAME_LAN_MESSAGES_H
#define GAME_LAN_MESSAGES_H

#include "Game/NetworkMessages.h"

void RegisterLANMessages();

class NetMessageFindGame : public NetworkMessage
{
public:
    NetMessageFindGame();
    virtual void Serialize(NetworkMessageSerializer* serializer);
    virtual ~NetMessageFindGame() { }
    virtual int GetType();

    /* 0x08 */ u32 mToken[2];
}; // size: 0x10

class NetMessageFoundGame : public NetworkMessage
{
public:
    NetMessageFoundGame();
    virtual void Serialize(NetworkMessageSerializer* serializer);
    virtual ~NetMessageFoundGame() { }
    virtual int GetType();

    /* 0x08 */ u8 mToken[8];
    /* 0x10 */ int mGameType;
    /* 0x14 */ u8 mAddress[4];
    /* 0x18 */ u16 mPort;
    /* 0x1A */ char mHostName[12];
}; // size: 0x28

class NetMessageJoinRequest : public NetworkMessage
{
public:
    NetMessageJoinRequest();
    virtual void Serialize(NetworkMessageSerializer* serializer);
    virtual ~NetMessageJoinRequest() { }
    virtual int GetType();

    /* 0x08 */ u8 mAddress[4];
    /* 0x0C */ u16 mPort;
    /* 0x0E */ char mName[11];
    /* 0x19 */ u8 mUserMatchDataSize;
    /* 0x1A */ u8 mUserMatchData[8];
}; // size: 0x24

struct LANPeerMessageInfo
{
    LANPeerMessageInfo()
        : mUserMatchDataSize(0)
    {
    }

    union
    {
        /* 0x00 */ u8 mAddress[4];
        /* 0x00 */ u32 mAddressWord;
    };
    /* 0x04 */ u16 mPort;
    /* 0x06 */ char mName[11];
    /* 0x11 */ s8 mPeerIndex;
    /* 0x12 */ u8 mUserMatchDataSize;
    /* 0x13 */ u8 mUserMatchData[8];
}; // size: 0x1C

class NetMessageJoinResponse : public NetworkMessage
{
public:
    NetMessageJoinResponse();
    virtual void Serialize(NetworkMessageSerializer* serializer);
    virtual ~NetMessageJoinResponse() { }
    virtual int GetType();

    /* 0x08 */ u8 mAddress[4];
    /* 0x0C */ u16 mPort;
    /* 0x0E */ bool mAccepted;
    /* 0x0F */ char mName[11];
    /* 0x1A */ u8 mUserMatchDataSize;
    /* 0x1B */ u8 mUserMatchData[8];
    /* 0x23 */ u8 mPeerCount;
    /* 0x24 */ LANPeerMessageInfo mPeers[7];
}; // size: 0xE8

class NetMessageGamePeerAdded : public NetworkMessage
{
public:
    NetMessageGamePeerAdded();
    virtual void Serialize(NetworkMessageSerializer* serializer);
    virtual ~NetMessageGamePeerAdded() { }
    virtual int GetType();

    /* 0x08 */ LANPeerMessageInfo mPeer;
}; // size: 0x24

class NetMessageTransportType6 : public NetworkMessage
{
public:
    virtual void Serialize(NetworkMessageSerializer* serializer);
    virtual ~NetMessageTransportType6();
    virtual int GetType();
};

class NetMessageReadyToLaunchRequest : public NetworkMessage
{
public:
    virtual void Serialize(NetworkMessageSerializer* serializer);
    virtual ~NetMessageReadyToLaunchRequest() { }
    virtual int GetType();
};

class NetMessageReadyToLaunchConfirm : public NetworkMessage
{
public:
    NetMessageReadyToLaunchConfirm()
        : mConfirmed(0)
    {
    }

    virtual void Serialize(NetworkMessageSerializer* serializer);
    virtual ~NetMessageReadyToLaunchConfirm() { }
    virtual int GetType();

    /* 0x08 */ u8 mConfirmed;
};

class NetMessageClientConfirmedJoin : public NetworkMessage
{
public:
    NetMessageClientConfirmedJoin(s8 index = -1)
        : mMachineIndex(index)
    {
    }

    virtual void Serialize(NetworkMessageSerializer* serializer);
    virtual ~NetMessageClientConfirmedJoin() { }
    virtual int GetType();

    /* 0x08 */ s8 mMachineIndex;
};

#endif // GAME_LAN_MESSAGES_H
