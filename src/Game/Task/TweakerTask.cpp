#include "Game/Task/TweakerTask.h"

#include "Game/NetworkSession.h"
#include "Game/NetworkDebug.h"
#include "Game/NetworkSync.h"

#include "types.h"
#include "Game/UnidentifiedStaticStorage.h"

s32 gTweakerAccelButton = -1;
s32 gTweakerBackButton = -1;
s32 gTweakerToggleButton = -1;
s32 gTweakerModifierButton = -1;
s32 gTweakerLeftButton = -1;
s32 gTweakerRightButton = -1;
s32 gTweakerUpButton = -1;
s32 gTweakerDownButton = -1;

bool g_bTweaking;

void ToggleTweaking()
{
    g_bTweaking = !g_bTweaking;
}

void TweakerTask::Run(float)
{
    if (gTweakerAccelButton == -1
        || gTweakerBackButton == -1
        || gTweakerToggleButton == -1
        || gTweakerLeftButton == -1
        || gTweakerRightButton == -1
        || gTweakerUpButton == -1
        || gTweakerDownButton == -1)
    {
        return;
    }

    if (g_bDisplayNetwork && g_pNetworkSessionBase != 0)
    {
        g_pNetworkSessionBase->DebugDraw();
    }

    if (gNetworkSyncState != 0)
    {
        gNetworkSyncState->DebugDraw();
    }
}
