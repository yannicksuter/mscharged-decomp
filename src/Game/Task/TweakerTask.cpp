#include "Game/Task/TweakerTask.h"

#include "Game/NetworkSession.h"
#include "Game/NetworkDebug.h"
#include "Game/NetworkSync.h"

#include "types.h"
#include "Game/UnidentifiedStaticStorage.h"

s32 lbl_806DF2E0 = -1;
s32 lbl_806DF2E4 = -1;
s32 lbl_806DF2E8 = -1;
s32 lbl_806DF2EC = -1;
s32 lbl_806DF2F0 = -1;
s32 lbl_806DF2F4 = -1;
s32 lbl_806DF2F8 = -1;
s32 lbl_806DF2FC = -1;

bool g_bTweaking;

void fn_802BDA28()
{
    g_bTweaking = !g_bTweaking;
}

void TweakerTask::Run(float)
{
    if (lbl_806DF2E0 == -1
        || lbl_806DF2E4 == -1
        || lbl_806DF2E8 == -1
        || lbl_806DF2F0 == -1
        || lbl_806DF2F4 == -1
        || lbl_806DF2F8 == -1
        || lbl_806DF2FC == -1)
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
