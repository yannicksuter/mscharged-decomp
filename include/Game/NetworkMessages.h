#ifndef GAME_NETWORK_MESSAGES_H
#define GAME_NETWORK_MESSAGES_H

#include "types.h"

// Polymorphic network message family. The shared serializer at 0x8032C830
// walks slot 2 of each message vtable; slot 1 is the pool deleting
// destructor and slot 0 remains unidentified. Message class identities are
// reconstructed from the retail send/dispatch log strings; the vtables are
// owned by the later message translation unit.
class UnidentifiedNetworkMessage
{
public:
    void operator delete(void* p);

    UnidentifiedNetworkMessage()
        : mUnidentified04(0)
    {
    }

    virtual void MessageVirtual00();
    virtual ~UnidentifiedNetworkMessage() { }
    virtual int GetType();

    /* 0x04 */ u32 mUnidentified04;
};

// "Failed to SendTournamentStartToEveryone to %d because no connection".
class NetMessageTournamentStart : public UnidentifiedNetworkMessage
{
public:
    virtual void MessageVirtual00();
    virtual ~NetMessageTournamentStart() { }
    virtual int GetType();

    /* 0x08 */ u8 mMachineIndex;
    /* 0x09 */ u8 mMachineCount;
    /* 0x0A */ u8 mStadium;
    /* 0x0B */ u8 mUnidentified0B;
    /* 0x0C */ u8 mUnidentified0C;
    /* 0x0D */ u8 mSeedings[8];
};

// "Failed to SendGameStartToEveryone to %d because no connection".
class NetMessageGameStart : public UnidentifiedNetworkMessage
{
public:
    virtual void MessageVirtual00();
    virtual ~NetMessageGameStart() { }
    virtual int GetType();

    /* 0x08 */ u32 mRandomSeed;
    /* 0x0C */ u8 mMachineIndex;
    /* 0x0D */ u8 mMachineCount;
    /* 0x0E */ u8 mHomeCharacters[4];
    /* 0x12 */ u8 mAwayCharacters[4];
    /* 0x16 */ u8 mStadium;
    /* 0x17 */ u8 mMachineFlags[4];
    /* 0x1B */ u8 mUnidentified1B;
    /* 0x1C */ s8 mUnidentified1C[3];
    /* 0x20 */ u32 mUnidentified20;
    /* 0x24 */ u32 mUnidentified24;
};

struct UnidentifiedDraftEntryHead
{
    UnidentifiedDraftEntryHead()
        : mUnidentified00(1)
        , mUnidentified01(1)
        , mUnidentified02(2000)
    {
        mUnidentified04[0] = 0;
        mUnidentified04[1] = 0;
        mUnidentified04[2] = 0;
        mUnidentified04[3] = 0;
        mUnidentified04[4] = 0;
    }

    /* 0x00 */ u8 mUnidentified00;
    /* 0x01 */ u8 mUnidentified01;
    /* 0x02 */ u16 mUnidentified02;
    /* 0x04 */ u32 mUnidentified04[5];
}; // size: 0x18

struct UnidentifiedDraftFooter
{
    s8 mData[8];
};

struct UnidentifiedDraftEntry
{
    UnidentifiedDraftEntry()
    {
        mUnidentified7F = 0;
    }

    /* 0x00 */ UnidentifiedDraftEntryHead mHead;
    /* 0x18 */ u32 mUnidentified18;
    /* 0x1C */ u16 mName[11];
    /* 0x32 */ u8 mUnidentified32[0x4C];
    /* 0x7E */ u8 mIndex;
    /* 0x7F */ u8 mUnidentified7F;
}; // size: 0x80

// "Failed to SendDraftToEveryone to %d because no connection".
class NetMessageDraft : public UnidentifiedNetworkMessage
{
public:
    NetMessageDraft()
        : mMachineIndex(-1)
        , mMachineCount(-1)
        , mUnidentified0A(0)
    {
    }

    virtual void MessageVirtual00();
    virtual ~NetMessageDraft() { }
    virtual int GetType();

    /* 0x008 */ s8 mMachineIndex;
    /* 0x009 */ s8 mMachineCount;
    /* 0x00A */ u8 mUnidentified0A;
    /* 0x00B */ UnidentifiedDraftFooter mUnidentified0B;
    /* 0x014 */ UnidentifiedDraftEntry mEntries[8];
}; // size: 0x414

// "Failed to SendCheckConnectionToEveryone to %d because no connection".
class NetMessageCheckConnection : public UnidentifiedNetworkMessage
{
public:
    virtual void MessageVirtual00();
    virtual ~NetMessageCheckConnection() { }
    virtual int GetType();

    /* 0x08 */ u32 mUnidentified08[2];
};

// Payload-less loaded-game notifications. Every virtual is inline, so the
// vtables and retained weak copies belong to the session translation unit.
class NetMessageLoadedGame : public UnidentifiedNetworkMessage
{
public:
    virtual void MessageVirtual00() { }
    virtual ~NetMessageLoadedGame() { }
    virtual int GetType() { return 0xF; }
};

class NetMessageLoadedGameClient : public UnidentifiedNetworkMessage
{
public:
    virtual void MessageVirtual00() { }
    virtual ~NetMessageLoadedGameClient() { }
    virtual int GetType() { return 0x12; }
};

class NetMessageLoadedGameEveryone : public UnidentifiedNetworkMessage
{
public:
    virtual void MessageVirtual00() { }
    virtual ~NetMessageLoadedGameEveryone() { }
    virtual int GetType() { return 0x13; }
};

// "HOST sending Pause Response to all clients and myself".
class NetMessagePauseResponse_8050AD68 : public UnidentifiedNetworkMessage
{
public:
    virtual void MessageVirtual00();
    virtual ~NetMessagePauseResponse_8050AD68() { }
    virtual int GetType();

    /* 0x08 */ u8 mUnidentified08;
    /* 0x09 */ u8 mUnidentified09;
};

#endif // GAME_NETWORK_MESSAGES_H
