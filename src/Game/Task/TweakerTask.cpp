#include "Game/Task/TweakerTask.h"

#include "types.h"

struct SharedSubsystem
{
    u8 padding[0x242C];
    virtual void Unknown0();
    virtual void Unknown1();
    virtual void Unknown2();
    virtual void Unknown3();
    virtual void Unknown4();
    virtual void Unknown5();
    virtual void Unknown6();
    virtual void Unknown7();
    virtual void Unknown8();
    virtual void Unknown9();
    virtual void Unknown10();
    virtual void Unknown11();
    virtual void Unknown12();
    virtual void Unknown13();
    virtual void Unknown14();
    virtual void Unknown15();
    virtual void Unknown16();
    virtual void Unknown17();
    virtual void Unknown18();
};

extern s32 lbl_806DF2E0;
extern s32 lbl_806DF2E4;
extern s32 lbl_806DF2E8;
extern s32 lbl_806DF2F0;
extern s32 lbl_806DF2F4;
extern s32 lbl_806DF2F8;
extern s32 lbl_806DF2FC;
extern u8 lbl_806E20E4;
extern SharedSubsystem* lbl_806E20D8;
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
        lbl_806E20D8->Unknown18();
    }

    if (lbl_806E2168 != 0)
    {
        fn_80338AE4(lbl_806E2168);
    }
}
