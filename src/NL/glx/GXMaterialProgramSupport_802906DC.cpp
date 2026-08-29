#include <revolution/gx.h>

#include "NL/glx/GXMaterialProgram.h"
#include "NL/glx/glxGX.h"
#include "NL/glx/glxDisplayList.h"

template <>
void GXMaterialProgramImpl<GXMaterialProgram_8029B434>::Activate(GLView*)
{
    static_cast<GXMaterialProgram_8029B434*>(this)->ConfigureVertexFormat(true);
    gxSetNumChans(1);
    gxSetNumTevStages(4);
    gxSetNumTexGens(3);
    gxSetTevColourOp(0, 0, 0, 0, true, 1);
    gxSetTevOrder(0, 1, 1, 255);
    gxSetTevOrder(1, 0, 0, 4);
    gxSetTevOrder(2, 2, 2, 255);
    gxSetTevOrder(3, 1, 1, 255);
    gxSetTevColourIn(0, 15, 15, 15, 8);
    gxSetTevColourIn(1, 15, 10, 8, 15);
    gxSetTevColourIn(2, 2, 0, 9, 15);
    gxSetTevColourIn(3, 15, 15, 15, 0);
    gxSetTevAlphaIn(0, 7, 7, 7, 7);
    gxSetTevAlphaIn(1, 7, 7, 7, 7);
    gxSetTevAlphaIn(2, 7, 7, 7, 7);
    gxSetTevAlphaIn(3, 7, 7, 7, 4);
}

template <>
void GXMaterialProgramImpl<GXMaterialProgram_8029B434>::Deactivate()
{
    gxSetTevColourOp(0, 0, 0, 0, true, 0);
}

template <>
void GXMaterialProgramImpl<GXMaterialProgram_8029B434>::Prepare(
    const glModelPacket* packet)
{
    fn_802CC978(this, packet, *(unsigned long*)packet->unknown20);
}

template <>
void GXMaterialProgramImpl<GXMaterialProgram_8029B434>::Draw(
    const glModelPacket* packet)
{
    static_cast<GXMaterialProgram_8029B434*>(this)->BindVertexArrays(packet);
    static_cast<GXMaterialProgram_8029B434*>(this)->BindParameters(packet);

    if (packet->displayList != 0)
        GXCallDisplayList(packet->displayList->list, packet->displayList->size);
    else if (packet->indexBuffer != 0)
        static_cast<GXMaterialProgram_8029B434*>(this)->DrawIndexed(packet);
    else
        static_cast<GXMaterialProgram_8029B434*>(this)->DrawDirect(packet);
}
