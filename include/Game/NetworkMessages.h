#ifndef GAME_NETWORK_MESSAGES_H
#define GAME_NETWORK_MESSAGES_H

#include <string.h>

#include "types.h"

class UnidentifiedMessageSerializer
{
public:
    void Transfer(void* value, unsigned long size)
    {
        if (mDirection == 0)
        {
            memcpy(value, mPosition, size);
            mPosition += size;
        }
        else
        {
            memcpy(mPosition, value, size);
            mPosition += size;
        }
    }

    /* 0x0 */ int mDirection;
    /* 0x4 */ u8* mPosition;
};

extern "C" void* fn_8032C66C(unsigned long size, unsigned int alignment, bool);

// Polymorphic network message family. The shared serializer at 0x8032C830
// walks slot 2 of each message vtable; slot 1 is the pool deleting
// destructor and slot 0 remains unidentified. Message class identities are
// reconstructed from the retail send/dispatch log strings; each vtable is
// emitted with the translation unit that retains its virtual definitions.
class UnidentifiedNetworkMessage
{
public:
    void* operator new(unsigned long size)
    {
        return fn_8032C66C(size, 8, false);
    }
    void operator delete(void* p);

    UnidentifiedNetworkMessage()
        : mUnidentified04(0)
    {
    }

    virtual void Serialize(UnidentifiedMessageSerializer* serializer);
    virtual ~UnidentifiedNetworkMessage() { }
    virtual int GetType();

    /* 0x04 */ u32 mUnidentified04;
};

class UnidentifiedNetworkMessageReceiver
{
public:
    virtual int ReceiverVirtual00(UnidentifiedNetworkMessage* message) = 0;
};

// "Failed to SendTournamentStartToEveryone to %d because no connection".
class NetMessageTournamentStart : public UnidentifiedNetworkMessage
{
public:
    virtual void Serialize(UnidentifiedMessageSerializer* serializer);
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
    virtual void Serialize(UnidentifiedMessageSerializer* serializer);
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

    virtual void Serialize(UnidentifiedMessageSerializer* serializer);
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
    virtual void Serialize(UnidentifiedMessageSerializer* serializer);
    virtual ~NetMessageCheckConnection() { }
    virtual int GetType();

    /* 0x08 */ u32 mUnidentified08[2];
};

// Message IDs 16 and 17 are registered by this translation unit, but no
// surviving behavior-level name has yet been established for either payload.
class NetworkMessageType16_8050AC38 : public UnidentifiedNetworkMessage
{
public:
    virtual void Serialize(UnidentifiedMessageSerializer* serializer);
    virtual ~NetworkMessageType16_8050AC38() { }
    virtual int GetType();
};

class NetworkMessageType17_8050AC4C : public UnidentifiedNetworkMessage
{
public:
    virtual void Serialize(UnidentifiedMessageSerializer* serializer);
    virtual ~NetworkMessageType17_8050AC4C() { }
    virtual int GetType();

    /* 0x08 */ u32 mUnidentified08;
};

// Payload-less loaded-game notifications. Every virtual is inline, so the
// vtables and retained weak copies belong to the session translation unit.
class NetMessageLoadedGame : public UnidentifiedNetworkMessage
{
public:
    virtual void Serialize(UnidentifiedMessageSerializer*) { }
    virtual ~NetMessageLoadedGame() { }
    virtual int GetType() { return 0xF; }
};

class NetMessageLoadedGameClient : public UnidentifiedNetworkMessage
{
public:
    virtual void Serialize(UnidentifiedMessageSerializer*) { }
    virtual ~NetMessageLoadedGameClient() { }
    virtual int GetType() { return 0x12; }
};

class NetMessageLoadedGameEveryone : public UnidentifiedNetworkMessage
{
public:
    virtual void Serialize(UnidentifiedMessageSerializer*) { }
    virtual ~NetMessageLoadedGameEveryone() { }
    virtual int GetType() { return 0x13; }
};

class NetworkMessageType22_8050B7B4 : public UnidentifiedNetworkMessage
{
public:
    virtual void Serialize(UnidentifiedMessageSerializer* serializer);
    virtual ~NetworkMessageType22_8050B7B4();
    virtual int GetType();

    /* 0x008 */ UnidentifiedDraftEntry mEntry;
}; // size: 0x88

// "Sending NetworkDraftPickedCaptain team %d captain %d".
class NetMessageDraftPickedCaptain : public UnidentifiedNetworkMessage
{
public:
    virtual void Serialize(UnidentifiedMessageSerializer* serializer);
    virtual ~NetMessageDraftPickedCaptain();
    virtual int GetType();

    /* 0x08 */ u8 mTeamIndex;
    /* 0x09 */ u8 mCaptain;
};

// "Ignoring ReceivedDraftPickedSidekicks because in draft state %d".
class NetMessageDraftPickedSidekicks : public UnidentifiedNetworkMessage
{
public:
    virtual void Serialize(UnidentifiedMessageSerializer* serializer);
    virtual ~NetMessageDraftPickedSidekicks();
    virtual int GetType();

    /* 0x08 */ u8 mTeamIndex;
    /* 0x09 */ u8 mSidekick0;
    /* 0x0A */ u8 mSidekick1;
    /* 0x0B */ u8 mSidekick2;
};

class NetworkMessageType25_8050B778 : public UnidentifiedNetworkMessage
{
public:
    virtual void Serialize(UnidentifiedMessageSerializer* serializer);
    virtual ~NetworkMessageType25_8050B778();
    virtual int GetType();

    /* 0x08 */ u8 mUnidentified08;
    /* 0x09 */ u8 mUnidentified09;
    /* 0x0A */ u8 mUnidentified0A;
    /* 0x0B */ u8 mUnidentified0B;
};

class NetworkMessageType27_8050B750 : public UnidentifiedNetworkMessage
{
public:
    virtual void Serialize(UnidentifiedMessageSerializer* serializer);
    virtual ~NetworkMessageType27_8050B750();
    virtual int GetType();

    /* 0x08 */ u8 mUnidentified08;
    /* 0x09 */ u8 mUnidentified09;
};

class NetMessagePauseRequest_8050AD7C : public UnidentifiedNetworkMessage
{
public:
    virtual void Serialize(UnidentifiedMessageSerializer* serializer);
    virtual ~NetMessagePauseRequest_8050AD7C();
    virtual int GetType();

    /* 0x08 */ u8 mMachineIndex;
    /* 0x09 */ u8 mPaused;
};

// "HOST sending Pause Response to all clients and myself".
class NetMessagePauseResponse_8050AD68 : public UnidentifiedNetworkMessage
{
public:
    virtual void Serialize(UnidentifiedMessageSerializer* serializer);
    virtual ~NetMessagePauseResponse_8050AD68() { }
    virtual int GetType();

    /* 0x08 */ u8 mMachineMask;
};

class NetworkMessageType30_8050ADA4 : public UnidentifiedNetworkMessage
{
public:
    NetworkMessageType30_8050ADA4()
        : mUnidentified08(0)
    {
    }

    virtual void Serialize(UnidentifiedMessageSerializer* serializer);
    virtual ~NetworkMessageType30_8050ADA4();
    virtual int GetType();

    /* 0x08 */ u32 mUnidentified08;
};

class NetworkMessageType31_8050AD90 : public UnidentifiedNetworkMessage
{
public:
    NetworkMessageType31_8050AD90()
        : mUnidentified08(0)
    {
    }

    virtual void Serialize(UnidentifiedMessageSerializer* serializer);
    virtual ~NetworkMessageType31_8050AD90();
    virtual int GetType();

    /* 0x08 */ u32 mUnidentified08;
};

// Periodic state for one game in the online tournament bracket. The message
// carries a full BasicGameInfo only when mHasGameInfo is set.
class NetMessageTournamentGameUpdate : public UnidentifiedNetworkMessage
{
public:
    virtual void Serialize(UnidentifiedMessageSerializer* serializer);
    virtual ~NetMessageTournamentGameUpdate();
    virtual int GetType();

    /* 0x008 */ u8 mUpdateType;
    /* 0x009 */ u8 mGameIndex;
    /* 0x00A */ u8 mIsHomeMachine;
    /* 0x00B */ u8 mGameStatus;
    /* 0x00C */ u16 mGameTimeDelta;
    /* 0x00E */ u8 mHasGameInfo;
    /* 0x00F */ u8 mUnidentified0F;
    /* 0x010 */ u8 mGameInfo[0x128];
}; // size: 0x138

// Per-machine loading notification used while moving between a tournament
// matchup and the knockout presentation.
class NetMessageTournamentLoadingState : public UnidentifiedNetworkMessage
{
public:
    virtual void Serialize(UnidentifiedMessageSerializer* serializer);
    virtual ~NetMessageTournamentLoadingState();
    virtual int GetType();

    /* 0x08 */ u8 mMachineIndex;
    /* 0x09 */ u8 mFinishedLoadingToKnockout;
};

class NetworkMessageType34_8050ADCC : public UnidentifiedNetworkMessage
{
public:
    virtual void Serialize(UnidentifiedMessageSerializer* serializer);
    virtual ~NetworkMessageType34_8050ADCC();
    virtual int GetType();

    /* 0x08 */ u16 mUnidentified08;
    /* 0x0A */ u16 mUnidentified0A;
    /* 0x0C */ u8 mUnidentified0C;
    /* 0x0D */ u8 mUnidentified0D;
    /* 0x0E */ u8 mUnidentified0E;
};

class UnidentifiedNetworkMessage_80126D84 : public UnidentifiedNetworkMessage
{
public:
    UnidentifiedNetworkMessage_80126D84()
        : mCount(0)
    {
        for (int i = 0; i < 8; ++i)
        {
            mValues[i] = false;
        }
    }

    virtual void Serialize(UnidentifiedMessageSerializer* serializer);
    virtual ~UnidentifiedNetworkMessage_80126D84() { }
    virtual int GetType();

    /* 0x08 */ u8 mCount;
    /* 0x09 */ bool mValues[8];
}; // size: 0x14

#endif // GAME_NETWORK_MESSAGES_H
