#include "Game/Task/BeginFrameTask.h"

#include "Game/Camera/CameraMan.h"
#include "Game/Character.h"
#include "Game/CharacterTemplate.h"
#include "Game/Debug/FrameCounter.h"
#include "Game/Effects/ParticleSystem.h"
#include "Game/GL/GLTexturedColourMeshWriter.h"
#include "Game/HBMManager.h"
#include "Game/NisPlayer.h"
#include "Game/Render/RLView.h"
#include "Game/Render/RLViewLayers.h"
#include "Game/Render/RenderShadow.h"
#include "Game/Render/Warble.h"
#include "Game/Render/depthoffield.h"
#include "Game/TweakValue.h"
#include "Game/UnidentifiedStaticStorage.h"
#include "Game/main.h"
#include "NL/gl/gl.h"
#include "NL/gl/glDraw3.h"
#include "NL/gl/glModel.h"
#include "NL/gl/glPlat.h"
#include "NL/gl/glState.h"
#include "NL/globalpad.h"
#include "NL/glx/glxGX.h"
#include "NL/glx/glxSwap.h"
#include "NL/nlConfig.h"
#include "NL/nlFileGC.h"
#include "NL/nlMath.h"
#include "NL/platvmath.h"
#include "types.h"
#include "unclassified/tu_80284A58.h"
#include "Game/Sys/tweak.h"

extern "C" {
extern bool lbl_806DCCA1;
void GXPokeARGB(u16 x, u16 y, u32 color);
void GXCopyDisp(void* dest, u8 clear);
void GXDrawDone();
void VISetNextFrameBuffer(void* fb);
void VIFlush();
void VIWaitForRetrace();
}

float lbl_806DC7C8 = -1.0f;
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
static bool lbl_806E0FDC;

static TweakValueBool g_bDisplayFrameRate(
    "g_bDisplayFrameRate", "/Rendering/Statistics", false);
static TweakValueBool g_bDisplayFrameTicker(
    "g_bDisplayFrameTicker", gLastTweakCategory, false);

float fn_80112E0C()
{
    return 1.25f;
}

float fn_80112E14()
{
    return 1.666f;
}

glModel* fn_80112E1C(glModel* model)
{
    return NULL;
}

static inline float RadiansToDegrees(float radians)
{
    return radians * 180.0f / 3.1415927f;
}

static inline float GetAspectRatio()
{
    if (lbl_806DC7C8 > 0.1f)
    {
        if (IsWidescreen())
        {
            return 1.666f * lbl_806DC7C8 / 1.25f;
        }
        return lbl_806DC7C8;
    }
    if (IsWidescreen())
    {
        return 1.666f;
    }
    return 1.25f;
}

void SetupMatrices(cBaseCamera* pCamera, const nlMatrix4* pOverride)
{
    nlMatrix4 view;

    if (pOverride != NULL)
    {
        nlMultMatrices(view, pCamera->GetViewMatrix(), *pOverride);
        view = *pOverride;
    }
    else
    {
        view = pCamera->GetViewMatrix();
    }

    float fAspect = GetAspectRatio();

    float fFOV = pCamera->GetFOV();
    fFOV = fFOV >= 0.001f ? fFOV : 0.001f;
    fFOV = fFOV <= 179.999f ? fFOV : 179.999f;
    float fFOVRad = DegreesToRadians(fFOV);

    if (NisPlayer::Instance()->fn_8027E64C()
        && NisPlayer::Instance()->mUnidentified34338 == 4)
    {
        fAspect = 1.666f;
        if (IsWidescreen() && GetPresentation()->mLetterBoxEnabled)
        {
            unsigned short angle = (int)(65536.0f * (0.5f * fFOVRad)) / 360;
            fFOVRad = 2.0f
                * RadiansToDegrees(
                    nlATan(nlTan(angle) * fn_80112E14() / fn_80112E0C()));
        }
    }

    fn_80273144(cCameraManager::m_matView, view, GetAspectRatio(),
        DegreesToRadians(cCameraManager::m_fFOV), fAspect, fFOVRad);

    ParticleSystem::m_LightingCallback = fn_80112E1C;
    if (cCameraManager::PeekCamera() != NULL)
    {
        cCameraManager::m_pBeginFrameCameraType
            = cCameraManager::PeekCamera()->GetType();
    }
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

static void DrawCoPlanarReference()
{
    static u32 BlackTexture = glGetTexture("Global/black");

    float z = GetCoPlanarZ();
    nlVector3 points[4];

    points[0].x = -dimx + offx;
    points[0].y = -dimy + offy;
    points[0].z = z;
    points[1].x = dimx + offx;
    points[1].y = -dimy + offy;
    points[1].z = z;
    points[2].x = dimx + offx;
    points[2].y = dimy + offy;
    points[2].z = z;
    points[3].x = -dimx + offx;
    points[3].y = dimy + offy;
    points[3].z = z;

    glSetDefaultState(false);
    glSetRasterState(GLS_Culling, g_bCoPlanarRefVisible ? 1 : 3);
    glSetRasterState(GLS_DepthTest, g_bCoPlanarDepthTest);
    glSetRasterState(GLS_DepthWrite, g_bCoPlanarDepthWrite);
    glSetCurrentRasterState(glHandleizeRasterState());
    glSetCurrentTexture(BlackTexture, GLTT_Diffuse);

    glQuad3 q;
    q.m_pos[0] = points[0];
    q.m_uv[0].x = 0.0f;
    q.m_uv[0].y = 0.0f;
    q.m_pos[1] = points[1];
    q.m_uv[1].x = 0.0f;
    q.m_uv[1].y = 0.0f;
    q.m_pos[2] = points[2];
    q.m_uv[2].x = 0.0f;
    q.m_uv[2].y = 0.0f;
    q.m_pos[3] = points[3];
    q.m_uv[3].x = 0.0f;
    q.m_uv[3].y = 0.0f;

    q.SetColour(0xFF, 0xFF, 0xFF, 0xFF);
    q.Attach((eGLView)GetLayerView(eCLV_CoPlanar), 0);
}

static inline void SetTextureBinding(glTextureBinding* binding, u32 texture)
{
    binding->texture = texture;
    binding->textureIndex = 0xFFFF;
    binding->SetWrapS(true);
    binding->SetWrapT(true);
    binding->unknown07 = 0;
}

static inline void DrawLine(GLView* view, int stream, const nlColour& colour,
    int x0, int y0, int x1, int y1)
{
    GLTexturedColourMeshWriter mesh;
    glSetDefaultState(false);
    if (mesh.Begin(2, 4, 0))
    {
        SetTextureBinding(
            (glTextureBinding*)mesh.GetModel()->packets->materialParameters,
            glGetTexture("global/white"));

        mesh.Texcoord((short)0, (short)0);
        mesh.Colour(colour);
        mesh.Vertex((float)x0, (float)y0, 0.0f);
        mesh.Texcoord((short)0, (short)0);
        mesh.Colour(colour);
        mesh.Vertex((float)x1, (float)y1, 0.0f);
        if (mesh.End())
        {
            view->AttachModel(mesh.GetModel(), stream);
        }
    }
}

static void DrawSafeFrame()
{
    GLView* view;
    nlColour colour = { 0x40, 0xFF, 0x40, 0xFF };
    int offset = (glx_GetScaledXFBWidth() - 640) / 2;
    int left = offset + 16;
    int right = 624 - offset;
    int top = 30;
    int bottom = 450;

    {
        view = GetLayerView(eCLV_Debug);
        GLTexturedColourMeshWriter mesh;
        glSetDefaultState(false);
        if (mesh.Begin(2, 4, 0))
        {
            SetTextureBinding(
                (glTextureBinding*)mesh.GetModel()->packets->materialParameters,
                glGetTexture("global/white"));

            mesh.Texcoord((short)0, (short)0);
            mesh.Colour(colour);
            mesh.Vertex((float)left, (float)top, 0.0f);
            mesh.Texcoord((short)0, (short)0);
            mesh.Colour(colour);
            mesh.Vertex((float)right, (float)top, 0.0f);
            if (mesh.End())
            {
                view->AttachModel(mesh.GetModel(), 100);
            }
        }
    }

    {
        view = GetLayerView(eCLV_Debug);
        GLTexturedColourMeshWriter mesh;
        glSetDefaultState(false);
        if (mesh.Begin(2, 4, 0))
        {
            SetTextureBinding(
                (glTextureBinding*)mesh.GetModel()->packets->materialParameters,
                glGetTexture("global/white"));

            mesh.Texcoord((short)0, (short)0);
            mesh.Colour(colour);
            mesh.Vertex((float)left, (float)bottom, 0.0f);
            mesh.Texcoord((short)0, (short)0);
            mesh.Colour(colour);
            mesh.Vertex((float)right, (float)bottom, 0.0f);
            if (mesh.End())
            {
                view->AttachModel(mesh.GetModel(), 100);
            }
        }
    }

    {
        view = GetLayerView(eCLV_Debug);
        GLTexturedColourMeshWriter mesh;
        glSetDefaultState(false);
        if (mesh.Begin(2, 4, 0))
        {
            SetTextureBinding(
                (glTextureBinding*)mesh.GetModel()->packets->materialParameters,
                glGetTexture("global/white"));

            mesh.Texcoord((short)0, (short)0);
            mesh.Colour(colour);
            mesh.Vertex((float)left, (float)top, 0.0f);
            mesh.Texcoord((short)0, (short)0);
            mesh.Colour(colour);
            mesh.Vertex((float)left, (float)bottom, 0.0f);
            if (mesh.End())
            {
                view->AttachModel(mesh.GetModel(), 100);
            }
        }
    }

    {
        view = GetLayerView(eCLV_Debug);
        GLTexturedColourMeshWriter mesh;
        glSetDefaultState(false);
        if (mesh.Begin(2, 4, 0))
        {
            SetTextureBinding(
                (glTextureBinding*)mesh.GetModel()->packets->materialParameters,
                glGetTexture("global/white"));

            mesh.Texcoord((short)0, (short)0);
            mesh.Colour(colour);
            mesh.Vertex((float)right, (float)top, 0.0f);
            mesh.Texcoord((short)0, (short)0);
            mesh.Colour(colour);
            mesh.Vertex((float)right, (float)bottom, 0.0f);
            if (mesh.End())
            {
                view->AttachModel(mesh.GetModel(), 100);
            }
        }
    }
}

static void DrawGrid(int spacing)
{
    nlColour gridColour = { 0x40, 0x40, 0xFF, 0xFF };
    nlColour centreColour = { 0xFF, 0x40, 0x40, 0xFF };

    for (int y = 0; y < 480; y += spacing)
    {
        DrawLine(GetLayerView(eCLV_Debug), 99, gridColour, 0, y, 640, y);
    }
    for (int x = 0; x < 640; x += spacing)
    {
        DrawLine(GetLayerView(eCLV_Debug), 99, gridColour, x, 0, x, 480);
    }
    DrawLine(GetLayerView(eCLV_Debug), 99, centreColour, 320, 0, 320, 480);
}

void BeginFrameTask::Run(float dt)
{
    if (gpHBMManager != NULL && gpHBMManager->mActive && gpHBMManager->mReady)
    {
        static int counter;

        HBMManager::Render();
        ++counter;
        int y = counter / 640;
        int x = counter - 640 * y;
        GXPokeARGB(x, y, 0xFF0000FF);
        gxSetZMode(true, 3, true);
        gxSetColourUpdate(true);
        gxSetAlphaUpdate(true);
        GXCopyDisp(glxGetDisplayedBuffer(), 1);
        GXDrawDone();
        VISetNextFrameBuffer(glxGetDisplayedBuffer());
        VIFlush();
        VIWaitForRetrace();
        glxSwapBuffers();
        return;
    }

    gWarbleEnabled = false;
    if (lbl_806E0FDC)
    {
        cGlobalPad* pad = g_pPadManager->GetPad(0);
        if (pad != NULL && pad->PlatJustPressed(0x12, true))
        {
            glxRequestScreenShot();
        }
    }

    fn_80273A30(eCLV_ShadowVolume);
    fn_80273A30(eCLV_ShadowVolumeBlend);
    nlServiceFileSystem();

    g_FrameCounter.StartTimer(0);
    if (g_bFrameSmiler)
    {
        g_FrameCounter.DisplayFrameSmiler();
    }
    if (g_bFrameStatsOnScreen)
    {
        g_FrameCounter.fn_802B7FD4();
    }
    if (g_bDisplayFrameRate.GetValue())
    {
        g_FrameCounter.DisplayFrameRate();
    }
    if (g_bDisplayFrameTicker.GetValue())
    {
        g_FrameCounter.DisplayFrameTicker();
    }

    if (nlTaskManager::m_pInstance->mCurrentState & 0x2001B)
    {
        GetPresentation()->Update(dt);
        NisPlayer::Instance()->Update(dt);
    }

    glBeginFrame();

    for (int i = 0; i < 10; i++)
    {
        if (g_pCharacters[i] != NULL)
        {
            g_pCharacters[i]->fn_80022D58(dt);
        }
    }

    cCameraManager::Update(dt);

    if (!(nlTaskManager::m_pInstance->mCurrentState & 0x18))
    {
        DepthOfFieldManager::instance.TurnOff();
    }

    NisPlayer* pNis = NisPlayer::Instance();
    const nlMatrix4* pOverride
        = pNis->mUnidentified3435A ? &pNis->mUnidentified3435C : NULL;

    SetupMatrices(NisPlayer::Instance()->fn_8027E708(), pOverride);
    SetupRenderInfo();

    if (!lbl_806DCCA1)
    {
        DrawCoPlanarReference();
    }

    switch (nlTaskManager::m_pInstance->mCurrentState)
    {
    case 8:
        ParticleSystem::m_AllowInFront = false;
        break;
    default:
        ParticleSystem::m_AllowInFront = true;
        break;
    }

    if (g_bDrawSafeFrame)
    {
        DrawSafeFrame();
    }

    if (g_nGridDisplaySpacing > 0)
    {
        DrawGrid(g_nGridDisplaySpacing);
    }

    static bool showRegion = Config::Global().Get<bool>("show_region", false);
    if (showRegion)
    {
        nlScreenPrintf(0, 0, false, 4, "Region %d", GetRegion());
    }
}
