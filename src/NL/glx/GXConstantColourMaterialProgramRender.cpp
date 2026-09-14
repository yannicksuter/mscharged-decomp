#include <revolution/gx.h>
#include "NL/gl/glMaterialParameters.h"

#include "NL/glx/GXConstantColourMaterialProgram.h"
#include "NL/glx/glxGX.h"
#include "NL/glx/glxDisplayList.h"
#include "Game/UnidentifiedStaticStorage.h"

static inline GXColor MakeGXColour(float r, float g, float b, float a)
{
    GXColor colour;
    colour.r = (unsigned char)(r * 255.0f);
    colour.g = (unsigned char)(g * 255.0f);
    colour.b = (unsigned char)(b * 255.0f);
    colour.a = (unsigned char)(a * 255.0f);
    return colour;
}

template <>
void GXMaterialProgramImpl<GXConstantColourMaterialProgram>::Activate(
    GLView*)
{
    static_cast<GXConstantColourMaterialProgram*>(this)->ConfigureVertexFormat(true);
    gxSetNumChans(0);
    gxSetNumTexGens(1);
    gxSetNumTevStages(1);
    gxSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
    GXSetTevKColorSel(GX_TEVSTAGE0, GX_TEV_KCSEL_K0);
    GXSetTevKAlphaSel(GX_TEVSTAGE0, GX_TEV_KASEL_K0_A);
    gxSetTevColourIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_KONST, GX_CC_TEXC, GX_CC_ZERO);
    gxSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_KONST, GX_CA_TEXA, GX_CA_ZERO);
}

template <>
void GXMaterialProgramImpl<GXConstantColourMaterialProgram>::Deactivate()
{
}

template <>
void GXMaterialProgramImpl<GXConstantColourMaterialProgram>::Prepare(
    glModelPacket* packet)
{
    glSetMaterialTextureAlphaState(this, packet, static_cast<const GXConstantColourParameters*>(packet->materialParameters)->diffuseTexture.texture);
}

template <>
void GXMaterialProgramImpl<GXConstantColourMaterialProgram>::Draw(
    const glModelPacket* packet)
{
    const nlFloatColour& colour = static_cast<const GXConstantColourParameters*>(packet->materialParameters)->constantColour;
    float r = colour.c[0];
    float g = colour.c[1];
    float b = colour.c[2];
    float a = colour.c[3];
    GXSetTevKColor(GX_KCOLOR0, MakeGXColour(r, g, b, a));

    static_cast<GXConstantColourMaterialProgram*>(this)->BindVertexArrays(packet);
    static_cast<GXConstantColourMaterialProgram*>(this)->BindParameters(packet);

    if (packet->displayList != 0)
        GXCallDisplayList(packet->displayList->list, packet->displayList->size);
    else if (packet->indexBuffer != 0)
        static_cast<GXConstantColourMaterialProgram*>(this)->DrawIndexed(packet);
    else
        static_cast<GXConstantColourMaterialProgram*>(this)->DrawDirect(packet);
}
