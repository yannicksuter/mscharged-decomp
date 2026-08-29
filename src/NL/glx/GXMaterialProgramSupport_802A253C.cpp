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
void GXMaterialProgramImpl<GXMaterialProgram_802A4F0C>::Activate(
    GLView*)
{
    static_cast<GXMaterialProgram_802A4F0C*>(this)->ConfigureVertexFormat(true);
    gxSetNumChans(0);
    gxSetNumTevStages(2);
    gxSetNumTexGens(2);
    gxSetTevOrder(0, 0, 0, 255);
    gxSetTevOrder(1, 1, 1, 255);
    GXSetTevKColorSel(GX_TEVSTAGE0, GX_TEV_KCSEL_K0);
    GXSetTevKAlphaSel(GX_TEVSTAGE0, GX_TEV_KASEL_K0_A);
    GXSetTevKColorSel(GX_TEVSTAGE1, GX_TEV_KCSEL_K0);
    GXSetTevKAlphaSel(GX_TEVSTAGE1, GX_TEV_KASEL_K0_A);
    gxSetTevColourIn(0, 8, 15, 14, 15);
    gxSetTevAlphaIn(0, 4, 7, 6, 7);
    gxSetTevColourIn(1, 15, 8, 14, 0);
    gxSetTevAlphaIn(1, 7, 4, 6, 0);
}

template <>
void GXMaterialProgramImpl<GXMaterialProgram_802A4F0C>::Deactivate()
{
    gxSetNumTevStages(1);
    gxSetNumTexGens(1);
}

template <>
void GXMaterialProgramImpl<GXMaterialProgram_802A4F0C>::Prepare(
    const glModelPacket* packet)
{
    fn_802CC978(this, packet, *(unsigned long*)packet->unknown20);
}

template <>
void GXMaterialProgramImpl<GXMaterialProgram_802A4F0C>::Draw(
    const glModelPacket* packet)
{
    static_cast<GXMaterialProgram_802A4F0C*>(this)->BindVertexArrays(packet);
    static_cast<GXMaterialProgram_802A4F0C*>(this)->BindParameters(packet);

    float value = *(float*)((unsigned char*)packet->unknown20 + 16);
    float values[4] = { value, value, value, value };
    GXSetTevKColor(
        GX_KCOLOR0, makeColor(values[0], values[1], values[2], values[3]));

    if (packet->displayList != 0)
        GXCallDisplayList(packet->displayList->list, packet->displayList->size);
    else if (packet->indexBuffer != 0)
        static_cast<GXMaterialProgram_802A4F0C*>(this)->DrawIndexed(packet);
    else
        static_cast<GXMaterialProgram_802A4F0C*>(this)->DrawDirect(packet);
}
