#include "Game/NetworkDebug.h"

#include "Game/NetworkSession.h"
#include "Game/UnidentifiedTweakValue_8052BED0.h"
#include "NL/gl/glFont.h"
#include "unclassified/tu_802B7798.h"
#include "unclassified/tu_80332DC0.h"

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
    NetworkSocket_801246E4* socket = GetDirectSocket();
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
    UnidentifiedMachineRoster* roster = GetMachineRoster();
    if (roster != 0)
    {
        roster->DebugDraw(gNetworkDebugColumn, &row);
    }
    UnidentifiedInputRouter* router = fn_803330AC();
    if (router != 0)
    {
        router->RouterVirtual30(gNetworkDebugColumn, &row);
    }
    if (socket != 0)
    {
        socket->SocketVirtual44(gNetworkDebugColumn, &row, g_bDisplayNetworkVerbose);
    }
    glFontEnd();
    if (socket != 0)
    {
        socket->SocketVirtual48();
    }
}

void SetClientServerMode()
{
    UnidentifiedMachineRoster* roster = g_pNetworkSessionBase->GetMachineRoster();
    if (roster != 0)
    {
        roster->RosterVirtual0C(1);
    }
}

void SetPeerToPeerMode()
{
    UnidentifiedMachineRoster* roster = g_pNetworkSessionBase->GetMachineRoster();
    if (roster != 0)
    {
        roster->RosterVirtual0C(0);
    }
}

static TweakValueBoolImpl_804F4538 sDisplayNetworkTweak("g_bDisplayNetwork", "Network", &g_bDisplayNetwork, true);
static TweakValueBoolImpl_804F4538 sDisplayNetworkVerboseTweak("g_bDisplayNetworkVerbose", "Network", &g_bDisplayNetworkVerbose, true);
static TweakValueBoolImpl_804F4538 sDirectConnectModeTweak("g_bDirectConnectMode", "Network", &g_bDirectConnectMode, true);
static TweakValueIntImpl_804FD898 sConnectToServerAddress0Tweak("g_nConnectToServerAddress0", "Network", &g_nConnectToServerAddress[0], true);
static TweakValueIntImpl_804FD898 sConnectToServerAddress1Tweak("g_nConnectToServerAddress1", "Network", &g_nConnectToServerAddress[1], true);
static TweakValueIntImpl_804FD898 sConnectToServerAddress2Tweak("g_nConnectToServerAddress2", "Network", &g_nConnectToServerAddress[2], true);
static TweakValueIntImpl_804FD898 sConnectToServerAddress3Tweak("g_nConnectToServerAddress3", "Network", &g_nConnectToServerAddress[3], true);
static TweakValueIntImpl_804FD898 sConnectToServerPortTweak("g_nConnectToServerPort", "Network", &g_nConnectToServerPort, true);
static UnidentifiedTweakValue_8052BED0 sClientServerModeTweak("Set Client Server Mode", "Network", SetClientServerMode, true);
static UnidentifiedTweakValue_8052BED0 sPeerToPeerModeTweak("Set Peer Peer Mode", "Network", SetPeerToPeerMode, true);
