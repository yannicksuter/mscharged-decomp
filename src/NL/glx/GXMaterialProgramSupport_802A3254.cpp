#include <revolution/gx.h>

#include "NL/glx/GXMaterialProgram.h"
#include "NL/glx/glxGX.h"
#include "NL/glx/glxDisplayList.h"

template <>
void GXMaterialProgramImpl<GXMaterialProgram_802A6848>::Activate(
    GLView*)
{
    gxSetNumChans(1);
    gxSetNumTexGens(0);
    gxSetNumTevStages(1);
    gxSetTevOrder(0, 255, 255, 4);
    GXSetTevKColorSel(GX_TEVSTAGE0, GX_TEV_KCSEL_K0);
    GXSetTevKAlphaSel(GX_TEVSTAGE0, GX_TEV_KASEL_K0_A);
    gxSetTevColourIn(0, 15, 12, 14, 15);
    gxSetTevAlphaIn(0, 7, 6, 6, 7);
    static_cast<GXMaterialProgram_802A6848*>(this)->ConfigureVertexFormat(true);
}

template <>
void GXMaterialProgramImpl<GXMaterialProgram_802A6848>::Deactivate()
{
}

template <>
void GXMaterialProgramImpl<GXMaterialProgram_802A6848>::Prepare(
    const glModelPacket*)
{
}

template <>
void GXMaterialProgramImpl<GXMaterialProgram_802A6848>::Draw(
    const glModelPacket* packet)
{
    GXColor colour = { 255, 0, 0, 255 };
    GXSetTevKColor(GX_KCOLOR0, colour);

    static_cast<GXMaterialProgram_802A6848*>(this)->BindVertexArrays(packet);
    static_cast<GXMaterialProgram_802A6848*>(this)->BindParameters(packet);

    if (packet->displayList != 0)
        GXCallDisplayList(packet->displayList->list, packet->displayList->size);
    else if (packet->indexBuffer != 0)
        static_cast<GXMaterialProgram_802A6848*>(this)->DrawIndexed(packet);
    else
        static_cast<GXMaterialProgram_802A6848*>(this)->DrawDirect(packet);
}
