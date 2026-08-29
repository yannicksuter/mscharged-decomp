#include <revolution/gx.h>

#include "Game/Render/RLViewLayers.h"
#include "NL/gl/glMatrix.h"
#include "NL/gl/glModel.h"
#include "NL/gl/glState.h"
#include "NL/glx/glxGX.h"
#include "NL/nlMemory.h"
#include "NL/nlPrint.h"
#include "NL/nlString.h"

extern "C" void fn_8036EB44(float nearPlane, float farPlane);
extern "C" u32 fn_80369D4C();
extern "C" u32 fn_80369D54();
extern "C" void fn_8037091C();
extern "C" void fn_80370998(GLView*, GLView*);
extern "C" void fn_801A2394(void*);
extern "C" void fn_802DCDB4(nlMatrix4&, const nlMatrix4&, const nlMatrix4&);
extern "C" u8 lbl_80572020[];

void CopyShadowVolumeColour(const GXColor* colour);

struct RLViewTargetInfo
{
    /* 0x00 */ u32 height;
    /* 0x04 */ u32 width;
    /* 0x08 */ u32 unknown08;
    /* 0x0C */ u32 unknown0C;
    /* 0x10 */ u32 unknown10;
    /* 0x14 */ u32 format;
    /* 0x18 */ u32 unknown18;
    /* 0x1C */ u32 unknown1C;
    /* 0x20 */ u32 unknown20;
    /* 0x24 */ u8 colour[4];
};

struct RLViewLayerDesc
{
    /* 0x00 */ eCLV layer;
    /* 0x04 */ const char* name;
    /* 0x08 */ bool useDisplayTarget;
    /* 0x0C */ GLViewSortMode sortMode;
};

const nlMatrix4 sIdentityMatrix = {
    1.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 1.0f,
};

static const GXColor sShadowVolumeColour = { 0, 0, 0, 0 };

static char sDofTargetName[] = "dof";
static char sPipTargetName[] = "pip";

void fn_80271DE0()
{
    sLayerViews[eCLV_ImpostorTexture]->m_Visible = eCLV_ImpostorTextureEnabled.GetValue();
    sLayerViews[eCLV_ShadowTexture]->m_Visible = eCLV_ShadowTextureEnabled.GetValue();
    sLayerViews[eCLV_GrabTexture]->m_Visible = eCLV_GrabTextureEnabled.GetValue();
    sLayerViews[eCLV_PictureInPicture]->m_Visible = eCLV_PictureInPictureEnabled.GetValue();
    sLayerViews[eCLV_PictureInPictureAlpha]->m_Visible = eCLV_PictureInPictureAlphaEnabled.GetValue();
    sLayerViews[eCLV_NoFog]->m_Visible = eCLV_NoFogEnabled.GetValue();
    sLayerViews[eCLV_Shadowed]->m_Visible = eCLV_ShadowedEnabled.GetValue();
    sLayerViews[eCLV_WorldShadowed]->m_Visible = eCLV_WorldShadowedEnabled.GetValue();
    sLayerViews[eCLV_Unshadowed]->m_Visible = eCLV_UnshadowedEnabled.GetValue();
    sLayerViews[eCLV_MegastrikeBackground]->m_Visible = eCLV_MegastrikeBackgroundEnabled.GetValue();
    sLayerViews[eCLV_ImpostorOut]->m_Visible = eCLV_ImpostorOutEnabled.GetValue();
    sLayerViews[eCLV_Characters]->m_Visible = eCLV_CharactersEnabled.GetValue();
    sLayerViews[eCLV_PeachPhoto3D]->m_Visible = eCLV_PeachPhoto3DEnabled.GetValue();
    sLayerViews[eCLV_MoreCharacters]->m_Visible = eCLV_MoreCharactersEnabled.GetValue();
    sLayerViews[eCLV_WorldAlphaBlended]->m_Visible = eCLV_WorldAlphaBlendedEnabled.GetValue();
    sLayerViews[eCLV_HighRange3D]->m_Visible = eCLV_HighRange3DEnabled.GetValue();
    sLayerViews[eCLV_HighRange3DNoFog]->m_Visible = eCLV_HighRange3DNoFogEnabled.GetValue();
    sLayerViews[eCLV_HighRangeChain]->m_Visible = eCLV_HighRangeChainEnabled.GetValue();
    sLayerViews[eCLV_HighRange2D]->m_Visible = eCLV_HighRange2DEnabled.GetValue();
    sLayerViews[eCLV_BigBlackPolygon]->m_Visible = eCLV_BigBlackPolygonEnabled.GetValue();
    sLayerViews[eCLV_ShadowVolume]->m_Visible = eCLV_ShadowVolumeEnabled.GetValue();
    sLayerViews[eCLV_ShadowVolumeBlend]->m_Visible = eCLV_ShadowVolumeBlendEnabled.GetValue();
    sLayerViews[eCLV_UnsortedPerspective]->m_Visible = eCLV_UnsortedPerspectiveEnabled.GetValue();
    sLayerViews[eCLV_DepthOfField]->m_Visible = eCLV_DepthOfFieldEnabled.GetValue();
    sLayerViews[eCLV_LingeringParticles]->m_Visible = eCLV_LingeringParticlesEnabled.GetValue();
    sLayerViews[eCLV_Particles]->m_Visible = eCLV_ParticlesEnabled.GetValue();
    sLayerViews[eCLV_BallChargeAlphaBlended]->m_Visible = eCLV_BallChargeAlphaBlendedEnabled.GetValue();
    sLayerViews[eCLV_CoPlanar]->m_Visible = eCLV_CoPlanarEnabled.GetValue();
    sLayerViews[eCLV_InvisiblePlane]->m_Visible = eCLV_InvisiblePlaneEnabled.GetValue();
    sLayerViews[eCLV_ElectricFence]->m_Visible = eCLV_ElectricFenceEnabled.GetValue();
    sLayerViews[eCLV_PreWarble]->m_Visible = eCLV_PreWarbleEnabled.GetValue();
    sLayerViews[eCLV_Warble]->m_Visible = eCLV_WarbleEnabled.GetValue();
    sLayerViews[eCLV_WarbleBlend]->m_Visible = eCLV_WarbleBlendEnabled.GetValue();
    sLayerViews[eCLV_CameraSpace]->m_Visible = eCLV_CameraSpaceEnabled.GetValue();
    sLayerViews[eCLV_ScreenBlur]->m_Visible = eCLV_ScreenBlurEnabled.GetValue();
    sLayerViews[eCLV_ScreenBlur2]->m_Visible = eCLV_ScreenBlur2Enabled.GetValue();
    sLayerViews[eCLV_ScreenGrab]->m_Visible = eCLV_ScreenGrabEnabled.GetValue();
    sLayerViews[eCLV_FrontEnd]->m_Visible = eCLV_FrontEndEnabled.GetValue();
    sLayerViews[eCLV_UnsortedOrtho640]->m_Visible = eCLV_UnsortedOrtho640Enabled.GetValue();
    sLayerViews[eCLV_UnsortedSquareOrtho]->m_Visible = eCLV_UnsortedSquareOrthoEnabled.GetValue();
    sLayerViews[eCLV_Transitions3D]->m_Visible = eCLV_Transitions3DEnabled.GetValue();
    sLayerViews[eCLV_Transitions]->m_Visible = eCLV_TransitionsEnabled.GetValue();
    sLayerViews[eCLV_Anark3D_BG]->m_Visible = eCLV_Anark3D_BGEnabled.GetValue();
    sLayerViews[eCLV_Anark]->m_Visible = eCLV_AnarkEnabled.GetValue();
    sLayerViews[eCLV_Anark3D_FG]->m_Visible = eCLV_Anark3D_FGEnabled.GetValue();
    sLayerViews[eCLV_HomeButtonFadeOut]->m_Visible = eCLV_HomeButtonFadeOutEnabled.GetValue();
    sLayerViews[eCLV_Debug]->m_Visible = eCLV_DebugEnabled.GetValue();
    sLayerViews[eCLV_DebugSquare]->m_Visible = eCLV_DebugSquareEnabled.GetValue();
}

int fn_802721BC()
{
    return 11;
}

void fn_802721C4(int view, bool enabled)
{
    GLView* v = sViews[view];
    unsigned long target = 0;
    if (enabled)
    {
        target = 8;
    }
    v->m_Target = target;
}

GLView* fn_802721EC(int view)
{
    return sViews[view];
}

u32 fn_80272200(int view)
{
    return sShadowDebugTargets[view];
}

void fn_80272214(int view, const nlMatrix4& viewMatrix, const nlMatrix4& projectionMatrix)
{
    sShadowDebugCameras[view].Set(viewMatrix, projectionMatrix);
}

void fn_80272388()
{
    RLViewTargetInfo info;
    int i;

    info.unknown08 = 0;
    info.unknown0C = 0;

    for (i = 0; i < 11; i++)
    {
        nlZeroMemory(&sShadowDebugRects[i], sizeof(RLViewRect));
        sShadowDebugRects[i].width = 0x90;
        sShadowDebugRects[i].height = 0x90;
        sShadowDebugRects[i].x = (i % 4) * 0x90;
        sShadowDebugRects[i].y = (i / 4) * 0x90;
    }

    for (i = 0; i < 11; i++)
    {
        nlSNPrintf(sShadowDebugNames[i], sizeof(sShadowDebugNames[i]), "shadow_%02d", i);
    }

    for (i = 0; i < 11; i++)
    {
        nlZeroMemory(&info, sizeof(RLViewTargetInfo));
        info.width = 0x48;
        info.height = 0x48;
        info.format = 6;
        info.unknown18 = 0;
        info.unknown1C = 0;
        GLRenderPair pair = fn_802CD884(sShadowDebugNames[i], &info);
        sShadowDebugPairs[i] = pair;
        sShadowDebugTargets[i] = pair.hash;
    }

    for (i = 0; i < 11; i++)
    {
        RLView* view = new (8, false) RLView(&sShadowDebugCameras[i], sShadowDebugPairs[i], GLViewSort_None);
        view->m_Name = sShadowDebugNames[i];
        view->m_ViewportX = sShadowDebugRects[i].x;
        view->m_ViewportY = sShadowDebugRects[i].y;
        view->m_ViewportWidth = sShadowDebugRects[i].width;
        view->m_ViewportHeight = sShadowDebugRects[i].height;
        view->m_Unknown33 = false;
        view->m_ClearColour = false;
        view->m_Target = 0;
        sViews[i] = view;
    }

    for (i = 0; i < 11; i++)
    {
        sViews[i]->m_Enabled = true;
        sLayerViews[eCLV_ShadowTexture]->m_Children.AddEnd(sViews[i]);
        sViews[i]->m_Parent = sLayerViews[eCLV_ShadowTexture];
    }

    sLayerViews[eCLV_ShadowTexture]->m_Unknown33 = true;
    sLayerViews[eCLV_ShadowTexture]->m_ClearColour = true;
}

static RLViewLayerDesc sPerspectiveLayers[] = {
    { eCLV_Shadowed, "eCLV_Shadowed", true, GLViewSort_Texture },
    { eCLV_NoFog, "eCLV_NoFog", true, GLViewSort_Texture },
    { eCLV_ShadowVolume, "eCLV_ShadowVolume", true, GLViewSort_None },
    { eCLV_WorldShadowed, "eCLV_WorldShadowed", true, GLViewSort_Texture },
    { eCLV_Characters, "eCLV_Characters", true, GLViewSort_Texture },
    { eCLV_PeachPhoto3D, "eCLV_PeachPhoto3D", true, GLViewSort_None },
    { eCLV_MoreCharacters, "eCLV_MoreCharacters", true, GLViewSort_Texture },
    { eCLV_ImpostorOut, "eCLV_ImpostorOut", true, GLViewSort_Texture },
    { eCLV_Unshadowed, "eCLV_Unshadowed", true, GLViewSort_Texture },
    { eCLV_WorldAlphaBlended, "eCLV_WorldAlphaBlended", true, GLViewSort_Texture },
    { eCLV_HighRange3DNoFog, "eCLV_HighRange3DNoFog", true, GLViewSort_Texture },
    { eCLV_HighRange3D, "eCLV_HighRange3D", true, GLViewSort_Texture },
    { eCLV_Warble, "eCLV_Warble", true, GLViewSort_Texture },
    { eCLV_LingeringParticles, "eCLV_LingeringParticles", true, GLViewSort_Texture },
    { eCLV_Particles, "eCLV_Particles", true, GLViewSort_Texture },
    { eCLV_BallChargeAlphaBlended, "eCLV_BallChargeAlphaBlended", true, GLViewSort_Texture },
    { eCLV_UnsortedPerspective, "eCLV_UnsortedPerspective", true, GLViewSort_None },
    { eCLV_InvisiblePlane, "eCLV_InvisiblePlane", true, GLViewSort_Texture },
    { eCLV_ElectricFence, "eCLV_ElectricFence", true, GLViewSort_None },
    { eCLV_CoPlanar, "eCLV_CoPlanar", true, GLViewSort_None },
    { eCLV_PreWarble, "eCLV_PreWarble", true, GLViewSort_None },
    { eCLV_ImpostorTexture, "eCLV_ImpostorTexture", true, GLViewSort_Texture },
    { eCLV_ShadowTexture, "eCLV_ShadowTexture", true, GLViewSort_Texture },
    { eCLV_GrabTexture, "eCLV_GrabTexture", true, GLViewSort_Texture },
    { eCLV_ScreenBlur, "eCLV_ScreenBlur", true, GLViewSort_Texture },
    { eCLV_ScreenBlur2, "eCLV_ScreenBlur2", true, GLViewSort_Texture },
    { eCLV_ScreenGrab, "eCLV_ScreenGrab", true, GLViewSort_Texture },
};

static RLViewLayerDesc sPipLayers[] = {
    { eCLV_PictureInPicture, "eCLV_PictureInPicture", true, GLViewSort_Texture },
    { eCLV_PictureInPictureAlpha, "eCLV_PictureInPictureAlpha", true, GLViewSort_Texture },
};

static RLViewLayerDesc sOrthoLayers[] = {
    { eCLV_ShadowVolumeBlend, "eCLV_ShadowVolumeBlend", true, GLViewSort_None },
    { eCLV_FrontEnd, "eCLV_FrontEnd", true, GLViewSort_TransformedMatrixDepth },
    { eCLV_Transitions, "eCLV_Transitions", true, GLViewSort_None },
    { eCLV_WarbleBlend, "eCLV_WarbleBlend", true, GLViewSort_None },
    { eCLV_DepthOfField, "eCLV_DepthOfField", true, GLViewSort_Texture },
    { eCLV_MegastrikeBackground, "eCLV_MegastrikeBackground", true, GLViewSort_TransformedMatrixDepth },
    { eCLV_BigBlackPolygon, "eCLV_BigBlackPolygon", true, GLViewSort_TransformedMatrixDepth },
    { eCLV_HighRangeChain, "eCLV_HighRangeChain", true, GLViewSort_None },
    { eCLV_HighRange2D, "eCLV_HighRange2D", true, GLViewSort_None },
    { eCLV_UnsortedOrtho640, "eCLV_UnsortedOrtho640", true, GLViewSort_None },
    { eCLV_HomeButtonFadeOut, "eCLV_HomeButtonFadeOut", true, GLViewSort_None },
    { eCLV_Debug, "eCLV_Debug", true, GLViewSort_None },
};

static RLViewLayerDesc sSquareOrthoLayers[] = {
    { eCLV_UnsortedSquareOrtho, "eCLV_UnsortedSquareOrtho", true, GLViewSort_None },
    { eCLV_DebugSquare, "eCLV_DebugSquare", true, GLViewSort_None },
};

static RLViewLayerDesc sAnarkLayers[] = {
    { eCLV_Anark, "eCLV_Anark", true, GLViewSort_Reverse },
};

static RLViewLayerDesc sCameraSpaceLayers[] = {
    { eCLV_CameraSpace, "eCLV_CameraSpace", true, GLViewSort_Texture },
};

static RLViewLayerDesc sAnark3DLayers[] = {
    { eCLV_Anark3D_BG, "eCLV_Anark3D_BG", true, GLViewSort_Texture },
    { eCLV_Anark3D_FG, "eCLV_Anark3D_FG", true, GLViewSort_Texture },
};

static RLViewLayerDesc sTransitions3DLayers[] = {
    { eCLV_Transitions3D, "eCLV_Transitions3D", true, GLViewSort_Texture },
};

extern "C" RLView* fn_8027261C()
{
    return sLayerViews[eCLV_Shadowed];
}

extern "C" RLView* fn_8027262C()
{
    return sLayerViews[eCLV_Unshadowed];
}

glModelStream* fn_8027263C(const glModelPacket* packet, int id)
{
    for (unsigned int i = 0; i < packet->numStreams; i++)
    {
        glModelStream* stream = packet->streams + i;
        if (id == stream->id)
        {
            return stream;
        }
    }
    return 0;
}

extern "C" RLView* fn_8027267C(eCLV layer)
{
    if (layer < eCLV_Num)
    {
        return sLayerViews[layer];
    }
    return 0;
}

GLViewInterface* fn_802726A0()
{
    return &sOrthoCamera;
}

void fn_802726AC()
{
    RLViewTargetInfo info;
    info.unknown08 = 0;
    info.unknown0C = 0;
    info.width = 0x40;
    info.height = 0x40;
    info.format = 7;
    info.unknown18 = 0;
    info.unknown1C = 0;
    info.colour[0] = 0x7C;
    info.colour[1] = 0x7C;
    info.colour[2] = 0x7C;
    info.colour[3] = 0x7C;
    GLRenderPair texturePair = fn_802CD884("warbletexture", &info);
    sWarbleTextureTarget = texturePair;

    info.width = fn_80369D4C();
    info.height = fn_80369D54();
    info.format = 1;
    info.unknown18 = 0;
    info.unknown1C = 0;
    info.colour[0] = 0;
    info.colour[1] = 0;
    info.colour[2] = 0;
    info.colour[3] = 0;
    GLRenderPair colourPair = fn_802CD884("warblecolour", &info);
    sWarbleColourTarget = colourPair;
    sLayerViews[eCLV_PreWarble]->m_RenderPair = colourPair;
    sLayerViews[eCLV_PreWarble]->m_Target = 8;

    sLayerViews[eCLV_Warble]->m_ViewportX = 0;
    sLayerViews[eCLV_Warble]->m_ViewportY = 0;
    sLayerViews[eCLV_Warble]->m_ViewportWidth = fn_80369D4C() >> 1;
    sLayerViews[eCLV_Warble]->m_ViewportHeight = fn_80369D54() >> 1;

    info.width = fn_80369D4C() >> 2;
    info.height = fn_80369D54() >> 2;
    info.format = 5;
    info.unknown18 = 1;
    info.colour[0] = 0x80;
    info.colour[1] = 0x80;
    info.colour[2] = 0x80;
    info.colour[3] = 0x80;
    GLRenderPair offsetPair = fn_802CD884("warbleoffset", &info);
    sWarbleOffsetTarget = offsetPair;
    sLayerViews[eCLV_Warble]->m_RenderPair = offsetPair;
    sLayerViews[eCLV_Warble]->m_Unknown33 = true;
    sLayerViews[eCLV_Warble]->m_Target = 8;
}

void fn_80272850()
{
    RLViewTargetInfo info;
    info.unknown08 = 0;
    info.unknown0C = 0;
    nlZeroMemory(&info, sizeof(RLViewTargetInfo));
    info.width = fn_80369D4C() >> 1;
    info.height = fn_80369D54() >> 1;
    info.format = 1;
    info.unknown18 = 1;
    info.unknown1C = 0;
    info.colour[0] = 0xFF;
    info.colour[1] = 0xFF;
    info.colour[2] = 0;
    info.colour[3] = 0;
    GLRenderPair dofPair = fn_802CD884(sDofTargetName, &info);
    sDofTarget = dofPair;
    sLayerViews[eCLV_UnsortedPerspective]->m_RenderPair = dofPair;
    sLayerViews[eCLV_UnsortedPerspective]->m_Unknown33 = false;
    sLayerViews[eCLV_UnsortedPerspective]->m_ClearColour = false;

    info.unknown18 = 0;
    info.colour[0] = 0;
    info.colour[1] = 0;
    info.colour[2] = 0;
    info.colour[3] = 0;
    GLRenderPair grabPair = fn_802CD884("screengrab", &info);
    sScreenGrabTarget = grabPair;
    sLayerViews[eCLV_ScreenGrab]->m_RenderPair = grabPair;
    sLayerViews[eCLV_ScreenGrab]->m_Unknown33 = false;
    sLayerViews[eCLV_ScreenGrab]->m_ClearColour = false;

    nlZeroMemory(&info, sizeof(RLViewTargetInfo));
    info.width = 0x100;
    info.height = 0x80;
    info.format = 1;
    info.unknown18 = 1;
    info.unknown1C = 0;
    info.colour[0] = 0;
    info.colour[1] = 0;
    info.colour[2] = 0;
    info.colour[3] = 0xFF;
    sLayerViews[eCLV_PictureInPicture]->m_Target = 0;
    sLayerViews[eCLV_PictureInPicture]->m_ClearColour = true;
    sLayerViews[eCLV_PictureInPicture]->m_Unknown33 = true;
    sLayerViews[eCLV_PictureInPicture]->m_ViewportX = 0;
    sLayerViews[eCLV_PictureInPicture]->m_ViewportY = 0;
    sLayerViews[eCLV_PictureInPicture]->m_ViewportWidth = 0x200;
    sLayerViews[eCLV_PictureInPicture]->m_ViewportHeight = 0x100;
    GLRenderPair pipPair = fn_802CD884(sPipTargetName, &info);
    sLayerViews[eCLV_PictureInPictureAlpha]->SetRenderPair(pipPair);
    sLayerViews[eCLV_PictureInPictureAlpha]->m_Target = 9;
    sLayerViews[eCLV_PictureInPictureAlpha]->m_ClearColour = false;
    sLayerViews[eCLV_PictureInPictureAlpha]->m_Unknown33 = false;
    sLayerViews[eCLV_PictureInPictureAlpha]->m_ViewportX = 0;
    sLayerViews[eCLV_PictureInPictureAlpha]->m_ViewportY = 0;
    sLayerViews[eCLV_PictureInPictureAlpha]->m_ViewportWidth = 0x200;
    sLayerViews[eCLV_PictureInPictureAlpha]->m_ViewportHeight = 0x100;

    info.width = fn_80369D4C() >> 1;
    info.height = fn_80369D54() >> 1;
    info.format = 7;
    info.unknown18 = 0;
    GLRenderPair greyPair = fn_802CD884("grayscale", &info);
    sLayerViews[eCLV_Characters]->SetRenderPair(greyPair);
    sLayerViews[eCLV_Characters]->m_ClearColour = false;
    sLayerViews[eCLV_Characters]->m_Unknown33 = false;

    fn_802726AC();
}

void fn_80272AB4()
{
    int i;

    {
        GLRenderPair display = fn_802CD82C();
        GLRenderPair pairs[2] = { display, GLRenderPair() };
        for (i = 0; i < 27; i++)
        {
            RLView* view = new (8, false) RLView(&sPerspectiveCamera, sPerspectiveLayers[i].useDisplayTarget ? pairs[0] : pairs[1], sPerspectiveLayers[i].sortMode);
            sLayerViews[sPerspectiveLayers[i].layer] = view;
            sLayerViews[sPerspectiveLayers[i].layer]->m_Name = sPerspectiveLayers[i].name;
            sLayerViews[sPerspectiveLayers[i].layer]->m_Unknown48 = sPerspectiveLayers[i].layer;
        }
    }

    {
        GLRenderPair display = fn_802CD82C();
        GLRenderPair pairs[2] = { display, GLRenderPair() };
        for (i = 0; i < 2; i++)
        {
            RLView* view = new (8, false) RLView(&sPipCamera, sPipLayers[i].useDisplayTarget ? pairs[0] : pairs[1], sPipLayers[i].sortMode);
            sLayerViews[sPipLayers[i].layer] = view;
            sLayerViews[sPipLayers[i].layer]->m_Name = sPipLayers[i].name;
            sLayerViews[sPipLayers[i].layer]->m_Unknown48 = sPipLayers[i].layer;
        }
    }

    {
        GLRenderPair display = fn_802CD82C();
        GLRenderPair pairs[2] = { display, GLRenderPair() };
        for (i = 0; i < 12; i++)
        {
            RLView* view = new (8, false) RLView(&sOrthoCamera, sOrthoLayers[i].useDisplayTarget ? pairs[0] : pairs[1], sOrthoLayers[i].sortMode);
            sLayerViews[sOrthoLayers[i].layer] = view;
            sLayerViews[sOrthoLayers[i].layer]->m_Name = sOrthoLayers[i].name;
            sLayerViews[sOrthoLayers[i].layer]->m_Unknown48 = sOrthoLayers[i].layer;
        }
    }

    {
        GLRenderPair display = fn_802CD82C();
        GLRenderPair pairs[2] = { display, GLRenderPair() };
        for (i = 0; i < 2; i++)
        {
            RLView* view = new (8, false) RLView(&sOrtho640Camera, sSquareOrthoLayers[i].useDisplayTarget ? pairs[0] : pairs[1], sSquareOrthoLayers[i].sortMode);
            sLayerViews[sSquareOrthoLayers[i].layer] = view;
            sLayerViews[sSquareOrthoLayers[i].layer]->m_Name = sSquareOrthoLayers[i].name;
            sLayerViews[sSquareOrthoLayers[i].layer]->m_Unknown48 = sSquareOrthoLayers[i].layer;
        }
    }

    {
        GLRenderPair display = fn_802CD82C();
        GLRenderPair pairs[2] = { display, GLRenderPair() };
        for (i = 0; i < 1; i++)
        {
            RLView* view = new (8, false) RLView(&sOrthoCenteredCamera, sAnarkLayers[i].useDisplayTarget ? pairs[0] : pairs[1], sAnarkLayers[i].sortMode);
            sLayerViews[sAnarkLayers[i].layer] = view;
            sLayerViews[sAnarkLayers[i].layer]->m_Name = sAnarkLayers[i].name;
            sLayerViews[sAnarkLayers[i].layer]->m_Unknown48 = sAnarkLayers[i].layer;
        }
    }

    {
        GLRenderPair display = fn_802CD82C();
        GLRenderPair pairs[2] = { display, GLRenderPair() };
        for (i = 0; i < 1; i++)
        {
            RLView* view = new (8, false) RLView(&sCameraSpaceCamera, sCameraSpaceLayers[i].useDisplayTarget ? pairs[0] : pairs[1], sCameraSpaceLayers[i].sortMode);
            sLayerViews[sCameraSpaceLayers[i].layer] = view;
            sLayerViews[sCameraSpaceLayers[i].layer]->m_Name = sCameraSpaceLayers[i].name;
            sLayerViews[sCameraSpaceLayers[i].layer]->m_Unknown48 = sCameraSpaceLayers[i].layer;
        }
    }

    {
        GLRenderPair display = fn_802CD82C();
        GLRenderPair pairs[2] = { display, GLRenderPair() };
        for (i = 0; i < 1; i++)
        {
            RLView* view = new (8, false) RLView(&sTransitions3DCamera, sTransitions3DLayers[i].useDisplayTarget ? pairs[0] : pairs[1], sTransitions3DLayers[i].sortMode);
            sLayerViews[sTransitions3DLayers[i].layer] = view;
            sLayerViews[sTransitions3DLayers[i].layer]->m_Name = sTransitions3DLayers[i].name;
            sLayerViews[sTransitions3DLayers[i].layer]->m_Unknown48 = sTransitions3DLayers[i].layer;
        }
    }

    {
        GLRenderPair display = fn_802CD82C();
        GLRenderPair pairs[2] = { display, GLRenderPair() };
        for (i = 0; i < 2; i++)
        {
            RLView* view = new (8, false) RLView(&sAnark3DCamera, sAnark3DLayers[i].useDisplayTarget ? pairs[0] : pairs[1], sAnark3DLayers[i].sortMode);
            sLayerViews[sAnark3DLayers[i].layer] = view;
            sLayerViews[sAnark3DLayers[i].layer]->m_Name = sAnark3DLayers[i].name;
            sLayerViews[sAnark3DLayers[i].layer]->m_Unknown48 = sAnark3DLayers[i].layer;
        }
    }

    sLayerViews[eCLV_NoFog]->mZWriteEnabled = false;
    sLayerViews[eCLV_HighRange3DNoFog]->mZWriteEnabled = false;
    sLayerViews[eCLV_DepthOfField]->mZWriteEnabled = false;
    sLayerViews[eCLV_BigBlackPolygon]->mZWriteEnabled = false;
    sLayerViews[eCLV_MegastrikeBackground]->mZWriteEnabled = false;
    sLayerViews[eCLV_CoPlanar]->mCoPlanarEnabled = true;

    fn_80272850();

    for (i = 0; i < eCLV_Num; i++)
    {
        sLayerViews[i]->m_Enabled = false;
        lbl_8057F250.m_Children.AddEnd(sLayerViews[i]);
        sLayerViews[i]->m_Parent = &lbl_8057F250;
    }

    sLayerViews[eCLV_Warble]->m_Enabled = true;

    fn_80272388();
    fn_8037091C();
    fn_80370998(sLayerViews[eCLV_ShadowVolume], sLayerViews[eCLV_ShadowVolume]);

    GXColor colour = sShadowVolumeColour;
    CopyShadowVolumeColour(&colour);

    sLayerViews[eCLV_ImpostorTexture]->m_Unknown33 = true;
    sLayerViews[eCLV_ImpostorTexture]->m_ClearColour = true;
    sLayerViews[eCLV_ImpostorTexture]->m_Enabled = true;
    sLayerViews[eCLV_HighRange3D]->m_Enabled = true;
    sLayerViews[eCLV_HighRange3DNoFog]->m_Enabled = true;

    fn_801A2394(lbl_80572020);
}

float fn_8027313C()
{
    return 0.25f;
}

void fn_80273144(const nlMatrix4& view, const nlMatrix4& pipView, float aspect, float fov, float pipAspect, float pipFov)
{
    nlMatrix4 projection;
    glMatrixPerspective(projection, fov, aspect, 0.25f, 4096.0f);

    nlMatrix4 rotation = view;
    rotation.m41 = 0.0f;
    rotation.m42 = 0.0f;
    rotation.m43 = 0.0f;
    rotation.m44 = 1.0f;
    sNoTranslationCamera.Set(rotation, projection);
    sPerspectiveCamera.Set(view, projection);

    nlMatrix4 pipProjection;
    glMatrixPerspective(pipProjection, pipFov, pipAspect, 0.25f, 4096.0f);
    sPipCamera.Set(pipView, pipProjection);

    sCameraSpaceCamera.mProjection = projection;
    sCameraSpaceCamera.mView.SetIdentity();

    glMatrixOrthographic(sOrthoCamera.mMatrix, 640.0f, 480.0f);

    glMatrixOrthographic(sOrtho640Camera.mMatrix, (float)(sWidescreen ? 854 : 640), 480.0f);
    glMatrixOrthographicCentered(sOrthoCenteredCamera.mMatrix, (float)(sWidescreen ? 854 : 640), 480.0f, 0.0f, 5000.0f);

    glMatrixPerspective(sAnark3DCamera.mProjection, 0.4712389f, aspect, 0.25f, 4096.0f);
    sTransitions3DCamera.mProjection = sAnark3DCamera.mProjection;

    nlMatrix4 lookAt;
    nlVector3 eye = { 0.0f, 0.0f, 0.0f };
    nlVector3 at = { 0.0f, 0.0f, -1.0f };
    nlVector3 up = { 0.0f, 1.0f, 0.0f };
    glMatrixLookAt(lookAt, eye, at, up);
    sTransitions3DCamera.mView = lookAt;

    eye.x = 0.0f;
    eye.y = 12.0f;
    eye.z = 0.0f;
    up.x = 0.0f;
    up.y = 0.0f;
    up.z = 1.0f;
    glMatrixLookAt(lookAt, eye, at, up);
    sAnark3DCamera.mView = lookAt;

    fn_8036EB44(0.25f, 4096.0f);
    fn_80271DE0();
}

void fn_80273A14(eCLV layer)
{
    sLayerViews[layer]->m_Visible = true;
}

void fn_80273A30(eCLV layer)
{
    sLayerViews[layer]->m_Visible = false;
}

extern "C" void fn_80273A4C(eCLV layer, const glModel* model, unsigned long key)
{
    RLView* view;
    if (layer < eCLV_Num)
    {
        view = sLayerViews[layer];
    }
    else
    {
        view = 0;
    }

    for (unsigned long i = 0; i < model->numPackets; i++)
    {
        glModelPacket* packet = &model->packets[i];
        if (glGetRasterState(packet->rasterState, GLS_AlphaBlend) == 0)
        {
            view->AttachPacket(packet, key);
        }
        else
        {
            view->AttachPacket(packet, key + 1);
        }
    }
}

void fn_80273AF8(bool widescreen)
{
    sWidescreen = widescreen;
}

extern "C" bool fn_80273B00()
{
    return sWidescreen;
}

const nlMatrix4* RLViewCamera::GetShadowMatrix()
{
    if (mShadowDirty)
    {
        fn_802DCDB4(mShadowMatrix, mProjection, mView);
        mShadowDirty = false;
    }
    return &mShadowMatrix;
}

void RLViewCamera::GetViewProjectionMatrix(nlMatrix4& matrix)
{
    matrix = mViewProjection;
}

void RLViewCamera::GetInverseViewMatrix(nlMatrix4& matrix)
{
    matrix = mViewInverse;
}

void RLViewCamera::GetProjectionMatrix(nlMatrix4& matrix)
{
    matrix = mProjection;
}

const nlMatrix4* RLViewCamera::GetProjectionMatrix() const
{
    return &mProjection;
}

void RLViewCamera::GetViewMatrix(nlMatrix4& matrix)
{
    matrix = mView;
}

const nlMatrix4* RLViewCamera::GetViewMatrix() const
{
    return &mView;
}
