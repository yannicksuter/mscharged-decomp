#include "Game/Task/TweakerTask.h"

#include "Game/NetworkSession.h"
#include "Game/NetworkDebug.h"
#include "Game/NetworkSync.h"

#include "types.h"

extern s32 lbl_806DF2E0;
extern s32 lbl_806DF2E4;
extern s32 lbl_806DF2E8;
extern s32 lbl_806DF2F0;
extern s32 lbl_806DF2F4;
extern s32 lbl_806DF2F8;
extern s32 lbl_806DF2FC;

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
