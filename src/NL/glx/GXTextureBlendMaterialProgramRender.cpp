#include <revolution/gx.h>
#include "NL/gl/glMaterialParameters.h"

#include "NL/glx/GXTextureBlendMaterialProgram.h"
#include "NL/nlColour.h"
#include "NL/glx/glxGX.h"
#include "NL/glx/glxDisplayList.h"
#include "Game/UnidentifiedStaticStorage.h"

static inline GXColor MakeTextureBlendColour(float r, float g, float b, float a)
{
    GXColor colour;
    colour.r = (unsigned char)(r * 255.0f);
    colour.g = (unsigned char)(g * 255.0f);
    colour.b = (unsigned char)(b * 255.0f);
    colour.a = (unsigned char)(a * 255.0f);
    return colour;
}

template <>
void GXMaterialProgramImpl<GXTextureBlendMaterialProgram>::Activate(
    GLView*)
{
    static_cast<GXTextureBlendMaterialProgram*>(this)->ConfigureVertexFormat(true);
    gxSetNumChans(0);
    gxSetNumTevStages(2);
    gxSetNumTexGens(2);
    gxSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
    gxSetTevOrder(GX_TEVSTAGE1, GX_TEXCOORD1, GX_TEXMAP1, GX_COLOR_NULL);
    GXSetTevKColorSel(GX_TEVSTAGE0, GX_TEV_KCSEL_K0);
    GXSetTevKAlphaSel(GX_TEVSTAGE0, GX_TEV_KASEL_K0_A);
    GXSetTevKColorSel(GX_TEVSTAGE1, GX_TEV_KCSEL_K0);
    GXSetTevKAlphaSel(GX_TEVSTAGE1, GX_TEV_KASEL_K0_A);
    gxSetTevColourIn(GX_TEVSTAGE0, GX_CC_TEXC, GX_CC_ZERO, GX_CC_KONST, GX_CC_ZERO);
    gxSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_TEXA, GX_CA_ZERO, GX_CA_KONST, GX_CA_ZERO);
    gxSetTevColourIn(GX_TEVSTAGE1, GX_CC_ZERO, GX_CC_TEXC, GX_CC_KONST, GX_CC_CPREV);
    gxSetTevAlphaIn(GX_TEVSTAGE1, GX_CA_ZERO, GX_CA_TEXA, GX_CA_KONST, GX_CA_APREV);
}

template <>
void GXMaterialProgramImpl<GXTextureBlendMaterialProgram>::Deactivate()
{
    gxSetNumTevStages(1);
    gxSetNumTexGens(1);
}

template <>
void GXMaterialProgramImpl<GXTextureBlendMaterialProgram>::Prepare(
    glModelPacket* packet)
{
    glSetMaterialTextureAlphaState(this, packet, static_cast<const GXTextureBlendParameters*>(packet->materialParameters)->diffuseTexture.texture);
}

template <>
void GXMaterialProgramImpl<GXTextureBlendMaterialProgram>::Draw(
    const glModelPacket* packet)
{
    static_cast<GXTextureBlendMaterialProgram*>(this)->BindVertexArrays(packet);
    static_cast<GXTextureBlendMaterialProgram*>(this)->BindParameters(packet);

    float blendAmount = static_cast<const GXTextureBlendParameters*>(packet->materialParameters)->blendAmount;
    nlFloatColour blendColour = { { blendAmount, blendAmount, blendAmount, blendAmount } };
    GXSetTevKColor(
        GX_KCOLOR0, MakeTextureBlendColour(blendColour.c[0], blendColour.c[1], blendColour.c[2], blendColour.c[3]));

    if (packet->displayList != 0)
        GXCallDisplayList(packet->displayList->list, packet->displayList->size);
    else if (packet->indexBuffer != 0)
        static_cast<GXTextureBlendMaterialProgram*>(this)->DrawIndexed(packet);
    else
        static_cast<GXTextureBlendMaterialProgram*>(this)->DrawDirect(packet);
}
