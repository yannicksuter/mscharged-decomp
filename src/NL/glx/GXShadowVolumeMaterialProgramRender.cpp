#include <revolution/gx/GXTev.h>
#include <revolution/gx/GXDisplayList.h>

#include "NL/glx/GXMaterialProgram.h"
#include "NL/glx/glxGX.h"
#include "NL/glx/GXMaterialShadowTweaks.h"
#include "NL/glx/glxDisplayList.h"
#include "NL/nlMemory.h"

static int sShadowVolumeMode;

static void SetShadowVolumeMode(int mode)
{
    if (sShadowVolumeMode == mode)
        return;

    gxSetTevAlphaOp(0, 0, 0, 0, true, 0);
    gxSetTevAlphaIn(0, 7, 7, 7, 7);
    gxSetTevColourOp(0, 0, 0, 0, true, 0);
    gxSetTevColourIn(0, 15, 15, 15, 15);

    switch (mode)
    {
    case 1:
        break;
    case 3:
        gxSetTevColourIn(0, 15, 15, 15, 4);
        gxSetTevAlphaIn(0, 7, 7, 7, 2);
        break;
    case 2:
        gxSetTexCoordGen(0, 1, 4, 60);
        gxSetZMode(false, 4, false);
        gxSetAlphaCompare(7, 0);
        gxSetTevAlphaOp(0, 14, 0, 0, true, 0);
        gxSetTevAlphaIn(0, 4, 7, 1, 7);
        gxSetTevColourIn(0, 15, 15, 15, 2);
        break;
    }

    sShadowVolumeMode = mode;
}

void CopyShadowVolumeColour(const GXColor* colour)
{
    sShadowVolumeRed.value = colour->r;
    sShadowVolumeGreen.value = colour->g;
    sShadowVolumeBlue.value = colour->b;
    sShadowVolumeAlpha.value = colour->a;
}

static inline void SetShadowVolumeColour()
{
    GXColor colour;
    colour.r = sShadowVolumeRed.value;
    colour.g = sShadowVolumeGreen.value;
    colour.b = sShadowVolumeBlue.value;
    colour.a = sShadowVolumeAlpha.value;
    GXSetTevColor(GX_TEVREG0, colour);
}

static inline void SetShadowVolumeConstantColour()
{
    GXColor colour = { 4, 4, 4, 4 };
    GXSetTevColor(GX_TEVREG1, colour);
}

template <>
void GXMaterialProgramImpl<GXShadowVolumeMaterialProgram>::Activate(GLView*)
{
    SetShadowVolumeColour();
    SetShadowVolumeConstantColour();

    static_cast<GXShadowVolumeMaterialProgram*>(this)->ConfigureVertexFormat(true);
    gxSetNumChans(0);
    gxSetNumTexGens(1);
    gxSetNumTevStages(1);
    gxSetTevOrder(0, 0, 0, 255);
    SetShadowVolumeMode(3);
}

template <>
void GXMaterialProgramImpl<GXShadowVolumeMaterialProgram>::Deactivate()
{
    SetShadowVolumeMode(1);
}

template <>
void GXMaterialProgramImpl<GXShadowVolumeMaterialProgram>::Prepare(
    const glModelPacket* packet)
{
    glSetMaterialTextureAlphaState(this, packet, *(unsigned long*)packet->materialParameters);
}

template <>
void GXMaterialProgramImpl<GXShadowVolumeMaterialProgram>::Draw(
    const glModelPacket* packet)
{
    if (*(int*)((unsigned char*)packet->materialParameters + 8) == 0)
        SetShadowVolumeMode(2);
    else
        SetShadowVolumeMode(3);

    static_cast<GXShadowVolumeMaterialProgram*>(this)->BindVertexArrays(packet);
    static_cast<GXShadowVolumeMaterialProgram*>(this)->BindParameters(packet);

    if (packet->displayList != 0)
        GXCallDisplayList(packet->displayList->list, packet->displayList->size);
    else if (packet->indexBuffer != 0)
        static_cast<GXShadowVolumeMaterialProgram*>(this)->DrawIndexed(packet);
    else
        static_cast<GXShadowVolumeMaterialProgram*>(this)->DrawDirect(packet);
}

TweakValueInt sShadowVolumeRed(
    "Red", "/Rendering/ShadowVolume");
TweakValueInt sShadowVolumeGreen(
    "Green", "/Rendering/ShadowVolume");
TweakValueInt sShadowVolumeBlue(
    "Blue", "/Rendering/ShadowVolume");
TweakValueInt sShadowVolumeAlpha(
    "Alpha", "/Rendering/ShadowVolume");
