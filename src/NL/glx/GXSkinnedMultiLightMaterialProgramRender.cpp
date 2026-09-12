#include <revolution/gx.h>
#include "NL/gl/glMaterialParameters.h"
#include <revolution/mtx.h>

#include "NL/gl/gl.h"
#include "NL/gl/glMatrix.h"
#include "NL/gl/glView.h"
#include "NL/glx/GXSkinnedMultiLightMaterialProgram.h"
#include "NL/glx/glxLight.h"
#include "NL/glx/glxSkinMatrix.h"
#include "NL/glx/glxGX.h"
#include "NL/glx/glxDisplayList.h"
#include "NL/glx/glxMatrix.h"
#include "NL/nlMath.h"
#include "NL/platvmath.h"
#include "Game/UnidentifiedStaticStorage.h"

static nlMatrix4 sSkinnedMultiLightViewMatrix;
static int sSkinnedMultiLightAnimationFrames = 5;

template <>
void GXMaterialProgramImpl<GXSkinnedMultiLightMaterialProgram>::Activate(GLView* view)
{
    gxSetNumChans(1);
    gxSetNumTevStages(1);
    gxSetNumTexGens(1);
    static_cast<GXSkinnedMultiLightMaterialProgram*>(this)->ConfigureVertexFormat(true);
    view->m_Interface->GetViewMatrix(sSkinnedMultiLightViewMatrix);
}

template <>
void GXMaterialProgramImpl<GXSkinnedMultiLightMaterialProgram>::Deactivate()
{
    gxSetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY);
    GXSetChanCtrl(GX_COLOR1, false, GX_SRC_REG, GX_SRC_VTX, (GXLightID)(GX_LIGHT4 | GX_LIGHT5 | GX_LIGHT6 | GX_LIGHT7), GX_DF_NONE, GX_AF_NONE);
}

template <>
void GXMaterialProgramImpl<GXSkinnedMultiLightMaterialProgram>::Prepare(
    glModelPacket* packet)
{
    glSetMaterialTextureAlphaState(this, packet, static_cast<const GXSkinnedMultiLightParameters*>(packet->materialParameters)->diffuseTexture.texture);
}

extern "C" void glxConfigureSkinnedMultiLightBlend(float blendAmount)
{
    if (blendAmount >= 0.999f)
    {
        gxSetNumTexGens(2);
        gxSetNumTevStages(2);
        GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD1, GX_TEXMAP1, GX_COLOR0A0);
        GXSetTevOrder(GX_TEVSTAGE1, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
        gxSetTexCoordGen(GX_TEXCOORD1, GX_TG_SRTG, GX_TG_COLOR0, GX_IDENTITY);
        gxSetTevColourOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_2, true, GX_TEVPREV);
        GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ONE, GX_CC_TEXC, GX_CC_ZERO);
        GXSetTevColorIn(GX_TEVSTAGE1, GX_CC_ZERO, GX_CC_CPREV, GX_CC_TEXC, GX_CC_ZERO);
        gxSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_KONST, GX_CA_KONST, GX_CA_ZERO);
        gxSetTevAlphaIn(GX_TEVSTAGE1, GX_CA_ZERO, GX_CA_KONST, GX_CA_TEXA, GX_CA_ZERO);
    }
    else
    {
        gxSetNumTexGens(3);
        gxSetNumTevStages(4);
        GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD2, GX_TEXMAP2, GX_COLOR0A0);
        GXSetTevOrder(GX_TEVSTAGE1, GX_TEXCOORD1, GX_TEXMAP1, GX_COLOR_NULL);
        GXSetTevOrder(GX_TEVSTAGE2, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
        GXSetTevOrder(GX_TEVSTAGE3, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR_NULL);
        gxSetTexCoordGen(GX_TEXCOORD2, GX_TG_SRTG, GX_TG_COLOR0, GX_IDENTITY);
        gxSetTevColourOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, true, GX_TEVREG0);
        gxSetTevColourOp(GX_TEVSTAGE3, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_2, true, GX_TEVPREV);
        GXSetTevKColorSel(GX_TEVSTAGE1, GX_TEV_KCSEL_K0);
        GXSetTevKColorSel(GX_TEVSTAGE2, GX_TEV_KCSEL_K0);

        GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ONE, GX_CC_TEXC, GX_CC_ZERO);
        GXSetTevColorIn(GX_TEVSTAGE1, GX_CC_ZERO, GX_CC_ONE, GX_CC_TEXC, GX_CC_KONST);
        GXSetTevColorIn(GX_TEVSTAGE2, GX_CC_ZERO, GX_CC_TEXC, GX_CC_CPREV, GX_CC_ZERO);
        GXSetTevColorIn(GX_TEVSTAGE3, GX_CC_ZERO, GX_CC_CPREV, GX_CC_C0, GX_CC_ZERO);

        gxSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO);
        gxSetTevAlphaIn(GX_TEVSTAGE1, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO);
        gxSetTevAlphaIn(GX_TEVSTAGE2, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_TEXA);
        gxSetTevAlphaIn(GX_TEVSTAGE3, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_APREV);
    }
}

static inline void LoadMaterialDiffuseLight(unsigned int index,
    const GXSkinnedMultiLightParameters* parameters,
    const nlMatrix4& viewMatrix)
{
    const nlFloatColour* colour = &parameters->lightColours[index];
    const GXSkinnedMultiLightParameters::PointLight* point = &parameters->pointLights[index];
    const nlVector3* direction = &parameters->lightDirections[index];
    nlVector3 transformed;
    if (parameters->useDirectionalLights != 0)
    {
        nlMultDirVectorMatrix(transformed, *direction, viewMatrix);
        glx_LoadDirectionalLight(index, &transformed, colour);
    }
    else
    {
        nlMultPosVectorMatrix(transformed, point->position, viewMatrix);
        glx_LoadPointLight(index, &transformed, colour, parameters->pointLights[index].distance);
    }
}

static inline void LoadMaterialSpecularLight(unsigned int index,
    const GXSkinnedMultiLightParameters* parameters,
    const nlMatrix4& viewMatrix)
{
    float exponent = parameters->specularLights[index].exponent;
    nlVector3 transformed;
    nlVector3 direction;
    nlFloatColour colour;
    nlVec3Set(direction, parameters->specularLights[index].direction.x,
        parameters->specularLights[index].direction.y,
        parameters->specularLights[index].direction.z);
    nlFloatColourSet(colour, parameters->lightColours[index + 4].c[0],
        parameters->lightColours[index + 4].c[1],
        parameters->lightColours[index + 4].c[2],
        parameters->lightColours[index + 4].c[3]);
    nlMultDirVectorMatrix(transformed, direction, viewMatrix);
    glx_LoadSpecular(index + 4, &transformed, &colour, exponent);
}

template <>
void GXMaterialProgramImpl<GXSkinnedMultiLightMaterialProgram>::Draw(
    const glModelPacket* packet)
{
    static_cast<GXSkinnedMultiLightMaterialProgram*>(this)->BindVertexArrays(packet);
    GXSetVtxDesc(GX_VA_PNMTXIDX, GX_DIRECT);

    nlMatrix4 model;
    nlMatrix4 modelview;
    glGetMatrix(packet->matrix, model);
    nlMultMatrices(modelview, model, sSkinnedMultiLightViewMatrix);

    int diffuseLightCount = static_cast<const GXSkinnedMultiLightParameters*>(packet->materialParameters)->diffuseLightCount;
    int specularLightCount = static_cast<const GXSkinnedMultiLightParameters*>(packet->materialParameters)->specularLightCount;

    glxConfigureSkinnedMultiLightBlend(static_cast<const GXSkinnedMultiLightParameters*>(packet->materialParameters)->blendAmount);
    nlFloatColour floatColour = { { static_cast<const GXSkinnedMultiLightParameters*>(packet->materialParameters)->blendAmount,
        static_cast<const GXSkinnedMultiLightParameters*>(packet->materialParameters)->blendAmount,
        static_cast<const GXSkinnedMultiLightParameters*>(packet->materialParameters)->blendAmount,
        static_cast<const GXSkinnedMultiLightParameters*>(packet->materialParameters)->blendAmount } };
    nlColour colour;
    ConvertColour(colour, floatColour);
    GXColor gxColour = *(GXColor*)&colour;
    GXSetTevKColor(GX_KCOLOR0, gxColour);
    GXSetTevKColor(GX_KCOLOR1, gxColour);
    GXSetTevKColor(GX_KCOLOR2, gxColour);

    if (static_cast<const GXSkinnedMultiLightParameters*>(packet->materialParameters)->animateNormalTexCoords != 0)
    {
        Mtx textureMatrix;
        Mtx scaleMatrix;
        Mtx translationMatrix;
        glxCopyMatrix(textureMatrix, modelview);
        gxSetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX3x4, GX_TG_NRM, GX_TEXMTX9);
        PSMTXScale(scaleMatrix, 0.5f, -0.5f, 0.0f);
        PSMTXTrans(translationMatrix, 0.5f, 0.5f, 1.0f);
        PSMTXConcat(scaleMatrix, textureMatrix, textureMatrix);
        PSMTXConcat(translationMatrix, textureMatrix, textureMatrix);

        float translationScale = 1.0f / (float)sSkinnedMultiLightAnimationFrames;
        unsigned long frame = glGetCurrentFrame();
        unsigned long frameDiv = frame / sSkinnedMultiLightAnimationFrames;
        unsigned long frameMod = frame - frameDiv * sSkinnedMultiLightAnimationFrames;
        float translation = translationScale * frameMod;
        translation = 2.0f * translation - 1.0f;
        textureMatrix[0][2] = translation;
        textureMatrix[1][2] = translation;
        GXLoadTexMtxImm(textureMatrix, GX_TEXMTX9, GX_MTX3x4);
    }
    else
    {
        gxSetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY);
    }

    if (diffuseLightCount > 0)
        LoadMaterialDiffuseLight(0, static_cast<const GXSkinnedMultiLightParameters*>(packet->materialParameters), sSkinnedMultiLightViewMatrix);
    if (diffuseLightCount > 1)
        LoadMaterialDiffuseLight(1, static_cast<const GXSkinnedMultiLightParameters*>(packet->materialParameters), sSkinnedMultiLightViewMatrix);
    if (diffuseLightCount > 2)
        LoadMaterialDiffuseLight(2, static_cast<const GXSkinnedMultiLightParameters*>(packet->materialParameters), sSkinnedMultiLightViewMatrix);
    if (diffuseLightCount > 3)
        LoadMaterialDiffuseLight(3, static_cast<const GXSkinnedMultiLightParameters*>(packet->materialParameters), sSkinnedMultiLightViewMatrix);

    if (specularLightCount > 0)
        LoadMaterialSpecularLight(0, static_cast<const GXSkinnedMultiLightParameters*>(packet->materialParameters), sSkinnedMultiLightViewMatrix);
    if (specularLightCount > 1)
        LoadMaterialSpecularLight(1, static_cast<const GXSkinnedMultiLightParameters*>(packet->materialParameters), sSkinnedMultiLightViewMatrix);
    if (specularLightCount > 2)
        LoadMaterialSpecularLight(2, static_cast<const GXSkinnedMultiLightParameters*>(packet->materialParameters), sSkinnedMultiLightViewMatrix);
    if (specularLightCount > 3)
        LoadMaterialSpecularLight(3, static_cast<const GXSkinnedMultiLightParameters*>(packet->materialParameters), sSkinnedMultiLightViewMatrix);

    static_cast<GXSkinnedMultiLightParameters*>(packet->materialParameters)->rampTexture.SetWrapS(true);
    static_cast<GXSkinnedMultiLightParameters*>(packet->materialParameters)->rampTexture.SetWrapT(true);
    static_cast<GXSkinnedMultiLightMaterialProgram*>(this)->BindParameters(packet);

    if (specularLightCount > 0)
    {
        gxSetNumChans(2);
        unsigned int lightMask = 0;
        if (specularLightCount >= 1)
            lightMask |= GX_LIGHT4;
        if (specularLightCount >= 2)
            lightMask |= GX_LIGHT5;
        if (specularLightCount >= 3)
            lightMask |= GX_LIGHT6;
        if (specularLightCount >= 4)
            lightMask |= GX_LIGHT7;
        GXSetChanCtrl(GX_COLOR1, true, GX_SRC_REG, GX_SRC_REG, (GXLightID)lightMask, GX_DF_NONE, GX_AF_SPEC);
    }
    else
    {
        gxSetNumChans(1);
        GXSetChanCtrl(GX_COLOR1, false, GX_SRC_REG, GX_SRC_VTX, (GXLightID)(GX_LIGHT4 | GX_LIGHT5 | GX_LIGHT6 | GX_LIGHT7), GX_DF_NONE, GX_AF_NONE);
    }

    if (diffuseLightCount > 0)
    {
        unsigned int lightMask = 0;
        if (diffuseLightCount >= 1)
            lightMask |= GX_LIGHT0;
        if (diffuseLightCount >= 2)
            lightMask |= GX_LIGHT1;
        if (diffuseLightCount >= 3)
            lightMask |= GX_LIGHT2;
        if (diffuseLightCount >= 4)
            lightMask |= GX_LIGHT3;
        GXSetChanCtrl(GX_COLOR0, true, GX_SRC_REG, GX_SRC_REG, (GXLightID)lightMask, GX_DF_CLAMP, GX_AF_NONE);
    }
    else
    {
        GXSetChanCtrl(GX_COLOR0, false, GX_SRC_REG, GX_SRC_VTX, (GXLightID)(GX_LIGHT0 | GX_LIGHT1 | GX_LIGHT2 | GX_LIGHT3), GX_DF_NONE, GX_AF_NONE);
    }

    if (packet->skinnedVertices == 0)
    {
        glx_LoadSkinMatrices(static_cast<const GXSkinnedMultiLightParameters*>(packet->materialParameters)->skinMatrices, static_cast<const GXSkinnedMultiLightParameters*>(packet->materialParameters)->skinMatricesSize / 48, &modelview, 0);
    }
    else
    {
        glx_LoadDefaultSkinMatrices(&modelview);
    }

    GXCallDisplayList(packet->displayList->list, packet->displayList->size);
    gxSetCurrentMtx(0, true);
}
