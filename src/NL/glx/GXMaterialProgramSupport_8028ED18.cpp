#include <revolution/gx.h>
#include <revolution/mtx.h>

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
    void* fn_80182240(int, int);
    unsigned long fn_80182EB8();
    void fn_80182ED0(void*, GLView*, int);
    void fn_801832F4(int, void*, int);
    void fn_801836FC(int);
    void fn_8036D774(const nlMatrix4* matrix);
    void fn_8036D7EC(
        const void* matrices, unsigned long count, const nlMatrix4* matrix,
        int unknown);
}

struct GXMaterialProgramParameters_802997B8
{
    /* 0x00 */ UnidentifiedTextureState texture0;
    /* 0x08 */ UnidentifiedTextureState texture1;
    /* 0x10 */ UnidentifiedTextureState texture2;
    /* 0x18 */ const void* matrices;
    /* 0x1C */ unsigned long matricesSize;
    /* 0x20 */ float value32;
    /* 0x24 */ float value36;
    /* 0x28 */ int value40;
    /* 0x2C */ int value44;
}; // size: 0x30

static bool lbl_806DF038 = true;
static bool lbl_806DF039 = true;
static bool lbl_806DF03A = true;
static float lbl_806DF03C = 1.0f;
static float lbl_806DF040 = 1.0f;
static bool lbl_806DF044 = true;
static int lbl_806DF048 = -1;

static Mtx lbl_80524290 = {
    { 0.0f, 0.0f, 1.0f, 0.0f },
    { 0.0f, 0.0f, 1.0f, 0.0f },
    { 0.0f, 0.0f, 0.0f, 1.0f },
};

static nlMatrix4 lbl_8057AF80;
static unsigned long lbl_8057AFC0[5] = {
    glGetTexture("global/white"),
    glGetTexture("global/cfresnel1"),
    glGetTexture("global/cfresnel2"),
    glGetTexture("global/cfresnel3"),
    glGetTexture("global/cfresnel4"),
};

static bool lbl_806E1AF8;
static unsigned long lbl_806E1AFC;
static void* lbl_806E1B00;
static unsigned long lbl_806E1B04;

template <>
void GXMaterialProgramImpl<GXMaterialProgram_802997B8>::Activate(GLView* view)
{
    static_cast<GXMaterialProgram_802997B8*>(this)->ConfigureVertexFormat(true);
    view->m_Interface->GetViewMatrix(lbl_8057AF80);
    lbl_806E1AFC = -1;
    lbl_806E1B04 = -1;
    GXLoadTexMtxImm(lbl_80524290, 67, GX_MTX3x4);
    gxSetTexCoordGen(3, 0, 1, 30, true, 67);

    if (lbl_806DF039)
    {
        lbl_806E1B00 = fn_80182240(0, 1);
        fn_80182ED0(lbl_806E1B00, view, 0);
        fn_801832F4(1, lbl_806E1B00, 0);
    }

    unsigned int numChans;
    unsigned int numTexGens;
    unsigned int numTevStages;
    if (lbl_806DF039 && lbl_806E1AF8)
    {
        UnidentifiedTextureState texture;
        texture.texture = fn_80182EB8();
        texture.textureIndex = 0xFFFF;
        texture.flags = 3;
        texture.unknown07 = 0;
        fn_8036BE88(4, &texture);

        numChans = 1;
        numTexGens = 5;
        numTevStages = 6;
        bool subtract = lbl_806DF03A;
        gxSetTevOrder(0, 0, 0, 255);
        gxSetTevOrder(1, 3, 3, 255);
        gxSetTevOrder(2, 2, 2, 255);
        gxSetTevOrder(3, 1, 1, 255);
        gxSetTevOrder(4, 255, 255, 255);
        gxSetTevOrder(5, 4, 4, 4);
        gxSetTexCoordGen(4, 10, 19, 60);
        gxSetTevColourOp(0, 0, 0, 0, true, 1);
        gxSetTevColourOp(2, 0, 0, 0, true, 2);
        gxSetTevColourOp(5, 0, 0, subtract, true, 0);
        GXSetTevKColorSel(GX_TEVSTAGE3, GX_TEV_KCSEL_K0);
        gxSetTevColourIn(0, 15, 15, 15, 8);
        gxSetTevColourIn(1, 15, 15, 15, 8);
        gxSetTevColourIn(2, 2, 8, 0, 15);
        gxSetTevColourIn(3, 15, 12, 8, 14);
        gxSetTevColourIn(4, 15, 0, 4, 15);
        gxSetTevColourIn(5, 15, 0, 8, 15);
        gxSetTevAlphaIn(0, 7, 7, 7, 4);
        gxSetTevAlphaIn(1, 7, 7, 7, 0);
        gxSetTevAlphaIn(2, 7, 7, 7, 0);
        gxSetTevAlphaIn(3, 7, 7, 7, 0);
        gxSetTevAlphaIn(4, 7, 7, 7, 0);
        gxSetTevAlphaIn(5, 7, 7, 7, 0);
    }
    else if (lbl_806DF039)
    {
        fn_801836FC(1);
        numChans = 1;
        numTexGens = 4;
        numTevStages = 6;
        gxSetTevOrder(0, 0, 0, 255);
        gxSetTevOrder(1, 3, 3, 255);
        gxSetTevOrder(2, 2, 2, 255);
        gxSetTevOrder(3, 1, 1, 255);
        gxSetTevOrder(4, 255, 255, 255);
        gxSetTevOrder(5, 255, 255, 4);
        gxSetTevColourOp(0, 0, 0, 0, true, 1);
        gxSetTevColourOp(2, 0, 0, 0, true, 2);
        GXSetTevKColorSel(GX_TEVSTAGE3, GX_TEV_KCSEL_K0);
        gxSetTevColourIn(0, 15, 15, 15, 8);
        gxSetTevColourIn(1, 15, 15, 15, 8);
        gxSetTevColourIn(2, 2, 8, 0, 15);
        gxSetTevColourIn(3, 15, 12, 8, 14);
        gxSetTevColourIn(4, 15, 0, 4, 15);
        gxSetTevColourIn(5, 15, 0, 10, 15);
        gxSetTevAlphaIn(0, 7, 7, 7, 4);
        gxSetTevAlphaIn(1, 7, 7, 7, 0);
        gxSetTevAlphaIn(2, 7, 7, 7, 0);
        gxSetTevAlphaIn(3, 7, 7, 7, 0);
        gxSetTevAlphaIn(4, 7, 7, 7, 0);
        gxSetTevAlphaIn(5, 7, 7, 7, 0);
    }
    else
    {
        numChans = 0;
        numTexGens = 1;
        lbl_806E1B00 = 0;
        numTevStages = 1;
        gxSetTevOrder(0, 0, 0, 255);
        gxSetTevColourIn(0, 15, 12, 8, 15);
        gxSetTevAlphaIn(0, 7, 6, 4, 7);
    }

    gxSetNumChans(numChans);
    gxSetNumTexGens(numTexGens);
    gxSetNumTevStages(numTevStages);
    GXSetVtxDesc(GX_VA_PNMTXIDX, GX_DIRECT);
}

template <>
void GXMaterialProgramImpl<GXMaterialProgram_802997B8>::Deactivate()
{
    gxSetCurrentMtx(0, true);
    if (lbl_806DF039)
    {
        fn_801836FC(0);
        fn_801832F4(0, lbl_806E1B00, 1);
        gxSetTevColourOp(0, 0, 0, 0, true, 0);
        gxSetTevColourOp(2, 0, 0, 0, true, 0);
        if (lbl_806E1AF8)
        {
            gxSetTevColourOp(5, 0, 0, 0, true, 0);
            gxSetTexCoordGen(4, 1, 8, 60);
        }
    }
    gxSetTexCoordGen(3, 1, 7, 60);
}

template <>
void GXMaterialProgramImpl<GXMaterialProgram_802997B8>::Prepare(
    const glModelPacket* packet)
{
    fn_802CC978(this, packet, *(unsigned long*)packet->unknown20);
}

struct FloatColour_8028ED18
{
    float c[4];
};

static inline GXColor ConvertColour_8028ED18(
    const FloatColour_8028ED18& source)
{
    GXColor colour;
    colour.r = (unsigned char)(source.c[0] * 255.0f);
    colour.g = (unsigned char)(source.c[1] * 255.0f);
    colour.b = (unsigned char)(source.c[2] * 255.0f);
    colour.a = (unsigned char)(source.c[3] * 255.0f);
    return colour;
}

template <>
void GXMaterialProgramImpl<GXMaterialProgram_802997B8>::Draw(
    const glModelPacket* packet)
{
    if (!lbl_806DF038)
        return;

    GXMaterialProgramParameters_802997B8* parameters = (GXMaterialProgramParameters_802997B8*)packet->unknown20;
    float value36 = lbl_806DF040;
    if (value36 == 1.0f)
        value36 = parameters->value36;
    float value32 = lbl_806DF03C;
    if (value32 == 1.0f)
        value32 = parameters->value32;
    int textureIndex = lbl_806DF048;
    if (textureIndex < 0)
        textureIndex = parameters->value40;
    if (value36 == 0.0f)
        return;

    FloatColour_8028ED18 source32 = { { value32, value32, value32, value32 } };
    GXColor colour32 = ConvertColour_8028ED18(source32);
    GXSetTevKColor(GX_KCOLOR0, colour32);
    FloatColour_8028ED18 source36 = { { value36, value36, value36, value36 } };
    GXColor colour36 = ConvertColour_8028ED18(source36);
    GXSetTevKColor(GX_KCOLOR1, colour36);

    static_cast<GXMaterialProgram_802997B8*>(this)->BindVertexArrays(packet);
    static_cast<GXMaterialProgram_802997B8*>(this)->BindParameters(packet);

    unsigned long texture = lbl_8057AFC0[textureIndex];
    if (lbl_806E1B04 != texture)
    {
        UnidentifiedTextureState textureState;
        textureState.texture = texture;
        textureState.textureIndex = 0xFFFF;
        textureState.flags = 3;
        textureState.unknown07 = 0;
        fn_8036BE88(3, &textureState);
        lbl_806E1B04 = texture;
    }

    nlMatrix4 model;
    nlMatrix4 modelview;
    glGetMatrix(packet->matrix, model);
    nlMultMatrices(modelview, model, lbl_8057AF80);

    if (lbl_806E1AFC != packet->matrix)
    {
        Mtx source;
        Mtx inverse;
        glxCopyMatrix(source, modelview);
        PSMTXInvXpose(source, inverse);
        GXLoadTexMtxImm(inverse, 30, GX_MTX3x4);
        lbl_806E1AFC = packet->matrix;
    }

    if (*(unsigned long*)packet->unknown28 == 0)
    {
        fn_8036D7EC(parameters->matrices, parameters->matricesSize / 48, &modelview, 0);
    }
    else
    {
        fn_8036D774(&modelview);
    }

    if (lbl_806DF039 && value36 != 1.0f)
    {
        gxSaveZMode();
        if (lbl_806DF044)
        {
            bool colourUpdate = gxSetColourUpdate(false);
            GXCallDisplayList(
                packet->displayList->list, packet->displayList->size);
            gxSetColourUpdate(colourUpdate);
            gxSetZMode(true, GX_EQUAL, true);
        }
        gxSaveBlendMode();
        gxSetBlendMode(true, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, false);
        GXSetTevKAlphaSel(GX_TEVSTAGE0, GX_TEV_KASEL_K1_A);
        gxSetTevAlphaIn(0, 7, 7, 7, 6);
        GXCallDisplayList(packet->displayList->list, packet->displayList->size);
        gxRestoreBlendMode();
        gxRestoreZMode();
    }
    else
    {
        GXCallDisplayList(packet->displayList->list, packet->displayList->size);
    }

    gxSetCurrentMtx(0, true);
}
