#include <revolution/gx.h>

#include "Game/GameObjectLighting.h"
#include "NL/gl/glMatrix.h"
#include "NL/gl/glView.h"
#include "NL/glx/GXMaterialProgram.h"
#include "NL/glx/glxGX.h"
#include "NL/glx/glxDisplayList.h"
#include "NL/nlMath.h"
#include "unclassified/tu_801820FC.h"

struct GXMaterialProgramParameters_80299A90
{
    /* 0x00 */ UnidentifiedTextureState texture0;
    /* 0x08 */ const float (*matrices)[3][4];
    /* 0x0C */ unsigned long matricesSize;
}; // size: 0x10

extern bool lbl_806DF050;

static nlMatrix4 sViewMatrix;
static void* sUnidentifiedState;

template <>
void GXMaterialProgramImpl<GXMaterialProgram_80299A90>::Activate(GLView* view)
{
    static_cast<GXMaterialProgram_80299A90*>(this)->ConfigureVertexFormat(true);
    view->m_Interface->GetViewMatrix(sViewMatrix);
    sUnidentifiedState = fn_80182240(0, 1);
    fn_80182ED0((s32)sUnidentifiedState, view, 0);
    fn_801832F4(1, (s32)sUnidentifiedState, 0);
    GXSetVtxDesc(GX_VA_PNMTXIDX, GX_DIRECT);
    gxSetNumChans(1);
    gxSetNumTexGens(1);
    gxSetNumTevStages(1);
    gxSetTevOrder(0, 0, 0, 255);
    gxSetTevColourIn(0, 15, 10, 8, 15);
    gxSetTevAlphaIn(0, 7, 7, 7, 4);
}

template <>
void GXMaterialProgramImpl<GXMaterialProgram_80299A90>::Deactivate()
{
    fn_801832F4(0, (s32)sUnidentifiedState, 1);
    gxSetCurrentMtx(0, true);
}

template <>
void GXMaterialProgramImpl<GXMaterialProgram_80299A90>::Prepare(
    const glModelPacket* packet)
{
    fn_802CC978(this, packet, static_cast<const GXMaterialProgramParameters_80299A90*>(packet->unknown20)->texture0.texture);
}

template <>
void GXMaterialProgramImpl<GXMaterialProgram_80299A90>::Draw(
    const glModelPacket* packet)
{
    if (!lbl_806DF050)
        return;

    static_cast<GXMaterialProgram_80299A90*>(this)->BindVertexArrays(packet);
    GXSetVtxDesc(GX_VA_PNMTXIDX, GX_DIRECT);
    static_cast<GXMaterialProgram_80299A90*>(this)->BindParameters(packet);

    nlMatrix4 model;
    nlMatrix4 modelview;
    glGetMatrix(packet->matrix, model);
    nlMultMatrices(modelview, model, sViewMatrix);

    if (packet->unknown28 == 0)
    {
        const GXMaterialProgramParameters_80299A90* parameters = static_cast<const GXMaterialProgramParameters_80299A90*>(packet->unknown20);
        fn_8036D7EC(parameters->matrices,
            parameters->matricesSize / 48,
            &modelview,
            0);
    }
    else
    {
        fn_8036D774(&modelview);
    }

    GXCallDisplayList(packet->displayList->list, packet->displayList->size);
}
