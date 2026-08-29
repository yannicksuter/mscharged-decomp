#include <revolution/gx.h>
#include <revolution/mtx.h>

#include <string.h>

#include "NL/gl/glMatrix.h"
#include "NL/gl/glState.h"
#include "NL/gl/glView.h"
#include "NL/glx/GXMaterialProgram.h"
#include "NL/glx/glxDisplayList.h"
#include "NL/glx/glxGX.h"
#include "NL/glx/glxMatrix.h"
#include "NL/nlMath.h"

extern "C"
{
    int fn_80182118();
    void* fn_80182240(int, int);
    void fn_80182ED0(void*, GLView*, int);
    void fn_801832F4(int, void*, int);
    void fn_801836FC(int);
    void fn_801837DC(int, unsigned long);
    void fn_80183A98();
    void fn_80183B40(unsigned long matrix);
    void fn_80183BF4(const nlMatrix4* matrix);
    void* fn_802CDF0C();
    unsigned long fn_802CE1B8(void*, unsigned long texture);
    void fn_8036D774(const nlMatrix4* matrix);
    void fn_8036D7EC(
        const void* matrices, unsigned long count, const nlMatrix4* matrix,
        int unknown);
}

struct GXMaterialProgramParameters_80298478
{
    /* 0x00 */ UnidentifiedTextureState texture0;
    /* 0x08 */ UnidentifiedTextureState texture1;
    /* 0x10 */ UnidentifiedTextureState texture2;
    /* 0x18 */ UnidentifiedTextureState texture3;
    /* 0x20 */ const void* matrices;
    /* 0x24 */ unsigned long matricesSize;
    /* 0x28 */ float value40;
    /* 0x2C */ float value44;
    /* 0x30 */ float value48;
    /* 0x34 */ float scaleX;
    /* 0x38 */ float scaleY;
    /* 0x3C */ int textureIndex;
    /* 0x40 */ unsigned long value64;
    /* 0x44 */ int value68;
}; // size: 0x48

static bool lbl_806DEFF0 = true;
static bool lbl_806DEFF1 = true;
static int lbl_806DEFF4 = -1;
static float lbl_806DEFF8 = 1.0f;
static float lbl_806DEFFC = 1.0f;
static bool lbl_806DF000 = true;

static const Mtx lbl_804E8700 = {
    { 0.0f, 0.0f, 0.0f, 0.5f },
    { 0.0f, 0.0f, 0.0f, 0.5f },
    { 0.0f, 0.0f, 0.0f, 1.0f },
};

static Mtx lbl_80524110 = {
    { 0.0f, 0.0f, 1.0f, 0.0f },
    { 0.0f, 0.0f, 1.0f, 0.0f },
    { 0.0f, 0.0f, 0.0f, 1.0f },
};

static nlMatrix4 lbl_8057AD88;
static unsigned long lbl_8057ADC8[5] = {
    glGetTexture("global/white"),
    glGetTexture("global/fresnel0"),
    glGetTexture("global/fresnel1"),
    glGetTexture("global/fresnel2"),
    glGetTexture("global/fresnel4"),
};
static unsigned long lbl_8057ADDC[5];

static unsigned long lbl_806E1A60;
static float lbl_806E1A68[2];
static void* lbl_806E1A70;
static bool lbl_806E1A74;
static bool lbl_806E1A75;
static unsigned long lbl_806E1A78;

extern "C" void fn_8028B1FC(bool enabled)
{
    if (enabled)
    {
        fn_801836FC(1);
        fn_801832F4(1, lbl_806E1A70, 0);
    }

    gxSetNumChans(1);
    gxSetNumTexGens(5);
    gxSetNumTevStages(6);
    gxSetTevOrder(0, 1, 1, 255);
    gxSetTevOrder(1, 0, 0, 255);
    gxSetTevOrder(2, 255, 255, enabled ? 4 : 255);
    gxSetTevOrder(3, 2, 2, 255);
    gxSetTevOrder(4, 3, 3, 255);
    gxSetTevOrder(5, 4, 4, 255);
    GXSetTexCoordGen2(GX_TEXCOORD2, GX_TG_MTX3x4, GX_TG_POS, GX_TEXMTX0, GX_TRUE, GX_PTTEXMTX0);
    GXSetTexCoordGen2(GX_TEXCOORD4, GX_TG_MTX3x4, GX_TG_POS, GX_TEXMTX0, GX_TRUE, GX_PTTEXMTX3);
    gxSetTevColourOp(2, 0, 0, 0, true, 1);
    if (enabled && fn_80182118() != 0)
        gxSetTevColourOp(1, 0, 0, 1, true, 0);
    GXSetTevKColorSel(GX_TEVSTAGE0, GX_TEV_KCSEL_K0);
    GXSetTevKColorSel(GX_TEVSTAGE3, GX_TEV_KCSEL_K2);
    gxSetTevColourIn(0, 15, 12, 8, 14);
    gxSetTevColourIn(1, 15, 0, 8, 15);
    gxSetTevColourIn(2, 15, 0, enabled ? 10 : 12, 15);
    gxSetTevColourIn(3, 15, 14, 8, 15);
    gxSetTevColourIn(4, 15, 0, 8, 15);
    gxSetTevColourIn(5, 15, 0, 8, 2);
    gxSetTevAlphaIn(0, 7, 7, 7, 7);
    gxSetTevAlphaIn(1, 7, 7, 7, 4);
    gxSetTevAlphaIn(2, 7, 7, 7, 0);
    gxSetTevAlphaIn(3, 7, 7, 7, 0);
    gxSetTevAlphaIn(4, 7, 7, 7, 0);
    gxSetTevAlphaIn(5, 7, 7, 7, 0);
    GXSetVtxDesc(GX_VA_PNMTXIDX, GX_DIRECT);
}

template <>
void GXMaterialProgramImpl<GXMaterialProgram_80298478>::Activate(GLView* view)
{
    static_cast<GXMaterialProgram_80298478*>(this)->ConfigureVertexFormat(true);
    view->m_Interface->GetViewMatrix(lbl_8057AD88);
    fn_80183BF4(&lbl_8057AD88);
    fn_80183B40(-1);
    lbl_806E1A60 = -1;
    lbl_806E1A68[0] = 255.0f;
    lbl_806E1A68[1] = 255.0f;
    lbl_806E1A78 = -1;
    GXLoadTexMtxImm(lbl_80524110, 67, GX_MTX_3x4);
    lbl_806E1A70 = fn_80182240(0, 1);
    fn_80182ED0(lbl_806E1A70, view, 0);
    lbl_806E1A74 = lbl_806DEFF1;
    fn_8028B1FC(lbl_806DEFF1);
}

template <>
void GXMaterialProgramImpl<GXMaterialProgram_80298478>::Deactivate()
{
    fn_801836FC(0);
    fn_801832F4(0, lbl_806E1A70, 1);
    gxSetTevColourOp(1, 0, 0, 0, true, 0);
    gxSetTexCoordGen(2, 1, 6, 60);
    gxSetTexCoordGen(4, 1, 8, 60);
    gxSetTevColourOp(2, 0, 0, 0, true, 0);
    gxSetCurrentMtx(0, true);
}

template <>
void GXMaterialProgramImpl<GXMaterialProgram_80298478>::Prepare(
    const glModelPacket* packet)
{
    fn_802CC978(this, packet, *(unsigned long*)packet->unknown20);
}

struct FloatColour_8028B1FC
{
    float c[4];
};

static inline GXColor ConvertColour_8028B1FC(
    const FloatColour_8028B1FC& source)
{
    GXColor colour;
    colour.r = (unsigned char)(source.c[0] * 255.0f);
    colour.g = (unsigned char)(source.c[1] * 255.0f);
    colour.b = (unsigned char)(source.c[2] * 255.0f);
    colour.a = (unsigned char)(source.c[3] * 255.0f);
    return colour;
}

template <>
void GXMaterialProgramImpl<GXMaterialProgram_80298478>::Draw(
    const glModelPacket* packet)
{
    if (!lbl_806DEFF0)
        return;

    GXMaterialProgram_80298478* program = static_cast<GXMaterialProgram_80298478*>(this);
    program->BindVertexArrays(packet);
    program->BindParameters(packet);
    GXMaterialProgramParameters_80298478* parameters = (GXMaterialProgramParameters_80298478*)packet->unknown20;

    float value40 = lbl_806DEFF8;
    if (value40 == 1.0f)
        value40 = parameters->value40;
    float value44 = lbl_806DEFFC;
    if (value44 == 1.0f)
        value44 = parameters->value44;
    if (value44 == 0.0f)
        return;

    int textureIndex = lbl_806DEFF4;
    float value48 = parameters->value48;
    float scaleX = parameters->scaleX;
    float scaleY = parameters->scaleY;
    if (textureIndex < 0)
        textureIndex = parameters->textureIndex;

    FloatColour_8028B1FC source40 = { { value40, value40, value40, value40 } };
    GXColor colour40 = ConvertColour_8028B1FC(source40);
    GXSetTevKColor(GX_KCOLOR0, colour40);
    FloatColour_8028B1FC source44 = { { value44, value44, value44, value44 } };
    GXColor colour44 = ConvertColour_8028B1FC(source44);
    GXSetTevKColor(GX_KCOLOR1, colour44);
    FloatColour_8028B1FC source48 = { { value48, value48, value48, value48 } };
    GXColor colour48 = ConvertColour_8028B1FC(source48);
    GXSetTevKColor(GX_KCOLOR2, colour48);

    bool enabled = parameters->value68 == 1;
    if (enabled != lbl_806E1A74)
    {
        lbl_806E1A74 = enabled;
        Deactivate();
        fn_8028B1FC(enabled);
    }

    if (lbl_806E1A68[0] != scaleX || lbl_806E1A68[1] != scaleY)
    {
        Mtx textureMatrix;
        memcpy(textureMatrix, lbl_804E8700, sizeof(Mtx));
        textureMatrix[0][0] = 0.5f * scaleX;
        textureMatrix[1][1] = -0.5f * scaleY;
        GXLoadTexMtxImm(textureMatrix, 64, GX_MTX_3x4);
        lbl_806E1A68[0] = scaleX;
        lbl_806E1A68[1] = scaleY;
    }

    unsigned long texture = lbl_8057ADC8[textureIndex];
    if (lbl_806E1A78 != texture)
    {
        if (!lbl_806E1A75)
        {
            lbl_8057ADDC[0] = -1;
            lbl_8057ADDC[1] = -1;
            lbl_8057ADDC[2] = -1;
            lbl_8057ADDC[3] = -1;
            lbl_8057ADDC[4] = -1;
            lbl_806E1A75 = true;
        }
        if (lbl_8057ADDC[textureIndex] == 0xFFFF
            || lbl_8057ADDC[textureIndex] == 0)
        {
            lbl_8057ADDC[textureIndex] = fn_802CE1B8(fn_802CDF0C(), texture);
        }
        UnidentifiedTextureState textureState;
        textureState.texture = texture;
        textureState.textureIndex = lbl_8057ADDC[textureIndex];
        textureState.flags = 3;
        textureState.unknown07 = 0;
        fn_8036BE88(4, &textureState);
        lbl_806E1A78 = texture;
    }

    nlMatrix4 model;
    nlMatrix4 modelview;
    glGetMatrix(packet->matrix, model);
    nlMultMatrices(modelview, model, lbl_8057AD88);
    fn_80183B40(packet->matrix);

    if (lbl_806E1A60 != packet->matrix)
    {
        Mtx source;
        Mtx inverse;
        lbl_806E1A60 = packet->matrix;
        glxCopyMatrix(source, modelview);
        PSMTXInvXpose(source, inverse);
        GXLoadTexMtxImm(inverse, 30, GX_MTX_3x4);
    }

    if (*(unsigned long*)packet->unknown28 == 0)
    {
        fn_8036D7EC(parameters->matrices, parameters->matricesSize / 48, &modelview, 0);
    }
    else
    {
        fn_8036D774(&modelview);
    }

    fn_801837DC(1, parameters->value64);
    if (lbl_806DEFF1 && value44 != 1.0f)
    {
        gxSaveZMode();
        if (lbl_806DF000)
        {
            bool colourUpdate = gxSetColourUpdate(false);
            GXCallDisplayList(
                packet->displayList->list, packet->displayList->size);
            gxSetColourUpdate(colourUpdate);
            gxSetZMode(true, GX_EQUAL, true);
        }
        gxSaveBlendMode();
        gxSetBlendMode(true, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, false);
        GXSetTevKAlphaSel(GX_TEVSTAGE1, GX_TEV_KASEL_K1_A);
        gxSetTevAlphaIn(1, 7, 7, 7, 6);
        GXCallDisplayList(packet->displayList->list, packet->displayList->size);
        gxRestoreBlendMode();
        gxRestoreZMode();
    }
    else
    {
        GXCallDisplayList(packet->displayList->list, packet->displayList->size);
    }
    fn_80183A98();
}
