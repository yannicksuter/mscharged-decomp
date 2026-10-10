#include <revolution/gx.h>
#include "NL/gl/glMaterialParameters.h"

#include "Game/TweakValue.h"
#include "Game/SharedStaticStorage.h"
#include "NL/gl/glMatrix.h"
#include "NL/gl/glState.h"
#include "NL/gl/glView.h"
#include "NL/glx/GXMaterialCrystalTweaks.h"
#include "NL/glx/GXCrystalMaterialProgram.h"
#include "NL/glx/glxGX.h"
#include "NL/glx/glxDisplayList.h"
#include "NL/glx/glxMatrix.h"
#include "NL/nlMath.h"
#include "NL/nlMemory.h"

static bool sUseCrystalDisplayLists = true;
static bool sAllowCrystalUncompiledDraws = true;
static bool sUseCrystalDiffuseTexture = true;
static bool sUseCrystalRampTexture = true;
static bool sUseCrystalDetailTexture = true;

TweakValueFloat sfSilhouetteLightingScale(
    "sfSilhouetteLightingScale", "/Rendering/Materials/Crystal");
TweakValueFloat sfEdgeTextureScale(
    "sfEdgeTextureScale", gLastTweakCategory);

static nlMatrix4 sCrystalViewMatrix;

static void ConfigureCrystalStages()
{
    gxSetNumTevStages(3);
    gxSetNumTexGens(3);
    gxSetNumChans(1);

    GXTevColorArg diffuseColourInput = GX_CC_ZERO;
    if (sUseCrystalDiffuseTexture)
        diffuseColourInput = GX_CC_RASC;
    GXTevAlphaArg diffuseAlphaInput = GX_CA_ZERO;
    if (sUseCrystalDiffuseTexture)
        diffuseAlphaInput = GX_CA_RASA;
    GXTevColorArg rampColourInput = GX_CC_ZERO;
    if (sUseCrystalRampTexture)
        rampColourInput = GX_CC_KONST;
    GXTevAlphaArg rampAlphaInput = GX_CA_ZERO;
    if (sUseCrystalRampTexture)
        rampAlphaInput = GX_CA_KONST;
    GXTevColorArg detailColourInput = GX_CC_ZERO;
    if (sUseCrystalDetailTexture)
        detailColourInput = GX_CC_KONST;
    GXTevAlphaArg detailAlphaInput = sUseCrystalDetailTexture ? GX_CA_KONST : GX_CA_ZERO;

    gxSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD1, GX_TEXMAP1, GX_COLOR0A0);
    gxSetTevColourIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_TEXC, diffuseColourInput, GX_CC_ZERO);
    gxSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_TEXA, diffuseAlphaInput, GX_CA_ZERO);

    gxSetTexCoordGen(GX_TEXCOORD1, GX_TG_MTX2x4, GX_TG_TEX1, GX_IDENTITY);
    gxSetTevOrder(GX_TEVSTAGE1, GX_TEXCOORD2, GX_TEXMAP2, GX_COLOR0A0);
    gxSetTevColourIn(GX_TEVSTAGE1, GX_CC_ZERO, GX_CC_TEXC, rampColourInput, GX_CC_CPREV);
    gxSetTevAlphaIn(GX_TEVSTAGE1, GX_CA_ZERO, GX_CA_TEXA, rampAlphaInput, GX_CA_APREV);

    gxSetTexCoordGen(GX_TEXCOORD2, GX_TG_MTX2x4, GX_TG_NRM, GX_TEXMTX0);
    gxSetTevKColourSel(GX_TEVSTAGE1, GX_TEV_KCSEL_K0);
    gxSetTevKAlphaSel(GX_TEVSTAGE1, GX_TEV_KASEL_K0_A);

    gxSetTevOrder(GX_TEVSTAGE2, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
    gxSetTevColourIn(GX_TEVSTAGE2, GX_CC_ZERO, detailColourInput, GX_CC_TEXC, GX_CC_CPREV);
    gxSetTevAlphaIn(GX_TEVSTAGE2, GX_CA_ZERO, detailAlphaInput, GX_CA_TEXA, GX_CA_APREV);

    gxSetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY);
    gxSetTevKColourSel(GX_TEVSTAGE2, GX_TEV_KCSEL_K1);
    gxSetTevKAlphaSel(GX_TEVSTAGE2, GX_TEV_KASEL_K1_A);
}

template <>
void GXMaterialProgramImpl<GXCrystalMaterialProgram>::Activate(GLView* view)
{
    static_cast<GXCrystalMaterialProgram*>(this)->ConfigureVertexFormat(true);
    ConfigureCrystalStages();
    view->m_Interface->GetViewMatrix(sCrystalViewMatrix);
}

template <>
void GXMaterialProgramImpl<GXCrystalMaterialProgram>::Deactivate()
{
    gxSetNumTevStages(1);
    gxSetNumTexGens(1);
    gxSetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY);
    gxSetTexCoordGen(GX_TEXCOORD1, GX_TG_MTX2x4, GX_TG_TEX1, GX_IDENTITY);
    gxSetTexCoordGen(GX_TEXCOORD2, GX_TG_MTX2x4, GX_TG_TEX2, GX_IDENTITY);
}

template <>
void GXMaterialProgramImpl<GXCrystalMaterialProgram>::Prepare(
    glModelPacket* packet)
{
    GXCrystalMaterialParameters* parameters = static_cast<GXCrystalMaterialParameters*>(packet->materialParameters);
    glSetMaterialTextureAlphaState(this, packet, parameters->diffuseTexture.texture);
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

    GXCrystalMaterialParameters* parameters = static_cast<GXCrystalMaterialParameters*>(packet->materialParameters);
    parameters->rampTexture.SetWrapS(true);

    nlMatrix4 model;
    nlMatrix4 modelview;
    Mtx modelViewTransform;
    Mtx normalMatrix;
    Mtx rampMatrix;
    glGetMatrix(packet->matrix, model);
    nlMultMatrices(modelview, model, sCrystalViewMatrix);
    glxCopyMatrix(modelViewTransform, modelview);
    PSMTXInvXpose(modelViewTransform, normalMatrix);

    for (int column = 0; column < 4; ++column)
    {
        rampMatrix[0][column] = -normalMatrix[2][column];
        rampMatrix[1][column] = 0.0f;
    }
    rampMatrix[0][3] += 1.0f;
    GXLoadTexMtxImm(rampMatrix, GX_TEXMTX0, GX_MTX3x4);

    if (packet->displayList == 0 || !sUseCrystalDisplayLists)
    {
        if (sAllowCrystalUncompiledDraws)
        {
            if (packet->indexBuffer == 0)
                static_cast<GXCrystalMaterialProgram*>(this)->DrawDirect(packet);
            else
                static_cast<GXCrystalMaterialProgram*>(this)->DrawIndexed(packet);
        }
    }
    else if (sUseCrystalDisplayLists)
    {
        GXCallDisplayList(packet->displayList->list, packet->displayList->size);
    }
}
