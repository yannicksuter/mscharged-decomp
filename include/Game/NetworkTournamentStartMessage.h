#ifndef GAME_NETWORK_TOURNAMENT_START_MESSAGE_H
#define GAME_NETWORK_TOURNAMENT_START_MESSAGE_H

#include "Game/NetworkMessage.h"

// "Failed to SendTournamentStartToEveryone to %d because no connection".
class NetMessageTournamentStart : public NetworkMessage
{
public:
    virtual void Serialize(NetworkMessageSerializer* serializer);
    virtual ~NetMessageTournamentStart() { }
    virtual int GetType() { return 20; }

    /* 0x08 */ u8 mMachineIndex;
    /* 0x09 */ u8 mMachineCount;
    /* 0x0A */ u8 mCupPersona;
    /* 0x0B */ u8 mFirstStadium;
    /* 0x0C */ u8 mSecondStadium;
    /* 0x0D */ u8 mSeedings[8];
};

#endif // GAME_NETWORK_TOURNAMENT_START_MESSAGE_H
