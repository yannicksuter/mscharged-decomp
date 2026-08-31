#include <revolution/gx.h>
#include <revolution/mtx.h>

#include "NL/gl/gl.h"
#include "NL/gl/glMatrix.h"
#include "NL/gl/glView.h"
#include "NL/glx/GXMaterialProgram.h"
#include "NL/glx/glxGX.h"
#include "NL/glx/glxDisplayList.h"
#include "NL/glx/glxMatrix.h"
#include "NL/nlMath.h"
#include "NL/platvmath.h"

extern "C"
{
    void fn_8036A800(unsigned int index, const void* first, const void* second);
    void fn_8036A9C4(
        unsigned int index, const void* first, const void* second, float value);
    void fn_8036AB40(
        unsigned int index, const void* first, const void* second, float value);
    void fn_8036D774(const nlMatrix4* matrix);
    void fn_8036D7EC(
        const void* matrices, unsigned long count, const nlMatrix4*, int);
}

struct GXMaterialProgramParameters_8029BA04
{
    /* 0x000 */ UnidentifiedTextureState texture0;
    /* 0x008 */ UnidentifiedTextureState texture1;
    /* 0x010 */ UnidentifiedTextureState texture2;
    /* 0x018 */ void* matrices;
    /* 0x01C */ unsigned long matricesSize;
    /* 0x020 */ float value32;
    /* 0x024 */ int value36;
    /* 0x028 */ nlVector3 values40[4];
    struct Value_80290914
    {
        nlVector3 vector;
        float value;
    };
    /* 0x058 */ Value_80290914 values88[4];
    /* 0x098 */ Value_80290914 values152[4];
    /* 0x0D8 */ nlVector4 values216[4];
    /* 0x118 */ nlVector4 values280[4];
    /* 0x158 */ int value344;
    /* 0x15C */ unsigned char padding348[12];
    /* 0x168 */ int count360;
    /* 0x16C */ int count364;
}; // size: 0x170

static nlMatrix4 lbl_8057B0D8;
static int lbl_806DF060 = 5;

template <>
void GXMaterialProgramImpl<GXMaterialProgram_8029BA04>::Activate(GLView* view)
{
    gxSetNumChans(1);
    gxSetNumTevStages(1);
    gxSetNumTexGens(1);
    static_cast<GXMaterialProgram_8029BA04*>(this)->ConfigureVertexFormat(true);
    view->m_Interface->GetViewMatrix(lbl_8057B0D8);
}

template <>
void GXMaterialProgramImpl<GXMaterialProgram_8029BA04>::Deactivate()
{
    gxSetTexCoordGen(0, 1, 4, 60);
    GXSetChanCtrl(GX_COLOR1, false, GX_SRC_REG, GX_SRC_VTX, (GXLightID)(GX_LIGHT4 | GX_LIGHT5 | GX_LIGHT6 | GX_LIGHT7), GX_DF_NONE, GX_AF_NONE);
}

template <>
void GXMaterialProgramImpl<GXMaterialProgram_8029BA04>::Prepare(
    const glModelPacket* packet)
{
    fn_802CC978(this, packet, *(unsigned long*)packet->unknown20);
}

extern "C" void fn_802909E4(float value)
{
    if (value >= 0.999f)
    {
        gxSetNumTexGens(2);
        gxSetNumTevStages(2);
        GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD1, GX_TEXMAP1, GX_COLOR0A0);
        GXSetTevOrder(GX_TEVSTAGE1, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
        gxSetTexCoordGen(1, 10, 19, 60);
        gxSetTevColourOp(1, 0, 0, 1, true, 0);
        GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ONE, GX_CC_TEXC, GX_CC_ZERO);
        GXSetTevColorIn(GX_TEVSTAGE1, GX_CC_ZERO, GX_CC_CPREV, GX_CC_TEXC, GX_CC_ZERO);
        gxSetTevAlphaIn(0, 7, 6, 6, 7);
        gxSetTevAlphaIn(1, 7, 6, 4, 7);
    }
    else
    {
        gxSetNumTexGens(3);
        gxSetNumTevStages(4);
        GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD2, GX_TEXMAP2, GX_COLOR0A0);
        GXSetTevOrder(GX_TEVSTAGE1, GX_TEXCOORD1, GX_TEXMAP1, GX_COLOR_NULL);
        GXSetTevOrder(GX_TEVSTAGE2, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
        GXSetTevOrder(GX_TEVSTAGE3, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR_NULL);
        gxSetTexCoordGen(2, 10, 19, 60);
        gxSetTevColourOp(0, 0, 0, 0, true, 1);
        gxSetTevColourOp(3, 0, 0, 1, true, 0);
        GXSetTevKColorSel(GX_TEVSTAGE1, GX_TEV_KCSEL_K0);
        GXSetTevKColorSel(GX_TEVSTAGE2, GX_TEV_KCSEL_K0);

        GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ONE, GX_CC_TEXC, GX_CC_ZERO);
        GXSetTevColorIn(GX_TEVSTAGE1, GX_CC_ZERO, GX_CC_ONE, GX_CC_TEXC, GX_CC_KONST);
        GXSetTevColorIn(GX_TEVSTAGE2, GX_CC_ZERO, GX_CC_TEXC, GX_CC_CPREV, GX_CC_ZERO);
        GXSetTevColorIn(GX_TEVSTAGE3, GX_CC_ZERO, GX_CC_CPREV, GX_CC_C0, GX_CC_ZERO);

        gxSetTevAlphaIn(0, 7, 7, 7, 7);
        gxSetTevAlphaIn(1, 7, 7, 7, 7);
        gxSetTevAlphaIn(2, 7, 7, 7, 4);
        gxSetTevAlphaIn(3, 7, 7, 7, 0);
    }
}

struct FloatColour_80290914
{
    float c[4];
};

static inline GXColor ConvertColour_80290914(
    const FloatColour_80290914& source)
{
    GXColor colour;
    colour.r = (unsigned char)(source.c[0] * 255.0f);
    colour.g = (unsigned char)(source.c[1] * 255.0f);
    colour.b = (unsigned char)(source.c[2] * 255.0f);
    colour.a = (unsigned char)(source.c[3] * 255.0f);
    return colour;
}

static inline void LoadValue_80290914(unsigned int index,
    const GXMaterialProgramParameters_8029BA04* parameters,
    const nlMatrix4& viewMatrix)
{
    nlVector3 transformed;
    if (parameters->value344 != 0)
    {
        nlMultDirVectorMatrix(
            transformed, parameters->values40[index], viewMatrix);
        fn_8036A800(index, &transformed, &parameters->values216[index]);
    }
    else
    {
        nlMultPosVectorMatrix(
            transformed, parameters->values88[index].vector, viewMatrix);
        fn_8036A9C4(index, &transformed, &parameters->values216[index], parameters->values88[index].value);
    }
}

static inline void LoadValue2_80290914(unsigned int index,
    const GXMaterialProgramParameters_8029BA04* parameters,
    const nlMatrix4& viewMatrix)
{
    nlVector3 source = parameters->values152[index].vector;
    nlVector4 value = parameters->values280[index];
    nlVector3 transformed;
    nlMultDirVectorMatrix(transformed, source, viewMatrix);
    fn_8036AB40(index + 4, &transformed, &value, parameters->values152[index].value);
}

template <>
void GXMaterialProgramImpl<GXMaterialProgram_8029BA04>::Draw(
    const glModelPacket* packet)
{
    static_cast<GXMaterialProgram_8029BA04*>(this)->BindVertexArrays(packet);
    GXSetVtxDesc(GX_VA_PNMTXIDX, GX_DIRECT);

    nlMatrix4 model;
    nlMatrix4 modelview;
    glGetMatrix(packet->matrix, model);
    nlMultMatrices(modelview, model, lbl_8057B0D8);

    GXMaterialProgramParameters_8029BA04* parameters = (GXMaterialProgramParameters_8029BA04*)packet->unknown20;
    int count360 = parameters->count360;
    int count364 = parameters->count364;

    fn_802909E4(parameters->value32);
    FloatColour_80290914 floatColour = { { parameters->value32,
        parameters->value32,
        parameters->value32,
        parameters->value32 } };
    GXColor colour = ConvertColour_80290914(floatColour);
    GXSetTevKColor(GX_KCOLOR0, colour);
    GXSetTevKColor(GX_KCOLOR1, colour);
    GXSetTevKColor(GX_KCOLOR2, colour);

    if (parameters->value36 != 0)
    {
        Mtx textureMatrix;
        Mtx scaleMatrix;
        Mtx translationMatrix;
        glxCopyMatrix(textureMatrix, modelview);
        gxSetTexCoordGen(0, 0, 1, 57);
        PSMTXScale(scaleMatrix, 0.5f, -0.5f, 0.0f);
        PSMTXTrans(translationMatrix, 0.5f, 0.5f, 1.0f);
        PSMTXConcat(scaleMatrix, textureMatrix, textureMatrix);
        PSMTXConcat(translationMatrix, textureMatrix, textureMatrix);

        float translationScale = 1.0f / (float)lbl_806DF060;
        unsigned long frame = glGetCurrentFrame();
        unsigned long frameDiv = frame / lbl_806DF060;
        unsigned long frameMod = frame - frameDiv * lbl_806DF060;
        float translation = translationScale * frameMod;
        translation = 2.0f * translation - 1.0f;
        textureMatrix[0][2] = translation;
        textureMatrix[1][2] = translation;
        GXLoadTexMtxImm(textureMatrix, GX_TEXMTX0, GX_MTX3x4);
    }
    else
    {
        gxSetTexCoordGen(0, 1, 4, 60);
    }

    if (count360 > 0)
        LoadValue_80290914(0, parameters, lbl_8057B0D8);
    if (count360 > 1)
        LoadValue_80290914(1, parameters, lbl_8057B0D8);
    if (count360 > 2)
        LoadValue_80290914(2, parameters, lbl_8057B0D8);
    if (count360 > 3)
        LoadValue_80290914(3, parameters, lbl_8057B0D8);

    if (count364 > 0)
        LoadValue2_80290914(0, parameters, lbl_8057B0D8);
    if (count364 > 1)
        LoadValue2_80290914(1, parameters, lbl_8057B0D8);
    if (count364 > 2)
        LoadValue2_80290914(2, parameters, lbl_8057B0D8);
    if (count364 > 3)
        LoadValue2_80290914(3, parameters, lbl_8057B0D8);

    parameters->texture2.SetWrapS(true);
    parameters->texture2.SetWrapT(true);
    static_cast<GXMaterialProgram_8029BA04*>(this)->BindParameters(packet);

    if (count364 > 0)
    {
        gxSetNumChans(2);
        unsigned int lightMask = 0;
        if (count364 >= 1)
            lightMask |= GX_LIGHT4;
        if (count364 >= 2)
            lightMask |= GX_LIGHT5;
        if (count364 >= 3)
            lightMask |= GX_LIGHT6;
        if (count364 >= 4)
            lightMask |= GX_LIGHT7;
        GXSetChanCtrl(GX_COLOR1, true, GX_SRC_REG, GX_SRC_REG, (GXLightID)lightMask, GX_DF_NONE, GX_AF_NONE);
    }
    else
    {
        gxSetNumChans(1);
        GXSetChanCtrl(GX_COLOR1, false, GX_SRC_REG, GX_SRC_VTX, (GXLightID)(GX_LIGHT4 | GX_LIGHT5 | GX_LIGHT6 | GX_LIGHT7), GX_DF_NONE, GX_AF_NONE);
    }

    if (count360 > 0)
    {
        unsigned int lightMask = 0;
        if (count360 >= 1)
            lightMask |= GX_LIGHT0;
        if (count360 >= 2)
            lightMask |= GX_LIGHT1;
        if (count360 >= 3)
            lightMask |= GX_LIGHT2;
        if (count360 >= 4)
            lightMask |= GX_LIGHT3;
        GXSetChanCtrl(GX_COLOR0, true, GX_SRC_REG, GX_SRC_REG, (GXLightID)lightMask, GX_DF_CLAMP, GX_AF_SPOT);
    }
    else
    {
        GXSetChanCtrl(GX_COLOR0, false, GX_SRC_REG, GX_SRC_VTX, (GXLightID)(GX_LIGHT0 | GX_LIGHT1 | GX_LIGHT2 | GX_LIGHT3), GX_DF_NONE, GX_AF_SPOT);
    }

    if (packet->unknown28 == 0)
    {
        fn_8036D7EC(parameters->matrices, parameters->matricesSize / 48, &modelview, 0);
    }
    else
    {
        fn_8036D774(&modelview);
    }

    GXCallDisplayList(packet->displayList->list, packet->displayList->size);
    gxSetCurrentMtx(0, true);
}
