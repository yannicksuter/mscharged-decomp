#ifndef GAME_NETWORK_DEBUG_H
#define GAME_NETWORK_DEBUG_H

struct RFLStoreData;
class NetworkSessionBase;

extern NetworkSessionBase* g_pNetworkSessionBase;

extern char gNetworkMiiName[12];
extern unsigned short gNetworkMiiNameWide[11];
extern RFLStoreData gNetworkMiiData;
extern int gNetworkSaveSlotIndex;
extern unsigned char gNetworkMiiChanged;

extern bool g_bDisplayNetwork;
extern bool g_bDisplayNetworkVerbose;
extern bool g_bDirectConnectMode;
extern int g_nConnectToServerAddress[4];
extern int g_nConnectToServerPort;

void SetClientServerMode();
void SetPeerToPeerMode();

#endif // GAME_NETWORK_DEBUG_H
