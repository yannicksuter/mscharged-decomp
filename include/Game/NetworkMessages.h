#ifndef GAME_NETWORK_MESSAGES_H
#define GAME_NETWORK_MESSAGES_H

#include "Game/DB/BasicGameInfo.h"
#include "Game/NetworkMessage.h"
#include "types.h"
#include "Game/NetworkStatsManager.h"

// "Failed to SendTournamentStartToEveryone to %d because no connection".
class NetMessageTournamentStart : public NetworkMessage
{
public:
    virtual void Serialize(NetworkMessageSerializer* serializer);
    virtual ~NetMessageTournamentStart() { }
    virtual int GetType();

    /* 0x08 */ u8 mMachineIndex;
    /* 0x09 */ u8 mMachineCount;
    /* 0x0A */ u8 mCupPersona;
    /* 0x0B */ u8 mFirstStadium;
    /* 0x0C */ u8 mSecondStadium;
    /* 0x0D */ u8 mSeedings[8];
};

// "Failed to SendGameStartToEveryone to %d because no connection".
class NetMessageGameStart : public NetworkMessage
{
public:
    virtual void Serialize(NetworkMessageSerializer* serializer);
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

struct NetworkDraftSides
{
    s8 mData[4][2];
};

struct NetworkDraftMachineInfo
{
    NetworkDraftMachineInfo()
    {
        mGuestEnabled = 0;
    }

    /* 0x00 */ NetworkRankingMeta mStats;
    /* 0x18 */ u32 mProfileId;
    /* 0x1C */ u16 mName[11];
    /* 0x32 */ u8 mMiiData[0x4C];
    /* 0x7E */ s8 mMachineIndex;
    /* 0x7F */ u8 mGuestEnabled;
}; // size: 0x80

// "Failed to SendDraftToEveryone to %d because no connection".
class NetMessageDraft : public NetworkMessage
{
public:
    NetMessageDraft()
        : mMachineIndex(-1)
        , mMachineCount(-1)
        , mUnidentified0A(0)
    {
        for (int i = 0; i < 8; ++i)
        {
            mPlayerSides.mData[i / 2][i % 2] = -1;
        }
    }

    virtual void Serialize(NetworkMessageSerializer* serializer);
    virtual ~NetMessageDraft() { }
    virtual int GetType();

    /* 0x008 */ s8 mMachineIndex;
    /* 0x009 */ s8 mMachineCount;
    /* 0x00A */ u8 mUnidentified0A;
    /* 0x00B */ NetworkDraftSides mPlayerSides;
    /* 0x014 */ NetworkDraftMachineInfo mEntries[8];
}; // size: 0x414

// "Failed to SendCheckConnectionToEveryone to %d because no connection".
class NetMessageCheckConnection : public NetworkMessage
{
public:
    virtual void Serialize(NetworkMessageSerializer* serializer);
    virtual ~NetMessageCheckConnection() { }
    virtual int GetType();

    /* 0x08 */ u32 mProfileIds[2];
};

// Message IDs 16 and 17 are registered by this translation unit, but no
// surviving behavior-level name has yet been established for either payload.
class NetworkMessageType16 : public NetworkMessage
{
public:
    virtual void Serialize(NetworkMessageSerializer* serializer);
    virtual ~NetworkMessageType16() { }
    virtual int GetType();
};

class NetworkMessageType17 : public NetworkMessage
{
public:
    virtual void Serialize(NetworkMessageSerializer* serializer);
    virtual ~NetworkMessageType17() { }
    virtual int GetType();

    /* 0x08 */ u32 mUnidentified08;
};

// Payload-less loaded-game notifications. Every virtual is inline, so the
// vtables and retained weak copies belong to the session translation unit.
class NetMessageLoadedGame : public NetworkMessage
{
public:
    virtual void Serialize(NetworkMessageSerializer*) { }
    virtual ~NetMessageLoadedGame() { }
    virtual int GetType() { return 0xF; }
};

class NetMessageLoadedGameClient : public NetworkMessage
{
public:
    virtual void Serialize(NetworkMessageSerializer*) { }
    virtual ~NetMessageLoadedGameClient() { }
    virtual int GetType() { return 0x12; }
};

class NetMessageLoadedGameEveryone : public NetworkMessage
{
public:
    virtual void Serialize(NetworkMessageSerializer*) { }
    virtual ~NetMessageLoadedGameEveryone() { }
    virtual int GetType() { return 0x13; }
};

class NetMessageDraftMachineInfo : public NetworkMessage
{
public:
    virtual void Serialize(NetworkMessageSerializer* serializer);
    virtual ~NetMessageDraftMachineInfo() { }
    virtual int GetType();

    /* 0x008 */ NetworkDraftMachineInfo mEntry;
}; // size: 0x88

// "Sending NetworkDraftPickedCaptain team %d captain %d".
class NetMessageDraftPickedCaptain : public NetworkMessage
{
public:
    virtual void Serialize(NetworkMessageSerializer* serializer);
    virtual ~NetMessageDraftPickedCaptain();
    virtual int GetType();

    /* 0x08 */ s8 mTeamIndex;
    /* 0x09 */ u8 mCaptain;
};

// "Ignoring ReceivedDraftPickedSidekicks because in draft state %d".
class NetMessageDraftPickedSidekicks : public NetworkMessage
{
public:
    virtual void Serialize(NetworkMessageSerializer* serializer);
    virtual ~NetMessageDraftPickedSidekicks();
    virtual int GetType();

    /* 0x08 */ u8 mTeamIndex;
    /* 0x09 */ u8 mSidekick0;
    /* 0x0A */ u8 mSidekick1;
    /* 0x0B */ u8 mSidekick2;
};

class NetMessageSidesChanged : public NetworkMessage
{
public:
    virtual void Serialize(NetworkMessageSerializer* serializer);
    virtual ~NetMessageSidesChanged() { }
    virtual int GetType();

    /* 0x08 */ u8 mMachineIndex;
    /* 0x09 */ u8 mSide;
    /* 0x0A */ bool mGuest;
    /* 0x0B */ u8 mAccepted;
};

class NetMessageConnectionDecision : public NetworkMessage
{
public:
    virtual void Serialize(NetworkMessageSerializer* serializer);
    virtual ~NetMessageConnectionDecision();
    virtual int GetType();

    /* 0x08 */ u8 mAccepted;
    /* 0x09 */ s8 mMachineIndex;
};

class NetMessagePauseRequest : public NetworkMessage
{
public:
    virtual void Serialize(NetworkMessageSerializer* serializer);
    virtual ~NetMessagePauseRequest();
    virtual int GetType();

    /* 0x08 */ u8 mMachineIndex;
    /* 0x09 */ u8 mPaused;
};

// "HOST sending Pause Response to all clients and myself".
class NetMessagePauseResponse : public NetworkMessage
{
public:
    virtual void Serialize(NetworkMessageSerializer* serializer);
    virtual ~NetMessagePauseResponse() { }
    virtual int GetType();

    /* 0x08 */ u8 mMachineMask;
};

class NetworkMessageType30 : public NetworkMessage
{
public:
    NetworkMessageType30()
        : mUnidentified08(0)
    {
    }
    NetworkMessageType30(u32 value)
        : mUnidentified08(value)
    {
    }

    virtual void Serialize(NetworkMessageSerializer* serializer);
    virtual int GetType() { return 30; }

    /* 0x08 */ u32 mUnidentified08;
};

class NetworkMessageType31 : public NetworkMessage
{
public:
    NetworkMessageType31()
        : mUnidentified08(0)
    {
    }
    NetworkMessageType31(u32 value)
        : mUnidentified08(value)
    {
    }

    virtual void Serialize(NetworkMessageSerializer* serializer);
    virtual int GetType() { return 31; }

    /* 0x08 */ u32 mUnidentified08;
};

// Periodic state for one game in the online tournament bracket. The message
// carries a full BasicGameInfo only when mHasGameInfo is set.
class NetMessageTournamentGameUpdate : public NetworkMessage
{
public:
    NetMessageTournamentGameUpdate() { }
    NetMessageTournamentGameUpdate(u8 updateType, u8 gameIndex,
        bool isHomeMachine, u8 gameStatus, u16 gameTimeDelta,
        bool hasGameInfo)
        : mUpdateType(updateType)
        , mGameIndex(gameIndex)
        , mIsHomeMachine(isHomeMachine)
        , mGameStatus(gameStatus)
        , mGameTimeDelta(gameTimeDelta)
        , mHasGameInfo(hasGameInfo)
        , mUnidentified0F(0)
    {
    }

    virtual void Serialize(NetworkMessageSerializer* serializer);
    virtual ~NetMessageTournamentGameUpdate();
    virtual int GetType();

    /* 0x008 */ u8 mUpdateType;
    /* 0x009 */ u8 mGameIndex;
    /* 0x00A */ u8 mIsHomeMachine;
    /* 0x00B */ u8 mGameStatus;
    /* 0x00C */ u16 mGameTimeDelta;
    /* 0x00E */ u8 mHasGameInfo;
    /* 0x00F */ u8 mUnidentified0F;
    /* 0x010 */ BasicGameInfo mGameInfo;
}; // size: 0x138

// Per-machine loading notification used while moving between a tournament
// matchup and the knockout presentation.
class NetMessageTournamentLoadingState : public NetworkMessage
{
public:
    NetMessageTournamentLoadingState() { }
    NetMessageTournamentLoadingState(
        int machineIndex, bool finishedLoadingToKnockout)
        : mMachineIndex(machineIndex)
        , mFinishedLoadingToKnockout(finishedLoadingToKnockout)
    {
    }

    virtual void Serialize(NetworkMessageSerializer* serializer);
    virtual ~NetMessageTournamentLoadingState();
    virtual int GetType();

    /* 0x08 */ s8 mMachineIndex;
    /* 0x09 */ u8 mFinishedLoadingToKnockout;
};

class NetMessageMegaBallPointer : public NetworkMessage
{
public:
    virtual void Serialize(NetworkMessageSerializer* serializer);
    virtual ~NetMessageMegaBallPointer() { }
    virtual int GetType();

    /* 0x08 */ s16 mPointerX;
    /* 0x0A */ s16 mPointerY;
    /* 0x0C */ u8 mAngleHighByte;
    /* 0x0D */ u8 mTextureIndex;
    /* 0x0E */ u8 mStatus;
};

class NetworkMessageType35 : public NetworkMessage
{
public:
    NetworkMessageType35()
        : mCount(0)
    {
        for (int i = 0; i < 8; ++i)
        {
            mValues[i] = false;
        }
    }

    virtual void Serialize(NetworkMessageSerializer* serializer);
    virtual ~NetworkMessageType35() { }
    virtual int GetType();

    /* 0x08 */ u8 mCount;
    /* 0x09 */ bool mValues[8];
}; // size: 0x14

#endif // GAME_NETWORK_MESSAGES_H
