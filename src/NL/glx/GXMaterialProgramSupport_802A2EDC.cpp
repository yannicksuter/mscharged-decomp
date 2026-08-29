#include <revolution/gx.h>

#include "NL/gl/glMatrix.h"
#include "NL/gl/glView.h"
#include "NL/glx/GXMaterialProgram.h"
#include "NL/glx/glxGX.h"
#include "NL/glx/glxDisplayList.h"
#include "NL/platvmath.h"

struct GXMaterialProgramParameters_802A61C8
{
    /* 0x00 */ unsigned long texture;
    /* 0x04 */ unsigned long padding;
    /* 0x08 */ void* matrices;
    /* 0x0C */ unsigned long matricesSize;
}; // size: 0x10

static nlMatrix4 mview;

extern "C"
{
    void fn_8036D774(const nlMatrix4* matrix);
    void fn_8036D7EC(
        void* matrices, unsigned long count, const nlMatrix4* matrix,
        bool unknown);
}

template <>
void GXMaterialProgramImpl<GXMaterialProgram_802A61C8>::Activate(GLView* view)
{
    gxSetTevColourOp(0, 0, 0, 0, true, 0);
    gxSetTevAlphaOp(0, 0, 0, 0, true, 0);
    gxSetTexCoordGen(0, 1, 4, 60);
    gxSetNumTexGens(1);
    gxSetNumChans(0);
    gxSetNumTevStages(1);
    gxSetTevOrder(0, 0, 0, 255);
    gxSetTevColourIn(0, 15, 12, 8, 15);
    gxSetTevAlphaIn(0, 7, 6, 4, 7);
    static_cast<GXMaterialProgram_802A61C8*>(this)->ConfigureVertexFormat(true);
    view->m_Interface->GetViewMatrix(mview);
}

template <>
void GXMaterialProgramImpl<GXMaterialProgram_802A61C8>::Deactivate()
{
    gxSetCurrentMtx(0, true);
}

template <>
void GXMaterialProgramImpl<GXMaterialProgram_802A61C8>::Prepare(
    const glModelPacket* packet)
{
    fn_802CC978(this, packet, *(unsigned long*)packet->unknown20);
}

template <>
void GXMaterialProgramImpl<GXMaterialProgram_802A61C8>::Draw(
    const glModelPacket* packet)
{
    nlMatrix4 model;
    nlMatrix4 modelview;

    static_cast<GXMaterialProgram_802A61C8*>(this)->BindVertexArrays(packet);
    static_cast<GXMaterialProgram_802A61C8*>(this)->BindParameters(packet);
    GXSetVtxDesc(GX_VA_PNMTXIDX, GX_DIRECT);
    glGetMatrix(packet->matrix, model);
    nlMultMatrices(modelview, model, mview);

    if (packet->unknown28 == 0)
    {
        GXMaterialProgramParameters_802A61C8* parameters = (GXMaterialProgramParameters_802A61C8*)packet->unknown20;
        fn_8036D7EC(parameters->matrices, parameters->matricesSize / 48, &modelview, false);
    }
    else
    {
        fn_8036D774(&modelview);
    }

    GXCallDisplayList(packet->displayList->list, packet->displayList->size);
    gxSetCurrentMtx(0, true);
}
