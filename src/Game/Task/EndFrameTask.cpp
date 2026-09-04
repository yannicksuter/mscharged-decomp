#include "Game/Task/EndFrameTask.h"

#include "Game/Render/RLView.h"

#include "Game/Debug/FrameCounter.h"
#include "Game/HBMManager_8024795C.h"
#include "NL/gl/gl.h"
#include "types.h"

struct RenderContext
{
    u8 padding[0x33];
    u8 enabled;
    u32 flags;
};

extern u8 lbl_806E16D4;
extern u8 lbl_806E0FB0;

extern "C" void fn_801B3EF4(u8* enabled);
extern "C" void fn_801B3F2C(u8* enabled);
extern "C" void fn_802C80FC();
extern "C" void GXPokeARGB(u16 x, u16 y, u32 colour);

void EndFrameTask::Run(float)
{
    if (gpHBMManager == 0 || !gpHBMManager->mActive || !gpHBMManager->mReady)
    {
        u8 useFrameContexts = lbl_806E16D4;

        RenderContext* context = (RenderContext*)GetLayerView(eCLV_PreWarble);
        u32 contextFlags = 0;
        if (useFrameContexts)
        {
            contextFlags = 8;
        }
        context->flags = contextFlags;

        context = (RenderContext*)GetLayerView(eCLV_Warble);
        contextFlags = 0;
        if (useFrameContexts)
        {
            contextFlags = 8;
        }
        context->flags = contextFlags;
        ((RenderContext*)GetLayerView(eCLV_Warble))->enabled = useFrameContexts;

        if (useFrameContexts)
        {
            fn_801B3EF4(&lbl_806E16D4);
            fn_801B3F2C(&lbl_806E16D4);
        }

        glEndFrame();
        g_FrameCounter.StartTimer(1);
        glSendFrame();

        if (gpHBMManager != 0 && gpHBMManager->mReady && gpHBMManager->mActive)
        {
            UnidentifiedHBMManager::fn_8024891C();
            fn_802C80FC();
        }

        g_FrameCounter.FinishTiming();

        if (lbl_806E0FB0)
        {
            for (int x = 0; x < 640; ++x)
            {
                GXPokeARGB(x, 0, -1);
                GXPokeARGB(x, 479, -1);
            }

            for (int y = 0; y < 480; ++y)
            {
                GXPokeARGB(0, y, -1);
                GXPokeARGB(639, y, -1);
            }
        }
    }
}
