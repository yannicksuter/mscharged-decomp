#ifndef GAME_NETWORK_DEBUG_H
#define GAME_NETWORK_DEBUG_H

extern bool g_bDisplayNetwork;
extern bool g_bDisplayNetworkVerbose;
extern bool g_bDirectConnectMode;
extern int g_nConnectToServerAddress[4];
extern int g_nConnectToServerPort;
extern int g_nConnectToServerAddress[4];

void SetClientServerMode();
void SetPeerToPeerMode();

#endif // GAME_NETWORK_DEBUG_H
