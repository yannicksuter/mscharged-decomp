#include <revolution/gx/GXTev.h>
#include <revolution/gx/GXDisplayList.h>

#include "NL/glx/GXShadowVolumeMaterialProgram.h"
#include "NL/gl/glMaterialParameters.h"
#include "NL/glx/glxGX.h"
#include "NL/glx/GXMaterialShadowTweaks.h"
#include "NL/glx/glxDisplayList.h"
#include "NL/nlMemory.h"
#include "Game/UnidentifiedStaticStorage.h"

enum ShadowVolumeMode
{
    SHADOW_VOLUME_DISABLED = 1,
    SHADOW_VOLUME_TEXTURE_MASK = 2,
    SHADOW_VOLUME_FIXED_COLOUR = 3,
};

static int sShadowVolumeMode;

static void SetShadowVolumeMode(int mode)
{
    if (sShadowVolumeMode == mode)
        return;

    gxSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, true, GX_TEVPREV);
    gxSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO);
    gxSetTevColourOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, true, GX_TEVPREV);
    gxSetTevColourIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO);

    switch (mode)
    {
    case SHADOW_VOLUME_DISABLED:
        break;
    case SHADOW_VOLUME_FIXED_COLOUR:
        gxSetTevColourIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_C1);
        gxSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_A1);
        break;
    case SHADOW_VOLUME_TEXTURE_MASK:
        gxSetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY);
        gxSetZMode(false, GX_GREATER, false);
        gxSetAlphaCompare(GX_ALWAYS, 0);
        gxSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_COMP_A8_GT, GX_TB_ZERO, GX_CS_SCALE_1, true, GX_TEVPREV);
        gxSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_TEXA, GX_CA_ZERO, GX_CA_A0, GX_CA_ZERO);
        gxSetTevColourIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_C0);
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
    gxSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
    SetShadowVolumeMode(SHADOW_VOLUME_FIXED_COLOUR);
}

template <>
void GXMaterialProgramImpl<GXShadowVolumeMaterialProgram>::Deactivate()
{
    SetShadowVolumeMode(SHADOW_VOLUME_DISABLED);
}

template <>
void GXMaterialProgramImpl<GXShadowVolumeMaterialProgram>::Prepare(
    glModelPacket* packet)
{
    glSetMaterialTextureAlphaState(this, packet, static_cast<const GXShadowVolumeParameters*>(packet->materialParameters)->diffuseTexture.texture);
}

template <>
void GXMaterialProgramImpl<GXShadowVolumeMaterialProgram>::Draw(
    const glModelPacket* packet)
{
    if (static_cast<const GXShadowVolumeParameters*>(packet->materialParameters)->useFixedColour == 0)
        SetShadowVolumeMode(SHADOW_VOLUME_TEXTURE_MASK);
    else
        SetShadowVolumeMode(SHADOW_VOLUME_FIXED_COLOUR);

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
