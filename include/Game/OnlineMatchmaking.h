#ifndef GAME_ONLINE_MATCHMAKING_H
#define GAME_ONLINE_MATCHMAKING_H

#include "NL/CircularQueue.h"
#include "types.h"

extern u8 gOnlineStartMatchmaking;
extern int gOnlineMaxMatchmakingEntries;
extern int gOnlineMinMatchmakingEntries;
extern u8 gOnlineSidekickChoiceSent;
extern u8 gOnlineUnrankedMatch;
extern StaticCircularQueue<unsigned int, 3> gRejectedOpponentProfileIds;

extern unsigned char gOnlineLoginStarted;

#endif // GAME_ONLINE_MATCHMAKING_H
