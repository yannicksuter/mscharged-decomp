#include "Game/BeginFrameTask.h"

#include "Game/TweakValue.h"
#include "NL/glx/glxSwap.h"
#include "types.h"

static float lbl_806DC7C8 = -1.0f;
static float dimx = 48.0f;
static float dimy = 28.0f;

eModelSkinMethod BeginFrameTask::s_GameplaySkin;
eModelSkinMethod BeginFrameTask::s_ReplaySkin;
bool BeginFrameTask::s_FramerateLocked;

static float offx;
static float offy;

bool g_bCoPlanarRefVisible;
bool g_bCoPlanarDepthTest;
bool g_bCoPlanarDepthWrite;
bool g_bFrameSmiler;
bool g_bFrameStatsOnScreen;

static bool g_bDrawSafeFrame;
static s32 g_nGridDisplaySpacing;
static s32 g_eWaitMode;

static TweakValueBool_804F4578 g_bDisplayFrameRate(
    "g_bDisplayFrameRate", "/Rendering/Statistics", false);
static TweakValueBool_804F4578 g_bDisplayFrameTicker(
    "g_bDisplayFrameTicker", lbl_806E1E90, false);

extern const f32 lbl_806E45F8;
extern const f32 lbl_806E45FC;

extern "C" float fn_80112E0C()
{
    return lbl_806E45F8;
}

extern "C" float fn_80112E14()
{
    return lbl_806E45FC;
}

extern "C" bool fn_80112E1C()
{
    return false;
}

static void SetupRenderInfo()
{
    s32 swapMode;

    switch (nlTaskManager::m_pInstance->mCurrentState)
    {
    case 16:
        if (BeginFrameTask::s_FramerateLocked)
        {
            swapMode = 2;
        }
        else
        {
            swapMode = 1;
        }
        break;
    case 4:
    case 8:
        swapMode = 1;
        break;
    default:
        switch (g_eWaitMode)
        {
        case 1:
            swapMode = 0;
            break;
        case 2:
            swapMode = 1;
            break;
        case 3:
            swapMode = 3;
            break;
        case 4:
            swapMode = 2;
            break;
        case 0:
        default:
            swapMode = 3;
            break;
        }
        break;
    }

    glxSetSwapMode(swapMode);
}

static void DrawCoPlanarReference();
static void DrawSafeFrame();
