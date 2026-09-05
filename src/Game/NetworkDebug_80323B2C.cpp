#include "Game/NetworkDebug_80323B2C.h"

#include "Game/NetworkSession.h"
#include "Game/UnidentifiedTweakValue_8052BED0.h"
#include "NL/gl/glFont.h"
#include "unclassified/tu_802B7798.h"
#include "unclassified/tu_80332DC0.h"

UnidentifiedNetworkSession* lbl_806E20D8;
u8 lbl_806E20DC;
int lbl_806E20E0;
bool g_bDisplayNetwork;
bool g_bDisplayNetworkVerbose;
bool g_bDirectConnectMode;

int lbl_805317E8[4] = { 192, 168, 2, 218 };
int g_nConnectToServerPort = 1000;
static const char* lbl_80531810[] = {
    "None", "LogIn", "Matchmake", "PreStart", "Loading", "InGame", "GameEnd"
};
int lbl_806DF668 = 40;
int lbl_806DF66C = 19;
int lbl_806DF670 = 28;

void UnidentifiedNetworkSessionBase::BaseVirtual50()
{
    NetworkSocket_801246E4* socket = GetDirectSocket();
    glFontSetFont(0);
    glFontBegin(false);
    int row = g_bDisplayNetworkVerbose ? lbl_806DF66C : lbl_806DF670;
    if (g_bDisplayNetworkVerbose)
    {
        const char* mode = "None";
        switch (OnlineVirtual0C())
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
        int state = OnlineVirtual10();
        glFontPrintf(GetDebugFontView(), lbl_806DF668, row++, "Mode: %s %s", mode, lbl_80531810[state]);
    }
    UnidentifiedMachineRoster* roster = GetMachineRoster();
    if (roster != 0)
    {
        roster->DebugDraw(lbl_806DF668, &row);
    }
    UnidentifiedInputRouter* router = fn_803330AC();
    if (router != 0)
    {
        router->RouterVirtual30(lbl_806DF668, &row);
    }
    if (socket != 0)
    {
        socket->SocketVirtual44(lbl_806DF668, &row, g_bDisplayNetworkVerbose);
    }
    glFontEnd();
    if (socket != 0)
    {
        socket->SocketVirtual48();
    }
}

extern "C" void fn_80323D04()
{
    UnidentifiedMachineRoster* roster = lbl_806E20D8->GetMachineRoster();
    if (roster != 0)
    {
        roster->RosterVirtual0C(1);
    }
}

extern "C" void fn_80323D50()
{
    UnidentifiedMachineRoster* roster = lbl_806E20D8->GetMachineRoster();
    if (roster != 0)
    {
        roster->RosterVirtual0C(0);
    }
}

static TweakValueBoolImpl_804F4538 lbl_805843E0("g_bDisplayNetwork", "Network", &g_bDisplayNetwork, true);
static TweakValueBoolImpl_804F4538 lbl_80584400("g_bDisplayNetworkVerbose", "Network", &g_bDisplayNetworkVerbose, true);
static TweakValueBoolImpl_804F4538 lbl_80584420("g_bDirectConnectMode", "Network", &g_bDirectConnectMode, true);
static TweakValueIntImpl_804FD898 lbl_80584440("g_nConnectToServerAddress0", "Network", &lbl_805317E8[0], true);
static TweakValueIntImpl_804FD898 lbl_80584460("g_nConnectToServerAddress1", "Network", &lbl_805317E8[1], true);
static TweakValueIntImpl_804FD898 lbl_80584480("g_nConnectToServerAddress2", "Network", &lbl_805317E8[2], true);
static TweakValueIntImpl_804FD898 lbl_805844A0("g_nConnectToServerAddress3", "Network", &lbl_805317E8[3], true);
static TweakValueIntImpl_804FD898 lbl_805844C0("g_nConnectToServerPort", "Network", &g_nConnectToServerPort, true);
static UnidentifiedTweakValue_8052BED0 lbl_805844E0("Set Client Server Mode", "Network", fn_80323D04, true);
static UnidentifiedTweakValue_8052BED0 lbl_80584508("Set Peer Peer Mode", "Network", fn_80323D50, true);
