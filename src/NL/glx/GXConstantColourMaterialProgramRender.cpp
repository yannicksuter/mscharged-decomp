#include <revolution/gx.h>
#include "NL/gl/glMaterialParameters.h"

#include "NL/glx/GXMaterialProgram.h"
#include "NL/glx/glxGX.h"
#include "NL/glx/glxDisplayList.h"
#include "Game/UnidentifiedStaticStorage.h"

static inline GXColor makeColor(float r, float g, float b, float a)
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
    gxSetTevOrder(0, 0, 0, 255);
    GXSetTevKColorSel(GX_TEVSTAGE0, GX_TEV_KCSEL_K0);
    GXSetTevKAlphaSel(GX_TEVSTAGE0, GX_TEV_KASEL_K0_A);
    gxSetTevColourIn(0, 15, 14, 8, 15);
    gxSetTevAlphaIn(0, 7, 6, 4, 7);
}

template <>
void GXMaterialProgramImpl<GXConstantColourMaterialProgram>::Deactivate()
{
}

template <>
void GXMaterialProgramImpl<GXConstantColourMaterialProgram>::Prepare(
    const glModelPacket* packet)
{
    glSetMaterialTextureAlphaState(this, packet, *(unsigned long*)packet->materialParameters);
}

template <>
void GXMaterialProgramImpl<GXConstantColourMaterialProgram>::Draw(
    const glModelPacket* packet)
{
    float* values = (float*)((unsigned char*)packet->materialParameters + 8);
    float r = values[0];
    float g = values[1];
    float b = values[2];
    float a = values[3];
    GXSetTevKColor(GX_KCOLOR0, makeColor(r, g, b, a));

    static_cast<GXConstantColourMaterialProgram*>(this)->BindVertexArrays(packet);
    static_cast<GXConstantColourMaterialProgram*>(this)->BindParameters(packet);

    if (packet->displayList != 0)
        GXCallDisplayList(packet->displayList->list, packet->displayList->size);
    else if (packet->indexBuffer != 0)
        static_cast<GXConstantColourMaterialProgram*>(this)->DrawIndexed(packet);
    else
        static_cast<GXConstantColourMaterialProgram*>(this)->DrawDirect(packet);
}
