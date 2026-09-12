#include <revolution/gx.h>

#include "NL/glx/GXTextureColourAddMaterialProgram.h"
#include "NL/glx/glxGX.h"
#include "NL/glx/glxDisplayList.h"
#include "Game/UnidentifiedStaticStorage.h"

static inline GXColor MakeTextureAddColour(float r, float g, float b, float a)
{
    GXColor colour;
    colour.r = (unsigned char)(r * 255.0f);
    colour.g = (unsigned char)(g * 255.0f);
    colour.b = (unsigned char)(b * 255.0f);
    colour.a = (unsigned char)(a * 255.0f);
    return colour;
}

template <>
void GXMaterialProgramImpl<GXTextureColourAddMaterialProgram>::Activate(
    GLView*)
{
    static_cast<GXTextureColourAddMaterialProgram*>(this)->ConfigureVertexFormat(true);
    gxSetNumChans(0);
    gxSetNumTexGens(1);
    gxSetNumTevStages(1);
    gxSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
    GXSetTevKColorSel(GX_TEVSTAGE0, GX_TEV_KCSEL_K0);
    GXSetTevKAlphaSel(GX_TEVSTAGE0, GX_TEV_KASEL_K0_A);
    gxSetTevColourIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ONE, GX_CC_TEXC, GX_CC_KONST);
    gxSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_KONST, GX_CA_TEXA, GX_CA_KONST);
}

template <>
void GXMaterialProgramImpl<GXTextureColourAddMaterialProgram>::Deactivate()
{
}

template <>
void GXMaterialProgramImpl<GXTextureColourAddMaterialProgram>::Prepare(
    glModelPacket*)
{
}

template <>
void GXMaterialProgramImpl<GXTextureColourAddMaterialProgram>::Draw(
    const glModelPacket* packet)
{
    const nlFloatColour& colour = static_cast<const GXTextureColourAddParameters*>(packet->materialParameters)->colour;
    float r = colour.c[0];
    float g = colour.c[1];
    float b = colour.c[2];
    float a = colour.c[3];
    GXSetTevKColor(GX_KCOLOR0, MakeTextureAddColour(r, g, b, a));

    static_cast<GXTextureColourAddMaterialProgram*>(this)->BindVertexArrays(packet);
    static_cast<GXTextureColourAddMaterialProgram*>(this)->BindParameters(packet);

    if (packet->displayList != 0)
        GXCallDisplayList(packet->displayList->list, packet->displayList->size);
    else if (packet->indexBuffer != 0)
        static_cast<GXTextureColourAddMaterialProgram*>(this)->DrawIndexed(packet);
    else
        static_cast<GXTextureColourAddMaterialProgram*>(this)->DrawDirect(packet);
}
