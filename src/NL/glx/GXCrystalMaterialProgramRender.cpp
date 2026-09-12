#include <revolution/gx.h>
#include "NL/gl/glMaterialParameters.h"

#include "Game/TweakValue.h"
#include "NL/gl/glMatrix.h"
#include "NL/gl/glState.h"
#include "NL/gl/glView.h"
#include "NL/glx/GXMaterialCrystalTweaks.h"
#include "NL/glx/GXMaterialProgram.h"
#include "NL/glx/glxGX.h"
#include "NL/glx/glxDisplayList.h"
#include "NL/glx/glxMatrix.h"
#include "NL/nlMath.h"
#include "NL/nlMemory.h"

static bool glx_CompiledDraw = true;
static bool glx_AllowUncompiledDraws = true;
static bool sUseCrystalTexture1 = true;
static bool sUseCrystalTexture2 = true;
static bool sUseCrystalTexture0 = true;

static nlMatrix4 sViewMatrix;

static void ConfigureCrystalStages()
{
    gxSetNumTevStages(3);
    gxSetNumTexGens(3);
    gxSetNumChans(1);

    int texture0Colour = 15;
    if (sUseCrystalTexture1)
        texture0Colour = 10;
    int texture0Alpha = 7;
    if (sUseCrystalTexture1)
        texture0Alpha = 5;
    int texture1Colour = 15;
    if (sUseCrystalTexture2)
        texture1Colour = 14;
    int texture1Alpha = 7;
    if (sUseCrystalTexture2)
        texture1Alpha = 6;
    int texture2Colour = 15;
    if (sUseCrystalTexture0)
        texture2Colour = 14;
    int texture2Alpha = 7;
    if (sUseCrystalTexture0)
        texture2Alpha = 6;

    gxSetTevOrder(0, 1, 1, 4);
    gxSetTevColourIn(0, 15, 8, texture0Colour, 15);
    gxSetTevAlphaIn(0, 7, 4, texture0Alpha, 7);

    gxSetTexCoordGen(1, 1, 5, 60);
    gxSetTevOrder(1, 2, 2, 4);
    gxSetTevColourIn(1, 15, 8, texture1Colour, 0);
    gxSetTevAlphaIn(1, 7, 4, texture1Alpha, 0);

    gxSetTexCoordGen(2, 1, 1, 30);
    gxSetTevKColourSel(1, 12);
    gxSetTevKAlphaSel(1, 28);

    gxSetTevOrder(2, 0, 0, 4);
    gxSetTevColourIn(2, 15, texture2Colour, 8, 0);
    gxSetTevAlphaIn(2, 7, texture2Alpha, 4, 0);

    gxSetTexCoordGen(0, 1, 4, 60);
    gxSetTevKColourSel(2, 13);
    gxSetTevKAlphaSel(2, 29);
}

template <>
void GXMaterialProgramImpl<GXCrystalMaterialProgram>::Activate(GLView* view)
{
    static_cast<GXCrystalMaterialProgram*>(this)->ConfigureVertexFormat(true);
    ConfigureCrystalStages();
    view->m_Interface->GetViewMatrix(sViewMatrix);
}

template <>
void GXMaterialProgramImpl<GXCrystalMaterialProgram>::Deactivate()
{
    gxSetNumTevStages(1);
    gxSetNumTexGens(1);
    gxSetTexCoordGen(0, 1, 4, 60);
    gxSetTexCoordGen(1, 1, 5, 60);
    gxSetTexCoordGen(2, 1, 6, 60);
}

template <>
void GXMaterialProgramImpl<GXCrystalMaterialProgram>::Prepare(
    glModelPacket* packet)
{
    GXCrystalMaterialParameters* parameters = (GXCrystalMaterialParameters*)packet->materialParameters;
    glSetMaterialTextureAlphaState(this, packet, parameters->texture1.texture);
    unsigned int& rasterState = packet->rasterState;
    glSetRasterState(rasterState, GLS_Culling, 1);
    glSetRasterState(rasterState, GLS_AlphaTest, 0);
    glSetRasterState(rasterState, GLS_AlphaBlend, 1);
}

template <>
void GXMaterialProgramImpl<GXCrystalMaterialProgram>::Draw(
    const glModelPacket* packet)
{
    static_cast<GXCrystalMaterialProgram*>(this)->BindVertexArrays(packet);
    static_cast<GXCrystalMaterialProgram*>(this)->BindParameters(packet);

    unsigned char silhouette = (unsigned char)(sfSilhouetteLightingScale.value * 255.0f);
    unsigned char edge = (unsigned char)(sfEdgeTextureScale.value * 255.0f);
    GXColor silhouetteColour = { silhouette, silhouette, silhouette, silhouette };
    GXColor edgeColour = { edge, edge, edge, edge };
    GXSetTevKColor(GX_KCOLOR0, silhouetteColour);
    GXSetTevKColor(GX_KCOLOR1, edgeColour);

    GXCrystalMaterialParameters* parameters = (GXCrystalMaterialParameters*)packet->materialParameters;
    parameters->texture2.SetWrapS(1);

    nlMatrix4 model;
    nlMatrix4 modelview;
    float source[3][4];
    float inverse[3][4];
    float textureMatrix[3][4];
    glGetMatrix(packet->matrix, model);
    nlMultMatrices(modelview, model, sViewMatrix);
    glxCopyMatrix(source, modelview);
    PSMTXInvXpose(source, inverse);

    float offset = -inverse[2][3];
    textureMatrix[0][0] = -inverse[2][0];
    textureMatrix[1][0] = 0.0f;
    textureMatrix[0][1] = -inverse[2][1];
    textureMatrix[1][1] = 0.0f;
    textureMatrix[0][2] = -inverse[2][2];
    textureMatrix[1][2] = 0.0f;
    textureMatrix[1][3] = 0.0f;
    textureMatrix[0][3] = offset + 1.0f;
    GXLoadTexMtxImm(textureMatrix, 30, GX_MTX3x4);

    if (packet->displayList != 0 && glx_CompiledDraw)
    {
        GXCallDisplayList(
            packet->displayList->list, packet->displayList->size);
    }
    else if (glx_AllowUncompiledDraws)
    {
        if (packet->indexBuffer == 0)
            static_cast<GXCrystalMaterialProgram*>(this)->DrawDirect(packet);
        else
            static_cast<GXCrystalMaterialProgram*>(this)->DrawIndexed(packet);
    }
}

TweakValueFloat sfSilhouetteLightingScale(
    "sfSilhouetteLightingScale", "/Rendering/Materials/Crystal");
TweakValueFloat sfEdgeTextureScale(
    "sfEdgeTextureScale", gLastTweakCategory);
