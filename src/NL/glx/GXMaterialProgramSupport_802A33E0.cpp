#include <revolution/gx.h>

#include "NL/glx/GXMaterialProgram.h"
#include "NL/glx/glxGX.h"
#include "NL/glx/glxDisplayList.h"

extern "C"
{
    unsigned long fn_80369394();
    unsigned long fn_803693A4();
}

template <>
void GXMaterialProgramImpl<GXMaterialProgram_802A6B6C>::Activate(
    GLView*)
{
    gxSetNumChans(1);
    gxSetNumTevStages(1);
    gxSetNumTexGens(1);
    gxSetTevColourOp(0, 0, 0, 0, true, 0);
    gxSetTevAlphaOp(0, 0, 0, 0, true, 0);
    gxSetTevOrder(0, 0, 0, 4);
    gxSetTevColourIn(0, 15, 10, 8, 15);
    gxSetTevAlphaIn(0, 7, 5, 4, 7);
    static_cast<GXMaterialProgram_802A6B6C*>(this)->ConfigureVertexFormat(true);
}

template <>
void GXMaterialProgramImpl<GXMaterialProgram_802A6B6C>::Deactivate()
{
    GXSetScissor(0, 0, fn_80369394(), fn_803693A4());
}

template <>
void GXMaterialProgramImpl<GXMaterialProgram_802A6B6C>::Prepare(
    const glModelPacket* packet)
{
    GXMaterialProgramParameters_802A6B6C* parameters = (GXMaterialProgramParameters_802A6B6C*)packet->unknown20;
    fn_802CC978(this, packet, parameters->texture);
}

template <>
void GXMaterialProgramImpl<GXMaterialProgram_802A6B6C>::Draw(
    const glModelPacket* packet)
{
    GXMaterialProgramParameters_802A6B6C& parameters = *(GXMaterialProgramParameters_802A6B6C*)packet->unknown20;
    float scissorX = parameters.scissorX;

    if (scissorX <= -0.1f)
    {
        GXSetScissor(0, 0, fn_80369394(), fn_803693A4());
    }
    else
    {
        GXSetScissor((int)scissorX,
            (int)parameters.scissorY,
            (int)parameters.scissorWidth,
            (int)parameters.scissorHeight);
    }

    static_cast<GXMaterialProgram_802A6B6C*>(this)->BindVertexArrays(packet);
    static_cast<GXMaterialProgram_802A6B6C*>(this)->BindParameters(packet);

    if (packet->displayList != 0)
    {
        GXCallDisplayList(packet->displayList->list, packet->displayList->size);
    }
    else if (packet->indexBuffer != 0)
    {
        static_cast<GXMaterialProgram_802A6B6C*>(this)->DrawIndexed(packet);
    }
    else
    {
        static_cast<GXMaterialProgram_802A6B6C*>(this)->DrawDirect(packet);
    }
}
