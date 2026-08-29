#include <revolution/gx.h>

#include "NL/gl/glMatrix.h"
#include "NL/gl/glView.h"
#include "NL/glx/GXMaterialProgram.h"
#include "NL/glx/glxGX.h"
#include "NL/glx/glxDisplayList.h"
#include "NL/glx/glxMatrix.h"
#include "NL/nlMath.h"

extern float lbl_80524370[3][4];

static nlMatrix4 sViewMatrix;
static unsigned long sLoadedMatrix;

template <>
void GXMaterialProgramImpl<GXMaterialProgram_8029AB0C>::Activate(GLView* view)
{
    static_cast<GXMaterialProgram_8029AB0C*>(this)->ConfigureVertexFormat(true);
    view->m_Interface->GetViewMatrix(sViewMatrix);
    GXLoadTexMtxImm(lbl_80524370, 64, GX_MTX3x4);
    GXSetTexCoordGen2(
        GX_TEXCOORD1, GX_TG_MTX3x4, GX_TG_NRM, 30, true, 64);
    sLoadedMatrix = -1;

    gxSetNumChans(0);
    gxSetNumTexGens(2);
    gxSetNumTevStages(3);
    gxSetTevOrder(0, 1, 1, 255);
    gxSetTevOrder(1, 0, 2, 255);
    gxSetTevOrder(2, 0, 0, 255);
    gxSetTevColourIn(0, 15, 12, 8, 15);
    gxSetTevColourIn(1, 15, 0, 8, 15);
    gxSetTevColourIn(2, 15, 12, 8, 0);
    gxSetTevAlphaIn(0, 7, 7, 7, 7);
    gxSetTevAlphaIn(1, 7, 7, 7, 7);
    gxSetTevAlphaIn(2, 7, 7, 7, 4);
}

template <>
void GXMaterialProgramImpl<GXMaterialProgram_8029AB0C>::Deactivate()
{
    gxSetTexCoordGen(1, 1, 5, 60);
}

template <>
void GXMaterialProgramImpl<GXMaterialProgram_8029AB0C>::Prepare(
    const glModelPacket* packet)
{
    fn_802CC978(this, packet, *(unsigned long*)packet->unknown20);
}

template <>
void GXMaterialProgramImpl<GXMaterialProgram_8029AB0C>::Draw(
    const glModelPacket* packet)
{
    static_cast<GXMaterialProgram_8029AB0C*>(this)->BindVertexArrays(packet);
    static_cast<GXMaterialProgram_8029AB0C*>(this)->BindParameters(packet);

    if (packet->matrix != sLoadedMatrix)
    {
        sLoadedMatrix = packet->matrix;
        nlMatrix4 model;
        nlMatrix4 modelview;
        float source[3][4];
        float inverse[3][4];
        glGetMatrix(packet->matrix, model);
        nlMultMatrices(modelview, model, sViewMatrix);
        glxCopyMatrix(source, modelview);
        PSMTXInvXpose(source, inverse);
        GXLoadTexMtxImm(inverse, 30, GX_MTX3x4);
    }

    if (packet->displayList != 0)
        GXCallDisplayList(packet->displayList->list, packet->displayList->size);
    else if (packet->indexBuffer != 0)
        static_cast<GXMaterialProgram_8029AB0C*>(this)->DrawIndexed(packet);
    else
        static_cast<GXMaterialProgram_8029AB0C*>(this)->DrawDirect(packet);
}
