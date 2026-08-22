#include "Game/Task/EndFrameTask.h"

#include "Game/Debug/FrameCounter.h"
#include "NL/gl/gl.h"
#include "types.h"

struct RenderContext
{
    u8 padding[0x33];
    u8 enabled;
    u32 flags;
};

struct FrameControl
{
    u8 padding[0x90];
    bool flag90;
    bool flag91;
};

extern FrameControl* lbl_806E18D8;
extern u8 lbl_806E16D4;
extern u8 lbl_806E0FB0;

extern "C" RenderContext* fn_8027267C(int index);
extern "C" void fn_801B3EF4(u8* enabled);
extern "C" void fn_801B3F2C(u8* enabled);
extern "C" int fn_8024891C();
extern "C" void fn_803A38E4(u16 x, u16 y, u32 colour);

void EndFrameTask::Run(float)
{
    if (lbl_806E18D8 == 0 || !lbl_806E18D8->flag91 || !lbl_806E18D8->flag90)
    {
        u8 useFrameContexts = lbl_806E16D4;

        RenderContext* context = fn_8027267C(30);
        u32 contextFlags = 0;
        if (useFrameContexts)
        {
            contextFlags = 8;
        }
        context->flags = contextFlags;

        context = fn_8027267C(31);
        contextFlags = 0;
        if (useFrameContexts)
        {
            contextFlags = 8;
        }
        context->flags = contextFlags;
        fn_8027267C(31)->enabled = useFrameContexts;

        if (useFrameContexts)
        {
            fn_801B3EF4(&lbl_806E16D4);
            fn_801B3F2C(&lbl_806E16D4);
        }

        glEndFrame();
        g_FrameCounter.StartTimer(1);
        glSendFrame();

        if (lbl_806E18D8 != 0 && lbl_806E18D8->flag90 && lbl_806E18D8->flag91)
        {
            glDiscardFrame(fn_8024891C());
        }

        g_FrameCounter.FinishTiming();

        if (lbl_806E0FB0)
        {
            for (int x = 0; x < 640; ++x)
            {
                fn_803A38E4(x, 0, -1);
                fn_803A38E4(x, 479, -1);
            }

            for (int y = 0; y < 480; ++y)
            {
                fn_803A38E4(0, y, -1);
                fn_803A38E4(639, y, -1);
            }
        }
    }
}
