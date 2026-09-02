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

struct GXMaterialProgramParameters_802987A0
{
    /* 0x00 */ UnidentifiedTextureState texture0;
    /* 0x08 */ UnidentifiedTextureState texture1;
    /* 0x10 */ UnidentifiedTextureState texture2;
    /* 0x18 */ UnidentifiedTextureState texture3;
    /* 0x20 */ UnidentifiedTextureState texture4;
    /* 0x28 */ const void* matrices;
    /* 0x2C */ unsigned long matricesSize;
    /* 0x30 */ float value48;
    /* 0x34 */ float value52;
    /* 0x38 */ float value56;
    /* 0x3C */ float scaleX;
    /* 0x40 */ float scaleY;
    /* 0x44 */ int textureIndex;
    /* 0x48 */ float value72;
    /* 0x4C */ unsigned long value76;
    /* 0x50 */ int value80;
    /* 0x54 */ int value84;
}; // size: 0x58

static const Mtx lbl_804E8730 = {
    { 0.0f, 0.0f, 0.0f, 0.5f },
    { 0.0f, 0.0f, 0.0f, 0.5f },
    { 0.0f, 0.0f, 0.0f, 1.0f },
};

static Mtx lbl_80524190 = {
    { 0.0f, 0.0f, 1.0f, 0.0f },
    { 0.0f, 0.0f, 1.0f, 0.0f },
    { 0.0f, 0.0f, 0.0f, 1.0f },
};

static nlMatrix4 lbl_8057ADF0;
static unsigned long lbl_8057AE30[5] = {
    glGetTexture("global/white"),
    glGetTexture("global/fresnel0"),
    glGetTexture("global/fresnel1"),
    glGetTexture("global/fresnel2"),
    glGetTexture("global/fresnel4"),
};
static unsigned long lbl_8057AE44[5];

static unsigned long lbl_806E1A80;
static float lbl_806E1A88[2];
static void* lbl_806E1A90;
static bool lbl_806E1A94;
static bool lbl_806E1A95;
static unsigned long lbl_806E1A98;

extern "C" void fn_8028BBF0(bool enabled)
{
    if (enabled)
    {
        fn_801836FC(1);
        fn_801832F4(1, lbl_806E1A90, 0);
    }

    gxSetNumChans(1);
    gxSetNumTexGens(6);
    gxSetNumTevStages(7);
    gxSetTevOrder(0, 1, 1, 255);
    gxSetTevOrder(1, 0, 0, 255);
    gxSetTevOrder(2, 255, 255, enabled ? 4 : 255);
    gxSetTevOrder(3, 2, 2, 255);
    gxSetTevOrder(4, 3, 3, 255);
    gxSetTevOrder(5, 5, 5, 255);
    gxSetTevOrder(6, 4, 4, 255);
    GXSetTexCoordGen2(GX_TEXCOORD2, GX_TG_MTX3x4, GX_TG_NRM, GX_TEXMTX0, GX_TRUE, GX_PTTEXMTX0);
    GXSetTexCoordGen2(GX_TEXCOORD5, GX_TG_MTX3x4, GX_TG_NRM, GX_TEXMTX0, GX_TRUE, GX_PTTEXMTX1);
    gxSetTevColourOp(2, 0, 0, 0, true, 1);
    if (enabled && fn_80182118() != 0)
        gxSetTevColourOp(1, 0, 0, 1, true, 0);
    GXSetTevKColorSel(GX_TEVSTAGE0, GX_TEV_KCSEL_K0);
    GXSetTevKColorSel(GX_TEVSTAGE3, GX_TEV_KCSEL_K2);
    GXSetTevKColorSel(GX_TEVSTAGE6, GX_TEV_KCSEL_K3);
    gxSetTevColourIn(0, 15, 12, 8, 14);
    gxSetTevColourIn(1, 15, 0, 8, 15);
    gxSetTevColourIn(2, 15, 0, enabled ? 10 : 12, 15);
    gxSetTevColourIn(3, 15, 14, 8, 15);
    gxSetTevColourIn(4, 15, 0, 8, 15);
    gxSetTevColourIn(5, 15, 0, 8, 2);
    gxSetTevColourIn(6, 0, 8, 14, 15);
    gxSetTevAlphaIn(0, 7, 7, 7, 7);
    gxSetTevAlphaIn(1, 7, 7, 7, 4);
    gxSetTevAlphaIn(2, 7, 7, 7, 0);
    gxSetTevAlphaIn(3, 7, 7, 7, 0);
    gxSetTevAlphaIn(4, 7, 7, 7, 0);
    gxSetTevAlphaIn(5, 7, 7, 7, 0);
    gxSetTevAlphaIn(6, 7, 7, 7, 0);
    GXSetVtxDesc(GX_VA_PNMTXIDX, GX_DIRECT);
}

template <>
void GXMaterialProgramImpl<GXMaterialProgram_802987A0>::Activate(GLView* view)
{
    static_cast<GXMaterialProgram_802987A0*>(this)->ConfigureVertexFormat(true);
    view->m_Interface->GetViewMatrix(lbl_8057ADF0);
    fn_80183BF4(&lbl_8057ADF0);
    fn_80183B40(-1);
    lbl_806E1A80 = -1;
    lbl_806E1A88[0] = 255.0f;
    lbl_806E1A88[1] = 255.0f;
    lbl_806E1A98 = -1;
    GXLoadTexMtxImm(lbl_80524190, 67, GX_MTX3x4);
    lbl_806E1A90 = fn_80182240(0, 1);
    fn_80182ED0(lbl_806E1A90, view, 0);
    lbl_806E1A94 = true;
    fn_8028BBF0(true);
}

template <>
void GXMaterialProgramImpl<GXMaterialProgram_802987A0>::Deactivate()
{
    fn_801836FC(0);
    fn_801832F4(0, lbl_806E1A90, 1);
    gxSetTevColourOp(1, 0, 0, 0, true, 0);
    gxSetTexCoordGen(2, 1, 6, 60);
    gxSetTexCoordGen(5, 1, 9, 60);
    gxSetTevColourOp(2, 0, 0, 0, true, 0);
    gxSetCurrentMtx(0, true);
}

template <>
void GXMaterialProgramImpl<GXMaterialProgram_802987A0>::Prepare(
    const glModelPacket* packet)
{
    fn_802CC978(this, packet, *(unsigned long*)packet->unknown20);
}

struct FloatColour_8028BBF0
{
    float c[4];
};

static inline GXColor ConvertColour_8028BBF0(
    const FloatColour_8028BBF0& source)
{
    GXColor colour;
    colour.r = (unsigned char)(source.c[0] * 255.0f);
    colour.g = (unsigned char)(source.c[1] * 255.0f);
    colour.b = (unsigned char)(source.c[2] * 255.0f);
    colour.a = (unsigned char)(source.c[3] * 255.0f);
    return colour;
}

template <>
void GXMaterialProgramImpl<GXMaterialProgram_802987A0>::Draw(
    const glModelPacket* packet)
{
    GXMaterialProgram_802987A0* program = static_cast<GXMaterialProgram_802987A0*>(this);
    program->BindVertexArrays(packet);
    program->BindParameters(packet);

    float value72 = ((GXMaterialProgramParameters_802987A0*)packet->unknown20)->value72;
    gxSetNumTevStages(value72 == 0.0f ? 6 : 7);
    GXMaterialProgramParameters_802987A0* parameters = (GXMaterialProgramParameters_802987A0*)packet->unknown20;
    float value48 = parameters->value48;
    float value52 = parameters->value52;
    if (value52 == 0.0f)
        return;
    float value56 = parameters->value56;
    float scaleX = parameters->scaleX;
    float scaleY = parameters->scaleY;
    int textureIndex = parameters->textureIndex;

    FloatColour_8028BBF0 source48 = { { value48, value48, value48, value48 } };
    GXSetTevKColor(GX_KCOLOR0, ConvertColour_8028BBF0(source48));
    FloatColour_8028BBF0 source52 = { { value52, value52, value52, value52 } };
    GXSetTevKColor(GX_KCOLOR1, ConvertColour_8028BBF0(source52));
    FloatColour_8028BBF0 source56 = { { value56, value56, value56, value56 } };
    GXSetTevKColor(GX_KCOLOR2, ConvertColour_8028BBF0(source56));
    FloatColour_8028BBF0 source72 = { { value72, value72, value72, value72 } };
    GXSetTevKColor(GX_KCOLOR3, ConvertColour_8028BBF0(source72));

    bool enabled = ((GXMaterialProgramParameters_802987A0*)packet->unknown20)->value80 == 1;
    if (enabled != lbl_806E1A94)
    {
        lbl_806E1A94 = enabled;
        Deactivate();
        fn_8028BBF0(enabled);
    }

    if (lbl_806E1A88[0] != scaleX || lbl_806E1A88[1] != scaleY)
    {
        Mtx textureMatrix;
        memcpy(textureMatrix, lbl_804E8730, sizeof(Mtx));
        textureMatrix[0][0] = 0.5f * scaleX;
        textureMatrix[1][1] = -0.5f * scaleY;
        GXLoadTexMtxImm(textureMatrix, 64, GX_MTX3x4);
        lbl_806E1A88[0] = scaleX;
        lbl_806E1A88[1] = scaleY;
    }

    unsigned long texture = lbl_8057AE30[textureIndex];
    if (lbl_806E1A98 != texture)
    {
        if (!lbl_806E1A95)
        {
            lbl_8057AE44[0] = 0xFFFF;
            lbl_8057AE44[1] = 0xFFFF;
            lbl_8057AE44[2] = 0xFFFF;
            lbl_8057AE44[3] = 0xFFFF;
            lbl_8057AE44[4] = 0xFFFF;
            lbl_806E1A95 = true;
        }
        if (lbl_8057AE44[textureIndex] == 0xFFFF
            || lbl_8057AE44[textureIndex] == 0)
        {
            lbl_8057AE44[textureIndex] = fn_802CE1B8(fn_802CDF0C(), texture);
        }
        UnidentifiedTextureState textureState;
        textureState.texture = texture;
        textureState.textureIndex = lbl_8057AE44[textureIndex];
        textureState.flags = 0;
        textureState.unknown07 = 0;
        textureState.SetWrapS(true);
        textureState.SetWrapT(true);
        fn_8036BE88(5, &textureState);
        lbl_806E1A98 = texture;
    }

    nlMatrix4 model;
    nlMatrix4 modelview;
    unsigned long matrix = packet->matrix;
    glGetMatrix(matrix, model);
    nlMultMatrices(modelview, model, lbl_8057ADF0);
    fn_80183B40(matrix);

    if (matrix != lbl_806E1A80)
    {
        Mtx source;
        Mtx inverse;
        lbl_806E1A80 = matrix;
        glxCopyMatrix(source, modelview);
        PSMTXInvXpose(source, inverse);
        GXLoadTexMtxImm(inverse, 30, GX_MTX3x4);
    }

    if (packet->unknown28 == 0)
    {
        fn_8036D7EC(
            ((GXMaterialProgramParameters_802987A0*)packet->unknown20)->matrices,
            ((GXMaterialProgramParameters_802987A0*)packet->unknown20)->matricesSize / 48,
            &modelview, 0);
    }
    else
    {
        fn_8036D774(&modelview);
    }

    fn_801837DC(
        1,
        ((GXMaterialProgramParameters_802987A0*)packet->unknown20)->value76);
    if (lbl_806E1A94 && value52 != 1.0f)
    {
        gxSaveZMode();
        bool colourUpdate = gxSetColourUpdate(false);
        GXCallDisplayList(packet->displayList->list, packet->displayList->size);
        gxSetColourUpdate(colourUpdate);
        gxSetZMode(true, GX_EQUAL, true);
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
