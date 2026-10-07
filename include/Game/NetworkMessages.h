#ifndef GAME_NETWORK_MESSAGES_H
#define GAME_NETWORK_MESSAGES_H

#include "Game/DB/BasicGameInfo.h"
#include "Game/NetworkMessage.h"
#include "types.h"
#include "Game/NetworkStatsManager.h"

#include "Game/NetworkTournamentStartMessage.h"

// "Failed to SendGameStartToEveryone to %d because no connection".
class NetMessageGameStart : public NetworkMessage
{
public:
    virtual void Serialize(NetworkMessageSerializer* serializer);
    virtual int GetType();

    /* 0x08 */ u32 mRandomSeed;
    /* 0x0C */ u8 mMachineIndex;
    /* 0x0D */ u8 mMachineCount;
    /* 0x0E */ u8 mHomeCharacters[4];
    /* 0x12 */ u8 mAwayCharacters[4];
    /* 0x16 */ u8 mStadium;
    /* 0x17 */ u8 mMachinePlayerCounts[4];
    /* 0x1B */ u8 mTournamentGame;
    // Bracket game index, followed by home and away machine indices.
    /* 0x1C */ s8 mTournamentSetup[3];
    /* 0x20 */ u32 mSecondGameRandomSeed;
    /* 0x24 */ u32 mThirdGameRandomSeed;
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
        , mChooseSides(0)
    {
        for (int i = 0; i < 8; ++i)
        {
            mPlayerSides.mData[i / 2][i % 2] = -1;
        }
    }

    virtual void Serialize(NetworkMessageSerializer* serializer);
    virtual int GetType() { return 21; }

    /* 0x008 */ s8 mMachineIndex;
    /* 0x009 */ s8 mMachineCount;
    /* 0x00A */ u8 mChooseSides;
    /* 0x00B */ NetworkDraftSides mPlayerSides;
    /* 0x014 */ NetworkDraftMachineInfo mEntries[8];
}; // size: 0x414

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

#include "Game/NetworkLoadedGameMessages.h"

class NetMessageDraftMachineInfo : public NetworkMessage
{
public:
    virtual void Serialize(NetworkMessageSerializer* serializer);
    virtual int GetType() { return 22; }

    /* 0x008 */ NetworkDraftMachineInfo mEntry;
}; // size: 0x88

// "Sending NetworkDraftPickedCaptain team %d captain %d".
class NetMessageDraftPickedCaptain : public NetworkMessage
{
public:
    virtual void Serialize(NetworkMessageSerializer* serializer);
    virtual int GetType() { return 23; }

    /* 0x08 */ s8 mTeamIndex;
    /* 0x09 */ u8 mCaptain;
};

// "Ignoring ReceivedDraftPickedSidekicks because in draft state %d".
class NetMessageDraftPickedSidekicks : public NetworkMessage
{
public:
    virtual void Serialize(NetworkMessageSerializer* serializer);
    virtual int GetType() { return 24; }

    /* 0x08 */ u8 mTeamIndex;
    /* 0x09 */ u8 mSidekick0;
    /* 0x0A */ u8 mSidekick1;
    /* 0x0B */ u8 mSidekick2;
};

class NetMessageSidesChanged : public NetworkMessage
{
public:
    virtual void Serialize(NetworkMessageSerializer* serializer);
    virtual int GetType() { return 25; }

    /* 0x08 */ u8 mMachineIndex;
    /* 0x09 */ s8 mSide;
    /* 0x0A */ bool mGuest;
    /* 0x0B */ u8 mAccepted;
};

// "Failed to SendCheckConnectionToEveryone to %d because no connection".
class NetMessageCheckConnection : public NetworkMessage
{
public:
    virtual void Serialize(NetworkMessageSerializer* serializer);
    virtual int GetType() { return 26; }

    /* 0x08 */ u32 mProfileIds[2];
};

class NetMessageConnectionDecision : public NetworkMessage
{
public:
    virtual void Serialize(NetworkMessageSerializer* serializer);
    virtual int GetType() { return 27; }

    /* 0x08 */ u8 mAccepted;
    /* 0x09 */ s8 mMachineIndex;
};

#include "Game/NetworkPauseMessages.h"

class NetMessageSkipNis : public NetworkMessage
{
public:
    NetMessageSkipNis()
        : mByPassNumber(0)
    {
    }
    NetMessageSkipNis(u32 byPassNumber)
        : mByPassNumber(byPassNumber)
    {
    }

    virtual void Serialize(NetworkMessageSerializer* serializer);
    virtual int GetType() { return 30; }

    /* 0x08 */ u32 mByPassNumber;
};

class NetMessageSkipNisClient : public NetworkMessage
{
public:
    NetMessageSkipNisClient()
        : mByPassNumber(0)
    {
    }
    NetMessageSkipNisClient(u32 byPassNumber)
        : mByPassNumber(byPassNumber)
    {
    }

    virtual void Serialize(NetworkMessageSerializer* serializer);
    virtual int GetType() { return 31; }

    /* 0x08 */ u32 mByPassNumber;
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
    virtual int GetType() { return 32; }

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
    virtual int GetType() { return 33; }

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

class NetMessageMegaStrikeMeter : public NetworkMessage
{
public:
    NetMessageMegaStrikeMeter()
        : mCount(0)
    {
        for (int i = 0; i < 8; ++i)
        {
            mValues[i] = false;
        }
    }

    virtual void Serialize(NetworkMessageSerializer* serializer);
    virtual ~NetMessageMegaStrikeMeter() { }
    virtual int GetType();

    /* 0x08 */ u8 mCount;
    /* 0x09 */ bool mValues[8];
}; // size: 0x14

void RegisterNetworkMessageFactories();

#endif // GAME_NETWORK_MESSAGES_H
