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
    void fn_801B5EE8(const bool* flags, int stageCount, int texGenCount,
        int texture4, int texture5, int texCoord3, int texCoord4,
        int texCoord5, int finalTexture, float value);
    void* fn_802CDF0C();
    unsigned long fn_802CE1B8(void*, unsigned long texture);
    void fn_8036D774(const nlMatrix4* matrix);
    void fn_8036D7EC(
        const void* matrices, unsigned long count, const nlMatrix4* matrix,
        int unknown);
}

struct GXMaterialProgramParameters_80298B18
{
    /* 0x00 */ UnidentifiedTextureState texture0;
    /* 0x08 */ UnidentifiedTextureState texture1;
    /* 0x10 */ UnidentifiedTextureState texture2;
    /* 0x18 */ UnidentifiedTextureState texture3;
    /* 0x20 */ UnidentifiedTextureState texture4;
    /* 0x28 */ UnidentifiedTextureState texture5;
    /* 0x30 */ const void* matrices;
    /* 0x34 */ unsigned long matricesSize;
    /* 0x38 */ float value56;
    /* 0x3C */ float value60;
    /* 0x40 */ float scaleX;
    /* 0x44 */ float scaleY;
    /* 0x48 */ int textureIndex;
    /* 0x4C */ float value76;
    /* 0x50 */ int value80;
    /* 0x54 */ unsigned long value84;
    /* 0x58 */ int value88;
    /* 0x5C */ int value92;
    /* 0x60 */ int value96;
    /* 0x64 */ int value100;
}; // size: 0x68

static int lbl_806DF008 = 5;
static int lbl_806DF00C = 4;
static bool lbl_806DF010 = true;
static int lbl_806DF014 = -1;
static bool lbl_806DF018 = true;
static int lbl_806DF01C = 255;
static int lbl_806DF020 = 255;

static const Mtx lbl_804E8760 = {
    { 0.0f, 0.0f, 0.0f, 0.5f },
    { 0.0f, 0.0f, 0.0f, 0.5f },
    { 0.0f, 0.0f, 0.0f, 1.0f },
};

static Mtx lbl_80524210 = {
    { 0.0f, 0.0f, -1.0f, 0.0f },
    { 0.0f, 0.0f, -1.0f, 0.0f },
    { 0.0f, 0.0f, 0.0f, 1.0f },
};

static nlMatrix4 lbl_8057AE58;
static unsigned long lbl_8057AE98[5] = {
    glGetTexture("global/white"),
    glGetTexture("global/fresnel0"),
    glGetTexture("global/fresnel1"),
    glGetTexture("global/fresnel2"),
    glGetTexture("global/fresnel4"),
};
static unsigned long lbl_8057AEAC[5];

static bool lbl_806E1AA0;
static float lbl_806E1AA4;
static float lbl_806E1AA8;
static bool lbl_806E1AAC;
static bool lbl_806E1AAD;
static bool lbl_806E1AAE;
static bool lbl_806E1AAF;
static bool lbl_806E1AB0;
static unsigned long lbl_806E1AB4;
static float lbl_806E1AB8[2];
static void* lbl_806E1AC0;
static bool lbl_806E1AC4;
static bool lbl_806E1AC5;
static unsigned long lbl_806E1AC8;

extern "C" void fn_8028C66C(bool enabled)
{
    if (!lbl_806DF010)
        return;

    if (enabled)
    {
        fn_801836FC(1);
        fn_801832F4(1, lbl_806E1AC0, 0);
    }

    lbl_806DF01C = 3;
    lbl_806DF020 = 3;
    GXSetVtxDesc(GX_VA_PNMTXIDX, GX_DIRECT);
    if (lbl_806E1AAE)
        lbl_806DF008 = 2;

    gxSetNumChans(1);
    gxSetNumTexGens(lbl_806DF00C);
    gxSetNumTevStages(lbl_806DF008);
    gxSetTevOrder(0, 0, 0, 255);
    gxSetTevColourIn(0, 15, 12, 8, 15);
    if (lbl_806E1AA0)
        gxSetTevColourIn(0, 15, 15, 15, 15);
    gxSetTevAlphaIn(0, 7, 7, 7, 4);
    if (enabled && fn_80182118() != 0)
        gxSetTevColourOp(0, 0, 0, 1, true, 0);

    gxSetTevOrder(1, 255, 255, enabled ? 4 : 255);
    gxSetTevColourIn(
        1, 15, lbl_806E1AAE ? 12 : 0, enabled ? 10 : 12, 15);
    gxSetTevAlphaIn(1, 7, 7, 7, 0);
    gxSetTevColourOp(1, 0, 0, 0, true, !lbl_806E1AAE);

    gxSetTevOrder(2, 1, 1, 255);
    gxSetTevColourIn(2, 15, 14, 8, 15);
    gxSetTevAlphaIn(2, 7, 7, 7, 0);
    GXSetTexCoordGen2(GX_TEXCOORD1, GX_TG_MTX3x4, GX_TG_POS, GX_TEXMTX0, GX_TRUE, GX_PTTEXMTX0);
    GXSetTevKColorSel(GX_TEVSTAGE2, GX_TEV_KCSEL_K2);

    gxSetTevOrder(3, 2, 2, 255);
    gxSetTevColourIn(3, 15, 0, 8, 15);
    gxSetTevAlphaIn(3, 7, 7, 7, 0);

    gxSetTevOrder(4, lbl_806DF020, lbl_806DF01C, 255);
    GXSetTexCoordGen2((GXTexCoordID)lbl_806DF020, GX_TG_MTX2x4, GX_TG_POS, GX_TEXMTX1, GX_FALSE, GX_PTIDENTITY);
    gxSetTevColourIn(4, 15, 0, 8, 2);
    if (lbl_806E1AAC)
        gxSetTevColourIn(4, 15, 12, 8, 15);
    if (lbl_806E1AAD)
        gxSetTevColourIn(4, 15, 0, 8, 15);
    gxSetTevAlphaIn(4, 7, 7, 7, 0);
}

template <>
void GXMaterialProgramImpl<GXMaterialProgram_80298B18>::Activate(GLView* view)
{
    static_cast<GXMaterialProgram_80298B18*>(this)->ConfigureVertexFormat(true);
    view->m_Interface->GetViewMatrix(lbl_8057AE58);
    if (lbl_806DF018)
    {
        fn_80183BF4(&lbl_8057AE58);
        fn_80183B40(-1);
    }
    lbl_806E1AB4 = -1;
    lbl_806E1AB8[0] = 255.0f;
    lbl_806E1AB8[1] = 255.0f;
    lbl_806E1AC8 = -1;
    GXLoadTexMtxImm(lbl_80524210, 67, GX_MTX3x4);
    lbl_806E1AC0 = fn_80182240(1, 1);
    fn_80182ED0(lbl_806E1AC0, view, 1);
    lbl_806E1AC4 = true;
    fn_8028C66C(true);
}

template <>
void GXMaterialProgramImpl<GXMaterialProgram_80298B18>::Deactivate()
{
    lbl_806DF020 = 3;
    fn_801836FC(0);
    fn_801832F4(0, lbl_806E1AC0, 1);
    gxSetTevColourOp(0, 0, 0, 0, true, 0);
    gxSetTevColourOp(1, 0, 0, 0, true, 0);
    gxSetTexCoordGen(1, 1, 5, 60);
    gxSetTexCoordGen(lbl_806DF020, 1, lbl_806DF020 + 4, 60);
    gxSetCurrentMtx(0, true);
}

template <>
void GXMaterialProgramImpl<GXMaterialProgram_80298B18>::Prepare(
    const glModelPacket* packet)
{
    fn_802CC978(this, packet, *(unsigned long*)packet->unknown20);
}

struct FloatColour_8028C66C
{
    float c[4];
};

static inline GXColor ConvertColour_8028C66C(
    const FloatColour_8028C66C& source)
{
    GXColor colour;
    colour.r = (unsigned char)(source.c[0] * 255.0f);
    colour.g = (unsigned char)(source.c[1] * 255.0f);
    colour.b = (unsigned char)(source.c[2] * 255.0f);
    colour.a = (unsigned char)(source.c[3] * 255.0f);
    return colour;
}

template <>
void GXMaterialProgramImpl<GXMaterialProgram_80298B18>::Draw(
    const glModelPacket* packet)
{
    if (!lbl_806DF010)
        return;

    GXMaterialProgram_80298B18* program = static_cast<GXMaterialProgram_80298B18*>(this);
    program->BindVertexArrays(packet);
    program->BindParameters(packet);
    GXMaterialProgramParameters_80298B18* parameters = (GXMaterialProgramParameters_80298B18*)packet->unknown20;

    float value76 = parameters->value76;
    if (lbl_806E1AA4 > 0.0f)
        value76 = lbl_806E1AA4;

    int stageCount = lbl_806DF008;
    if (parameters->value92 != 0)
    {
        gxSetNumTevStages(1);
        gxSetTevColourIn(0, 15, 15, 15, 15);
        gxSetTevAlphaIn(0, 7, 7, 7, 6);
    }
    else
    {
        gxSetNumTevStages(stageCount);
        gxSetTevColourIn(0, 15, 12, 8, 15);
        bool flags[2];
        flags[0] = lbl_806E1AAF || parameters->value96 != 0;
        flags[1] = lbl_806E1AB0 || parameters->value100 != 0;
        fn_801B5EE8(flags, stageCount, lbl_806DF00C, 4, 5, 3, 4, 5, 3, value76);
    }

    float value56 = parameters->value56;
    if (value56 == 0.0f)
        return;
    float scaleX = parameters->scaleX;
    float scaleY = parameters->scaleY;
    int textureIndex = parameters->textureIndex;
    float value60 = parameters->value60;
    if (lbl_806E1AA8 != 0.0f)
        value60 = lbl_806E1AA8;
    if (lbl_806DF014 != -1)
        textureIndex = lbl_806DF014;

    FloatColour_8028C66C sourceWhite = { { 1.0f, 1.0f, 1.0f, 1.0f } };
    GXColor colourWhite = ConvertColour_8028C66C(sourceWhite);
    GXSetTevKColor(GX_KCOLOR0, colourWhite);
    FloatColour_8028C66C source56 = { { value56, value56, value56, value56 } };
    GXColor colour56 = ConvertColour_8028C66C(source56);
    GXSetTevKColor(GX_KCOLOR1, colour56);
    FloatColour_8028C66C source60 = { { value60, value60, value60, value60 } };
    GXColor colour60 = ConvertColour_8028C66C(source60);
    GXSetTevKColor(GX_KCOLOR2, colour60);

    bool enabled = parameters->value88 == 1;
    if (enabled != lbl_806E1AC4)
    {
        lbl_806E1AC4 = enabled;
        Deactivate();
        fn_8028C66C(enabled);
    }

    if (lbl_806E1AB8[0] != scaleX || lbl_806E1AB8[1] != scaleY)
    {
        Mtx textureMatrix;
        memcpy(textureMatrix, lbl_804E8760, sizeof(Mtx));
        textureMatrix[0][0] = 0.5f * scaleX;
        textureMatrix[1][1] = -0.5f * scaleY;
        GXLoadTexMtxImm(textureMatrix, 64, GX_MTX3x4);
        lbl_806E1AB8[0] = scaleX;
        lbl_806E1AB8[1] = scaleY;
    }

    if (value76 == 0.0f)
    {
        unsigned long texture = lbl_8057AE98[textureIndex];
        if (!lbl_806E1AC5)
        {
            lbl_8057AEAC[0] = -1;
            lbl_8057AEAC[1] = -1;
            lbl_8057AEAC[2] = -1;
            lbl_8057AEAC[3] = -1;
            lbl_8057AEAC[4] = -1;
            lbl_806E1AC5 = true;
        }
        if (lbl_8057AEAC[textureIndex] == 0xFFFF
            || lbl_8057AEAC[textureIndex] == 0)
        {
            lbl_8057AEAC[textureIndex] = fn_802CE1B8(fn_802CDF0C(), texture);
        }
        UnidentifiedTextureState textureState;
        textureState.texture = texture;
        textureState.textureIndex = lbl_8057AEAC[textureIndex];
        textureState.flags = 3;
        textureState.unknown07 = 0;
        fn_8036BE88(lbl_806DF01C, &textureState);
    }

    nlMatrix4 model;
    nlMatrix4 modelview;
    glGetMatrix(packet->matrix, model);
    nlMultMatrices(modelview, model, lbl_8057AE58);
    if (lbl_806DF018)
        fn_80183B40(packet->matrix);

    if (lbl_806E1AB4 != packet->matrix)
    {
        Mtx source;
        Mtx inverse;
        lbl_806E1AB4 = packet->matrix;
        glxCopyMatrix(source, modelview);
        PSMTXInvXpose(source, inverse);
        GXLoadTexMtxImm(inverse, 30, GX_MTX3x4);

        float texturePlane[2][4] = {
            { inverse[2][0], inverse[2][1], inverse[2][2], inverse[2][3] },
            { 0.0f, 0.0f, 0.0f, 0.0f },
        };
        GXLoadTexMtxImm(texturePlane, 33, GX_MTX2x4);
    }

    if (packet->unknown28 == 0)
    {
        fn_8036D7EC(parameters->matrices, parameters->matricesSize / 48, &modelview, 0);
    }
    else
    {
        fn_8036D774(&modelview);
    }

    if (lbl_806DF018)
        fn_801837DC(1, parameters->value84);
    if (lbl_806E1AC4 && value56 != 1.0f)
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
