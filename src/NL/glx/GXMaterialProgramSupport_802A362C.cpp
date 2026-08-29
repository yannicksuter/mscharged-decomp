#include <revolution/gx.h>

#include "NL/glx/GXMaterialProgram.h"
#include "NL/glx/glxGX.h"
#include "NL/glx/glxDisplayList.h"

template <>
void GXMaterialProgramImpl<GXMaterialProgram_802A6FDC>::Activate(
    GLView*)
{
    static_cast<GXMaterialProgram_802A6FDC*>(this)->ConfigureVertexFormat(true);
    gxSetNumChans(1);
    gxSetNumTexGens(0);
    gxSetNumTevStages(1);
    gxSetTevOrder(0, 255, 255, 4);
    GXSetTevOp(GX_TEVSTAGE0, GX_PASSCLR);
}

template <>
void GXMaterialProgramImpl<GXMaterialProgram_802A6FDC>::Deactivate()
{
}

template <>
void GXMaterialProgramImpl<GXMaterialProgram_802A6FDC>::Prepare(
    const glModelPacket*)
{
}

template <>
void GXMaterialProgramImpl<GXMaterialProgram_802A6FDC>::Draw(
    const glModelPacket* packet)
{
    static_cast<GXMaterialProgram_802A6FDC*>(this)->BindVertexArrays(packet);
    static_cast<GXMaterialProgram_802A6FDC*>(this)->BindParameters(packet);

    if (packet->displayList != 0)
        GXCallDisplayList(packet->displayList->list, packet->displayList->size);
    else if (packet->indexBuffer != 0)
        static_cast<GXMaterialProgram_802A6FDC*>(this)->DrawIndexed(packet);
    else
        static_cast<GXMaterialProgram_802A6FDC*>(this)->DrawDirect(packet);
}
