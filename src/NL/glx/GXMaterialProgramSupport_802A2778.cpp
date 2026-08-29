#include <revolution/gx.h>

#include "NL/glx/GXMaterialProgram.h"
#include "NL/glx/glxGX.h"

static inline GXColor makeColor(float r, float g, float b, float a)
{
    GXColor colour;
    colour.r = (unsigned char)(r * 255.0f);
    colour.g = (unsigned char)(g * 255.0f);
    colour.b = (unsigned char)(b * 255.0f);
    colour.a = (unsigned char)(a * 255.0f);
    return colour;
}

static void fn_802A2778()
{
    gxSetNumChans(1);
    gxSetNumTevStages(3);
    gxSetNumTexGens(2);
    gxSetTexCoordGen(0, 1, 4, 60);
    gxSetTexCoordGen(1, 1, 5, 60);
    gxSetTevOrder(0, 0, 0, 255);
    gxSetTevOrder(1, 1, 1, 255);
    gxSetTevOrder(2, 255, 255, 4);
    gxSetTevColourOp(0, 0, 0, 0, true, 1);
    gxSetTevColourOp(1, 0, 0, 0, true, 2);
    gxSetTevColourOp(2, 0, 0, 0, true, 0);
    gxSetTevColourIn(0, 15, 15, 15, 8);
    gxSetTevAlphaIn(0, 7, 7, 7, 4);
    gxSetTevColourIn(1, 2, 8, 9, 15);
    gxSetTevAlphaIn(1, 7, 7, 7, 6);
    gxSetTevColourIn(2, 15, 4, 10, 15);
    gxSetTevAlphaIn(2, 7, 7, 7, 6);
}

template <>
void GXMaterialProgramImpl<GXMaterialProgram_802A53B4>::Activate(
    GLView*)
{
    GXSetChanCtrl(GX_COLOR0A0, false, GX_SRC_REG, GX_SRC_VTX, GX_LIGHT0, GX_DF_NONE, GX_AF_NONE);
    static_cast<GXMaterialProgram_802A53B4*>(this)->ConfigureVertexFormat(true);
    fn_802A2778();
}

template <>
void GXMaterialProgramImpl<GXMaterialProgram_802A53B4>::Deactivate()
{
    gxSetNumTevStages(1);
    gxSetNumTexGens(1);
}

template <>
void GXMaterialProgramImpl<GXMaterialProgram_802A53B4>::Prepare(
    const glModelPacket* packet)
{
    fn_802CC978(this, packet, *(unsigned long*)packet->unknown20);
}

template <>
void GXMaterialProgramImpl<GXMaterialProgram_802A53B4>::Draw(
    const glModelPacket* packet)
{
    static_cast<GXMaterialProgram_802A53B4*>(this)->BindVertexArrays(packet);
    static_cast<GXMaterialProgram_802A53B4*>(this)->BindParameters(packet);

    float values[4] = { 0.0f, 0.0f, 0.0f, 255.0f };
    float value = *(float*)((unsigned char*)packet->unknown20 + 16);
    values[0] = value;
    values[1] = value;
    values[2] = value;
    unsigned char colour = makeColor(values[0], values[1], values[2], values[3]).r;

    if (packet->indexBuffer != 0)
        static_cast<GXMaterialProgram_802A53B4*>(this)->DrawIndexed(
            packet, colour);
    else
        static_cast<GXMaterialProgram_802A53B4*>(this)->DrawDirect(
            packet, colour);
}
