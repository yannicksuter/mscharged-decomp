#include <revolution/gx.h>
#include <revolution/mtx.h>

#include "Game/TweakValue.h"
#include "NL/gl/glMatrix.h"
#include "NL/gl/glState.h"
#include "NL/gl/glView.h"
#include "NL/glx/GXMaterialCrystalTweaks.h"
#include "NL/gl/glMaterialProgram.h"
#include "NL/gl/glModel.h"
#include "NL/glx/GXMaterialShadowTweaks.h"
#include "NL/glx/glxGX.h"
#include "NL/glx/glxMatrix.h"
#include "NL/glx/glxSend.h"
#include "NL/platvmath.h"

static GXCompare gx_DepthFunc[] = {
    GX_ALWAYS,
    GX_LEQUAL,
    GX_EQUAL,
    GX_LESS,
};

static GXCullMode gx_Culling[] = {
    GX_CULL_NONE,
    GX_CULL_FRONT,
    GX_CULL_BACK,
    GX_CULL_ALL,
};

static s32 glx_Matrix = -1;
static bool glx_AllowAlphaUpdate = true;

static GXCompare gx_AlphaTest[] = {
    GX_ALWAYS,
    GX_GREATER,
};

static float glx_FogNear = 0.25f;
static float glx_FogFar = 130.0f;

static GLView* prev_view;
static GLMaterialProgram* glx_program;
static unsigned long glx_DirtyFlags;

static nlMatrix4 mview;
static nlMatrix4 mproj;
static nlMatrix4 viewproj;
static nlMatrix4 modelview;

static TweakValueFloat glx_FogStart(
    "sfFogStart", "/Render/Fog", 5.0f);
static TweakValueFloat glx_FogEnd(
    "sfFogEnd", "/Render/Fog", 160.0f);
static TweakValueInt glx_FogRed(
    "siFogRed", gLastTweakCategory, 255);
static TweakValueInt glx_FogGreen(
    "siFogGreen", gLastTweakCategory, 255);
static TweakValueInt glx_FogBlue(
    "siFogBlue", gLastTweakCategory, 255);
static TweakValueFloat glx_FogIntensity(
    "sfFogIntensity", gLastTweakCategory, 1.0f);
static TweakValueBool glx_bFog(
    "sbFogEnabled", gLastTweakCategory, false);
static TweakValueInt glx_FogType(
    "siFogType", gLastTweakCategory, 0);

static void glx_SwitchViews(GLView* view);
static void glx_SwitchRaster(const glModelPacket* p);

static inline bool glx_HasNormals(const glModelPacket* p)
{
    glModelStream* stream = p->streams;
    glModelStream* end = stream + p->numStreams;
    while (stream < end)
    {
        if ((char)stream->id == 2)
        {
            return true;
        }
        ++stream;
    }
    return false;
}

void glx_SendEnd()
{
    if (glx_program != 0)
    {
        glx_program->Deactivate();
    }
    glx_program = 0;
    glx_AllowAlphaUpdate = true;
    glx_DirtyFlags = 0;
}

void glx_SendFrame_cb(
    GLView* view, unsigned long flags, const glModelPacket* p)
{
    flags |= glx_DirtyFlags;
    glx_DirtyFlags = 0;

    if (p != 0)
    {
        GLMaterialProgram* program
            = (GLMaterialProgram*)p->materialProgram;
        if (glx_program != program)
        {
            if (glx_program != 0)
            {
                glx_program->Deactivate();
                gxSetTevColourOp(0, 0, 0, 0, true, 0);
                gxSetTevAlphaOp(0, 0, 0, 0, true, 0);
                gxSetTexCoordGen(0, 1, 4, 0x3C);
                gxSetNumTexGens(1);
                gxSetNumChans(0);
                gxSetNumTevStages(1);
                gxSetTevOrder(0, 0, 0, 0xFF);
                gxSetTevColourIn(0, 15, 12, 8, 15);
                gxSetTevAlphaIn(0, 7, 6, 4, 7);
            }
            program->Activate(prev_view);
            glx_program = program;
            glx_Matrix = -1;
        }

        if ((flags & 4) != 0)
        {
            Mtx inverse;
            nlMatrix4 matrix;
            Mtx source;

            unsigned long Matrix = p->matrix;
            if (glx_Matrix != Matrix)
            {
                glx_Matrix = Matrix;
                glGetMatrix(Matrix, matrix);
                nlMultMatrices(modelview, matrix, mview);
            }

            glxCopyMatrix(source, modelview);
            GXLoadPosMtxImm(source, 0);

            if (glx_HasNormals(p))
            {
                PSMTXInvXpose(source, inverse);
                GXLoadNrmMtxImm(inverse, 0);
            }
            gxSetCurrentMtx(0, false);
        }

        if ((flags & 2) != 0)
        {
            glx_SwitchRaster(p);
        }
        if ((flags & 0x80) != 0)
        {
            ((GLMaterialProgram*)p->materialProgram)->Draw(p);
        }
    }
    else
    {
        glx_SwitchViews(view);
        glx_DirtyFlags |= 2;
    }
}

static void glx_SwitchViews(GLView* view)
{
    prev_view = view;
    glx_AllowAlphaUpdate = view->m_Enabled;
    view->m_Interface->GetViewMatrix(mview);
    view->m_Interface->GetProjectionMatrix(mproj);
    nlMultMatrices(viewproj, mview, mproj);

    Mtx gx_mview;
    Mtx44 gx_proj;
    glxCopyMatrix(gx_mview, mview);
    glxCopyMatrix(gx_proj, mproj);
    if (mproj.m43 == -1.0f)
    {
        GXSetProjection(gx_proj, GX_PERSPECTIVE);
    }
    else
    {
        GXSetProjection(gx_proj, GX_ORTHOGRAPHIC);
    }

    if (glx_program != 0)
    {
        glx_program->Deactivate();
    }
    glx_program = 0;
    glx_Matrix = -1;
}

static void glx_SwitchRaster(const glModelPacket* p)
{
    unsigned long raster = p->rasterState;
    bool DepthTest = glGetRasterState(raster, GLS_DepthTest) != 0;
    bool DepthWrite = glGetRasterState(raster, GLS_DepthWrite) != 0;
    int DepthFunc = glGetRasterState(raster, GLS_DepthFunc);
    gxSetZMode(DepthTest, gx_DepthFunc[DepthFunc], DepthWrite);

    int Culling = glGetRasterState(raster, GLS_Culling);
    gxSetCullMode(gx_Culling[Culling]);

    int AlphaTest = glGetRasterState(raster, GLS_AlphaTest);
    unsigned long AlphaTestRef = glGetRasterState(raster, GLS_AlphaTestRef);
    gxSetAlphaCompare(gx_AlphaTest[AlphaTest], (unsigned char)AlphaTestRef);
    gxSetZCompLoc(AlphaTest == 0);

    unsigned long AlphaBlend = glGetRasterState(raster, GLS_AlphaBlend);
    switch (AlphaBlend)
    {
    case 0:
        gxSetBlendMode(false, 1, 0, false);
        break;
    case 1:
        gxSetBlendMode(true, 4, 5, false);
        break;
    case 2:
        gxSetBlendMode(true, 1, 1, false);
        break;
    case 3:
        gxSetBlendMode(true, 4, 1, false);
        break;
    case 4:
        gxSetBlendMode(true, 2, 0, false);
        break;
    case 5:
        gxSetBlendMode(true, 3, 1, false);
        break;
    case 6:
        gxSetBlendMode(true, 1, 0, false);
        break;
    case 7:
        gxSetBlendMode(true, 2, 0, true);
        break;
    }

    int ColourWrite = glGetRasterState(raster, GLS_ColourWrite);
    switch (ColourWrite)
    {
    case 0:
        gxSetColourUpdate(false);
        gxSetAlphaUpdate(false);
        break;
    case 1:
        gxSetColourUpdate(true);
        gxSetAlphaUpdate(false);
        break;
    case 2:
        gxSetColourUpdate(false);
        gxSetAlphaUpdate(true);
        break;
    case 3:
        gxSetColourUpdate(true);
        gxSetAlphaUpdate(glx_AllowAlphaUpdate);
        break;
    }
}

static GXFogType fogtype[] = {
    GX_FOG_PERSP_LIN,
    GX_FOG_PERSP_EXP,
    GX_FOG_PERSP_EXP2,
    GX_FOG_PERSP_REVEXP,
    GX_FOG_PERSP_REVEXP2,
};

void glx_SetFogClipPlanes(float nearPlane, float farPlane)
{
    glx_FogNear = nearPlane;
    glx_FogFar = farPlane;
}

float glx_GetFogStart()
{
    return glx_FogStart.value;
}

void glx_SetFogStart(float value)
{
    glx_FogStart.value = value;
}

float glx_GetFogEnd()
{
    return glx_FogEnd.value;
}

void glx_SetFogEnd(float value)
{
    glx_FogEnd.value = value;
}

void glx_Fog(bool enable)
{
    if (enable && glx_bFog)
    {
        s32 r = (s32)(glx_FogIntensity.value * glx_FogRed.value);
        s32 g = (s32)(glx_FogIntensity.value * glx_FogGreen.value);
        s32 b = (s32)(glx_FogIntensity.value * glx_FogBlue.value);
        GXColor fogColour;
        fogColour.r = r;
        fogColour.g = g;
        fogColour.b = b;
        fogColour.a = 0xFF;
        GXSetFog(fogtype[glx_FogType.value], fogColour, glx_FogStart.value, glx_FogEnd.value, glx_FogNear, glx_FogFar);
    }
    else
    {
        GXColor fogColour;
        fogColour.r = 0xFF;
        fogColour.g = 0xFF;
        fogColour.b = 0xFF;
        fogColour.a = 0xFF;
        GXSetFog(GX_FOG_NONE, fogColour, 0.0f, 0.0f, 0.0f, 0.0f);
    }
}
