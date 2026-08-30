#include "Game/Task/TweakerTask.h"

#include "Game/NetworkSession.h"

#include "types.h"

extern s32 lbl_806DF2E0;
extern s32 lbl_806DF2E4;
extern s32 lbl_806DF2E8;
extern s32 lbl_806DF2F0;
extern s32 lbl_806DF2F4;
extern s32 lbl_806DF2F8;
extern s32 lbl_806DF2FC;
extern u8 lbl_806E20E4;
extern void* lbl_806E2168;

extern "C" void fn_80338AE4(void*);

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

    if (lbl_806E20E4 && lbl_806E20D8 != 0)
    {
        lbl_806E20D8->BaseVirtual50();
    }

    if (lbl_806E2168 != 0)
    {
        fn_80338AE4(lbl_806E2168);
    }
}
