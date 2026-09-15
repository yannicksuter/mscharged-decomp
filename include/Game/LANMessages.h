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

    /* 0x08 */ u32 mUnidentified08[2];
}; // size: 0x10

class NetMessageFoundGame : public NetworkMessage
{
public:
    NetMessageFoundGame();
    virtual void Serialize(NetworkMessageSerializer* serializer);
    virtual ~NetMessageFoundGame() { }
    virtual int GetType();

    /* 0x08 */ u8 mUnidentified08[8];
    /* 0x10 */ int mUnidentified10;
    /* 0x14 */ u8 mUnidentified14[4];
    /* 0x18 */ u16 mUnidentified18;
    /* 0x1A */ char mUnidentified1A[12];
}; // size: 0x28

class NetMessageJoinRequest : public NetworkMessage
{
public:
    NetMessageJoinRequest();
    virtual void Serialize(NetworkMessageSerializer* serializer);
    virtual ~NetMessageJoinRequest() { }
    virtual int GetType();

    /* 0x08 */ u8 mUnidentified08[4];
    /* 0x0C */ u16 mUnidentified0C;
    /* 0x0E */ char mUnidentified0E[11];
    /* 0x19 */ u8 mUnidentified19;
    /* 0x1A */ u8 mUnidentified1A[8];
}; // size: 0x24

struct LANPeerMessageInfo
{
    LANPeerMessageInfo()
        : mUnidentified12(0)
    {
    }

    union
    {
        /* 0x00 */ u8 mUnidentified00[4];
        /* 0x00 */ u32 mAddressWord;
    };
    /* 0x04 */ u16 mUnidentified04;
    /* 0x06 */ char mUnidentified06[11];
    /* 0x11 */ s8 mUnidentified11;
    /* 0x12 */ u8 mUnidentified12;
    /* 0x13 */ u8 mUnidentified13[8];
}; // size: 0x1C

class NetMessageJoinResponse : public NetworkMessage
{
public:
    NetMessageJoinResponse();
    virtual void Serialize(NetworkMessageSerializer* serializer);
    virtual ~NetMessageJoinResponse() { }
    virtual int GetType();

    /* 0x08 */ u8 mUnidentified08[4];
    /* 0x0C */ u16 mUnidentified0C;
    /* 0x0E */ bool mUnidentified0E;
    /* 0x0F */ char mUnidentified0F[11];
    /* 0x1A */ u8 mUnidentified1A;
    /* 0x1B */ u8 mUnidentified1B[8];
    /* 0x23 */ u8 mUnidentified23;
    /* 0x24 */ LANPeerMessageInfo mUnidentified24[7];
}; // size: 0xE8

class NetMessageTransportType6 : public NetworkMessage
{
public:
    virtual void Serialize(NetworkMessageSerializer* serializer);
    virtual ~NetMessageTransportType6();
    virtual int GetType();
};

class NetMessageGamePeerAdded : public NetworkMessage
{
public:
    NetMessageGamePeerAdded();
    virtual void Serialize(NetworkMessageSerializer* serializer);
    virtual ~NetMessageGamePeerAdded() { }
    virtual int GetType();

    /* 0x08 */ LANPeerMessageInfo mUnidentified08;
}; // size: 0x24

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
        : mUnidentified08(0)
    {
    }

    virtual void Serialize(NetworkMessageSerializer* serializer);
    virtual ~NetMessageReadyToLaunchConfirm() { }
    virtual int GetType();

    /* 0x08 */ u8 mUnidentified08;
};

class NetMessageClientConfirmedJoin : public NetworkMessage
{
public:
    NetMessageClientConfirmedJoin(s8 index = -1)
        : mUnidentified08(index)
    {
    }

    virtual void Serialize(NetworkMessageSerializer* serializer);
    virtual ~NetMessageClientConfirmedJoin() { }
    virtual int GetType();

    /* 0x08 */ s8 mUnidentified08;
};

#endif // GAME_LAN_MESSAGES_H
