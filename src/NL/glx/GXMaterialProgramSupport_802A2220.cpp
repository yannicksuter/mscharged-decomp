#include <revolution/gx.h>

#include "NL/glx/GXMaterialProgram.h"
#include "NL/glx/glxGX.h"
#include "NL/glx/glxDisplayList.h"

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
void GXMaterialProgramImpl<GXMaterialProgram_802A4360>::Activate(
    GLView*)
{
    static_cast<GXMaterialProgram_802A4360*>(this)->ConfigureVertexFormat(true);
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
void GXMaterialProgramImpl<GXMaterialProgram_802A4360>::Deactivate()
{
}

template <>
void GXMaterialProgramImpl<GXMaterialProgram_802A4360>::Prepare(
    const glModelPacket* packet)
{
    fn_802CC978(this, packet, *(unsigned long*)packet->unknown20);
}

template <>
void GXMaterialProgramImpl<GXMaterialProgram_802A4360>::Draw(
    const glModelPacket* packet)
{
    float* values = (float*)((unsigned char*)packet->unknown20 + 8);
    float r = values[0];
    float g = values[1];
    float b = values[2];
    float a = values[3];
    GXSetTevKColor(GX_KCOLOR0, makeColor(r, g, b, a));

    static_cast<GXMaterialProgram_802A4360*>(this)->BindVertexArrays(packet);
    static_cast<GXMaterialProgram_802A4360*>(this)->BindParameters(packet);

    if (packet->displayList != 0)
        GXCallDisplayList(packet->displayList->list, packet->displayList->size);
    else if (packet->indexBuffer != 0)
        static_cast<GXMaterialProgram_802A4360*>(this)->DrawIndexed(packet);
    else
        static_cast<GXMaterialProgram_802A4360*>(this)->DrawDirect(packet);
}
