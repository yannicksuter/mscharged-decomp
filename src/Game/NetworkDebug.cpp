#include "Game/NetworkDebug.h"
#include "Game/OnlinePlayer.h"
#include <RVLFaceLib/RFL_Types.h>

#include "Game/NetworkSession.h"
#include "Game/TweakCallback.h"
#include "Game/UnidentifiedStaticStorage.h"
#include "NL/gl/glFont.h"
#include "NL/nlDebugViews.h"
#include "Game/InputRouter.h"

char gNetworkMiiName[12];
unsigned short gNetworkMiiNameWide[12];
RFLStoreData gNetworkMiiData;

NetworkSessionBase* g_pNetworkSessionBase;
u8 gNetworkMiiChanged;
int gNetworkSaveSlotIndex;
bool g_bDisplayNetwork;
bool g_bDisplayNetworkVerbose;
bool g_bDirectConnectMode;

int g_nConnectToServerAddress[4] = { 192, 168, 2, 218 };
int g_nConnectToServerPort = 1000;
static const char* sNetworkSessionStateNames[] = {
    "None", "LogIn", "Matchmake", "PreStart", "Loading", "InGame", "GameEnd"
};
int gNetworkDebugColumn = 40;
int gNetworkDebugVerboseRow = 19;
int gNetworkDebugRow = 28;

void NetworkSessionBase::DebugDraw()
{
    NetworkSocket* socket = GetDirectSocket();
    glFontSetFont(0);
    glFontBegin(false);
    int row = g_bDisplayNetworkVerbose ? gNetworkDebugVerboseRow : gNetworkDebugRow;
    if (g_bDisplayNetworkVerbose)
    {
        const char* mode = "None";
        switch (GetSessionMode())
        {
        case 0:
            mode = "Local";
            break;
        case 1:
            if (g_bDirectConnectMode)
            {
                mode = "LAN_D";
            }
            else
            {
                mode = "LAN";
            }
            break;
        case 2:
            mode = "INT";
            break;
        }
        int state = GetSessionState();
        glFontPrintf(GetDebugFontView(), gNetworkDebugColumn, row++, "Mode: %s %s", mode, sNetworkSessionStateNames[state]);
    }
    NetworkMachineRoster* roster = GetMachineRoster();
    if (roster != 0)
    {
        roster->DebugDraw(gNetworkDebugColumn, &row);
    }
    InputRouter* router = GetInputRouter();
    if (router != 0)
    {
        router->DebugDraw(gNetworkDebugColumn, &row);
    }
    if (socket != 0)
    {
        socket->DebugDraw(gNetworkDebugColumn, &row, g_bDisplayNetworkVerbose);
    }
    glFontEnd();
    if (socket != 0)
    {
        socket->SocketVirtual48();
    }
}

void SetClientServerMode()
{
    NetworkMachineRoster* roster = g_pNetworkSessionBase->GetMachineRoster();
    if (roster != 0)
    {
        roster->SetTopology(1);
    }
}

void SetPeerToPeerMode()
{
    NetworkMachineRoster* roster = g_pNetworkSessionBase->GetMachineRoster();
    if (roster != 0)
    {
        roster->SetTopology(0);
    }
}

static TweakBoolBinding sDisplayNetworkTweak("g_bDisplayNetwork", "Network", &g_bDisplayNetwork, true);
static TweakBoolBinding sDisplayNetworkVerboseTweak("g_bDisplayNetworkVerbose", "Network", &g_bDisplayNetworkVerbose, true);
static TweakBoolBinding sDirectConnectModeTweak("g_bDirectConnectMode", "Network", &g_bDirectConnectMode, true);
static TweakIntBinding sConnectToServerAddress0Tweak("g_nConnectToServerAddress0", "Network", &g_nConnectToServerAddress[0], true);
static TweakIntBinding sConnectToServerAddress1Tweak("g_nConnectToServerAddress1", "Network", &g_nConnectToServerAddress[1], true);
static TweakIntBinding sConnectToServerAddress2Tweak("g_nConnectToServerAddress2", "Network", &g_nConnectToServerAddress[2], true);
static TweakIntBinding sConnectToServerAddress3Tweak("g_nConnectToServerAddress3", "Network", &g_nConnectToServerAddress[3], true);
static TweakIntBinding sConnectToServerPortTweak("g_nConnectToServerPort", "Network", &g_nConnectToServerPort, true);
static TweakCallback sClientServerModeTweak("Set Client Server Mode", "Network", SetClientServerMode, true);
static TweakCallback sPeerToPeerModeTweak("Set Peer Peer Mode", "Network", SetPeerToPeerMode, true);
