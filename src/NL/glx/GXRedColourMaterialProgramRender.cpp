#include <revolution/gx.h>

#include "NL/glx/GXMaterialProgram.h"
#include "NL/glx/glxGX.h"
#include "NL/glx/glxDisplayList.h"
#include "Game/UnidentifiedStaticStorage.h"

template <>
void GXMaterialProgramImpl<GXRedColourMaterialProgram>::Activate(
    GLView*)
{
    gxSetNumChans(1);
    gxSetNumTexGens(0);
    gxSetNumTevStages(1);
    gxSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR0A0);
    GXSetTevKColorSel(GX_TEVSTAGE0, GX_TEV_KCSEL_K0);
    GXSetTevKAlphaSel(GX_TEVSTAGE0, GX_TEV_KASEL_K0_A);
    gxSetTevColourIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ONE, GX_CC_KONST, GX_CC_ZERO);
    gxSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_KONST, GX_CA_KONST, GX_CA_ZERO);
    static_cast<GXRedColourMaterialProgram*>(this)->ConfigureVertexFormat(true);
}

template <>
void GXMaterialProgramImpl<GXRedColourMaterialProgram>::Deactivate()
{
}

template <>
void GXMaterialProgramImpl<GXRedColourMaterialProgram>::Prepare(
    glModelPacket*)
{
}

template <>
void GXMaterialProgramImpl<GXRedColourMaterialProgram>::Draw(
    const glModelPacket* packet)
{
    GXColor colour = { 255, 0, 0, 255 };
    GXSetTevKColor(GX_KCOLOR0, colour);

    static_cast<GXRedColourMaterialProgram*>(this)->BindVertexArrays(packet);
    static_cast<GXRedColourMaterialProgram*>(this)->BindParameters(packet);

    if (packet->displayList != 0)
        GXCallDisplayList(packet->displayList->list, packet->displayList->size);
    else if (packet->indexBuffer != 0)
        static_cast<GXRedColourMaterialProgram*>(this)->DrawIndexed(packet);
    else
        static_cast<GXRedColourMaterialProgram*>(this)->DrawDirect(packet);
}
