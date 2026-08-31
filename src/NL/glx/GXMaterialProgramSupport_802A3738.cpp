#include <revolution/gx/GXTev.h>
#include <revolution/gx/GXDisplayList.h>

#include "NL/glx/GXMaterialProgram.h"
#include "NL/glx/glxGX.h"
#include "NL/glx/GXMaterialShadowTweaks.h"
#include "NL/glx/glxDisplayList.h"
#include "NL/nlMemory.h"

extern const char* lbl_806E1E90;

static int sShadowVolumeMode;

static void fn_802A3738(int mode)
{
    if (sShadowVolumeMode == mode)
        return;

    gxSetTevAlphaOp(0, 0, 0, 0, true, 0);
    gxSetTevAlphaIn(0, 7, 7, 7, 7);
    gxSetTevColourOp(0, 0, 0, 0, true, 0);
    gxSetTevColourIn(0, 15, 15, 15, 15);

    switch (mode)
    {
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
void GXMaterialProgramImpl<GXMaterialProgram_802A73B0>::Activate(GLView*)
{
    SetShadowVolumeColour();
    SetShadowVolumeConstantColour();

    static_cast<GXMaterialProgram_802A73B0*>(this)->ConfigureVertexFormat(true);
    gxSetNumChans(0);
    gxSetNumTexGens(1);
    gxSetNumTevStages(1);
    gxSetTevOrder(0, 0, 0, 255);
    fn_802A3738(3);
}

template <>
void GXMaterialProgramImpl<GXMaterialProgram_802A73B0>::Deactivate()
{
    fn_802A3738(1);
}

template <>
void GXMaterialProgramImpl<GXMaterialProgram_802A73B0>::Prepare(
    const glModelPacket* packet)
{
    fn_802CC978(this, packet, *(unsigned long*)packet->unknown20);
}

template <>
void GXMaterialProgramImpl<GXMaterialProgram_802A73B0>::Draw(
    const glModelPacket* packet)
{
    if (*(int*)((unsigned char*)packet->unknown20 + 8) == 0)
        fn_802A3738(2);
    else
        fn_802A3738(3);

    static_cast<GXMaterialProgram_802A73B0*>(this)->BindVertexArrays(packet);
    static_cast<GXMaterialProgram_802A73B0*>(this)->BindParameters(packet);

    if (packet->displayList != 0)
        GXCallDisplayList(packet->displayList->list, packet->displayList->size);
    else if (packet->indexBuffer != 0)
        static_cast<GXMaterialProgram_802A73B0*>(this)->DrawIndexed(packet);
    else
        static_cast<GXMaterialProgram_802A73B0*>(this)->DrawDirect(packet);
}

GXMaterialColourTweak_804FC520 sShadowVolumeRed(
    "Red", "/Rendering/ShadowVolume");
GXMaterialColourTweak_804FC520 sShadowVolumeGreen(
    "Green", "/Rendering/ShadowVolume");
GXMaterialColourTweak_804FC520 sShadowVolumeBlue(
    "Blue", "/Rendering/ShadowVolume");
GXMaterialColourTweak_804FC520 sShadowVolumeAlpha(
    "Alpha", "/Rendering/ShadowVolume");
