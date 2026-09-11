#include <revolution/gx.h>

#include "Game/Task/EndFrameTask.h"

unsigned char gDrawScreenBorder;

#include "Game/Debug/FrameCounter.h"
#include "Game/HBMManager.h"
#include "Game/Render/RLView.h"
#include "NL/gl/gl.h"
#include "types.h"
#include "Game/Render/Warble.h"
#include "Game/UnidentifiedStaticStorage.h"

void EndFrameTask::Run(float)
{
    if (gpHBMManager == 0 || !gpHBMManager->mActive || !gpHBMManager->mReady)
    {
        bool useWarble = gWarbleEnabled;

        GLView* view = GetLayerView(eCLV_PreWarble);
        unsigned long targetMode = GLViewTarget_None;
        if (useWarble)
        {
            targetMode = GLViewTarget_Mode8;
        }
        view->m_Target = targetMode;

        view = GetLayerView(eCLV_Warble);
        targetMode = GLViewTarget_None;
        if (useWarble)
        {
            targetMode = GLViewTarget_Mode8;
        }
        view->m_Target = targetMode;
        GetLayerView(eCLV_Warble)->m_ClearColour = useWarble;

        if (useWarble)
        {
            UpdateWarbleTexture(&gWarbleEnabled);
            RenderWarbleQuad(&gWarbleEnabled);
        }

        glEndFrame();
        g_FrameCounter.StartTimer(1);
        glSendFrame();

        if (gpHBMManager != 0 && gpHBMManager->mReady && gpHBMManager->mActive)
        {
            HBMManager::Render();
            glIsFrameActive();
        }

        g_FrameCounter.FinishTiming();

        if (gDrawScreenBorder)
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
