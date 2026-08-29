#include <revolution/gx.h>

#include <string.h>

#include "NL/gl/glMatrix.h"
#include "NL/gl/glState.h"
#include "NL/gl/glView.h"
#include "NL/glx/GXMaterialProgram.h"
#include "NL/glx/glxGX.h"
#include "NL/glx/glxDisplayList.h"
#include "NL/glx/glxMatrix.h"
#include "NL/nlMath.h"

extern "C"
{
    int fn_801820FC();
    int fn_80182104(int);
    int fn_80182118();
    void* fn_80182240(int, int);
    unsigned long fn_80182EB8();
    void fn_80182ED0(void*, GLView*, int);
    void fn_801832F4(int, void*, int);
    void fn_801836FC(int);
    void fn_801837DC(int, int);
    void fn_80183A98();
    void fn_80183B40(unsigned long matrix);
}

extern Mtx lbl_805243A0;

bool lbl_806DF0A0 = true;
float lbl_806DF0A4 = -0.1f;
bool lbl_806DF0A8 = true;

const Mtx lbl_804E8820 = {
    { 0.0f, 0.0f, 0.0f, 0.5f },
    { 0.0f, 0.0f, 0.0f, 0.5f },
    { 0.0f, 0.0f, 0.0f, 1.0f },
};

nlMatrix4 lbl_8057B2D8;
unsigned long lbl_8057B318[6] = {
    glGetTexture("global/white"),
    glGetTexture("global/fresnel0"),
    glGetTexture("global/fresnel1"),
    glGetTexture("global/fresnel2"),
    glGetTexture("global/fresnel4"),
};

bool lbl_806E1B98;
bool lbl_806E1B99;
unsigned long lbl_806E1B9C;
nlVector2 lbl_806E1BA0;
void* lbl_806E1BA8;
int lbl_806E1BAC;
unsigned long lbl_806E1BB0[2];

extern "C" void fn_802951A4(int mode)
{
    lbl_806E1BAC = mode;
    gxSetNumChans(1);
    GXSetTexCoordGen2((GXTexCoordID)1, (GXTexGenType)0, (GXTexGenSrc)1, GX_TEXMTX0, GX_TRUE, GX_PTTEXMTX0);
    GXSetTexCoordGen2((GXTexCoordID)3, (GXTexGenType)0, (GXTexGenSrc)1, GX_TEXMTX0, GX_TRUE, GX_PTTEXMTX1);

    if (mode != 0)
        fn_801832F4(1, lbl_806E1BA8, 1);
    if ((unsigned int)(mode - 1) <= 1)
        fn_801836FC(1);

    if (mode == 2)
    {
        gxSetNumTexGens(4);
        gxSetNumTevStages(5);
        gxSetTevOrder(0, 2, 2, 255);
        gxSetTevOrder(1, 3, 3, 255);
        gxSetTevOrder(2, 1, 1, 255);
        gxSetTevOrder(3, 0, 0, 4);
        gxSetTevOrder(4, 255, 255, 255);
        GXSetTevKColorSel(GX_TEVSTAGE0, GX_TEV_KCSEL_K0);
        gxSetTevColourOp(2, 0, 0, 0, true, 1);
        gxSetTevColourOp(3, 0, 0, 1, true, 0);
        gxSetTevColourIn(0, 15, 14, 8, 15);
        gxSetTevColourIn(1, 15, 0, 8, 15);
        gxSetTevColourIn(2, 15, 0, 8, 15);
        gxSetTevColourIn(3, 15, 10, 8, 15);
        gxSetTevColourIn(4, 15, 12, 0, 2);
        gxSetTevAlphaIn(0, 7, 7, 7, 7);
        gxSetTevAlphaIn(1, 7, 7, 7, 7);
        gxSetTevAlphaIn(2, 7, 7, 7, 7);
        gxSetTevAlphaIn(3, 7, 7, 7, 4);
        gxSetTevAlphaIn(4, 7, 7, 7, 0);
    }
    else if ((unsigned int)mode <= 1)
    {
        gxSetNumTexGens(4);
        gxSetNumTevStages(4);
        gxSetTevOrder(0, 2, 2, 255);
        gxSetTevOrder(1, 3, 3, 255);
        gxSetTevOrder(2, 1, 1, 255);
        gxSetTevOrder(3, 0, 0, 4);
        GXSetTevKColorSel(GX_TEVSTAGE0, GX_TEV_KCSEL_K0);
        gxSetTevColourIn(0, 15, 14, 8, 15);
        gxSetTevColourIn(1, 15, 0, 8, 15);
        gxSetTevColourIn(2, 15, 0, 8, 15);
        gxSetTevColourIn(3, 15, 10, 8, 0);
        gxSetTevAlphaIn(0, 7, 7, 7, 7);
        gxSetTevAlphaIn(1, 7, 7, 7, 7);
        gxSetTevAlphaIn(2, 7, 7, 7, 7);
        gxSetTevAlphaIn(3, 7, 7, 7, 4);
    }
    else if (mode == 3)
    {
        int useSubtract = fn_80182118() != 0;
        gxSetNumTexGens(5);
        gxSetNumTevStages(6);
        gxSetTevOrder(0, 4, 4, 4);
        gxSetTevOrder(1, 2, 2, 255);
        gxSetTevOrder(2, 3, 3, 255);
        gxSetTevOrder(3, 1, 1, 255);
        gxSetTevOrder(4, 0, 0, 255);
        gxSetTevOrder(5, 255, 255, 255);
        gxSetTexCoordGen(4, 10, 19, 60);
        gxSetTevColourOp(0, 0, 0, 0, true, 1);
        gxSetTevColourOp(3, 0, 0, 0, true, 2);
        gxSetTevColourOp(4, 0, 0, useSubtract, true, 0);
        GXSetTevKColorSel(GX_TEVSTAGE1, GX_TEV_KCSEL_K0);
        gxSetTevColourIn(0, 15, 15, 15, 8);
        gxSetTevColourIn(1, 15, 14, 8, 15);
        gxSetTevColourIn(2, 15, 0, 8, 15);
        gxSetTevColourIn(3, 15, 0, 8, 15);
        gxSetTevColourIn(4, 15, 2, 8, 15);
        gxSetTevColourIn(5, 15, 12, 0, 4);
        gxSetTevAlphaIn(0, 7, 7, 7, 7);
        gxSetTevAlphaIn(1, 7, 7, 7, 7);
        gxSetTevAlphaIn(2, 7, 7, 7, 7);
        gxSetTevAlphaIn(3, 7, 7, 7, 7);
        gxSetTevAlphaIn(4, 7, 7, 7, 4);
        gxSetTevAlphaIn(5, 7, 7, 7, 0);
    }
}

template <>
void GXMaterialProgramImpl<GXMaterialProgram_8029E8F8>::Activate(GLView* view)
{
    lbl_806E1BAC = 4;
    static_cast<GXMaterialProgram_8029E8F8*>(this)->ConfigureVertexFormat(true);
    view->m_Interface->GetViewMatrix(lbl_8057B2D8);
    lbl_806E1B9C = -1;
    lbl_806E1BA0.x = 255.0f;
    lbl_806E1BA0.y = 255.0f;
    lbl_806E1BB0[0] = -1;
    lbl_806E1BA8 = fn_80182240(0, 1);
    fn_80182ED0(lbl_806E1BA8, view, 0);

    UnidentifiedTextureState texture;
    texture.texture = fn_80182EB8();
    texture.textureIndex = 0xFFFF;
    texture.flags = 3;
    texture.unknown07 = 0;
    fn_8036BE88(4, &texture);
    GXLoadTexMtxImm(lbl_805243A0, 67, GX_MTX_3x4);
}

template <>
void GXMaterialProgramImpl<GXMaterialProgram_8029E8F8>::Deactivate()
{
    if (lbl_806E1B99)
    {
        fn_80183A98();
        lbl_806E1B99 = false;
    }

    fn_801836FC(0);
    gxSetTexCoordGen(1, 1, 5, 60);
    gxSetTexCoordGen(3, 1, 7, 60);

    if (lbl_806E1BAC == 3)
    {
        gxSetTexCoordGen(4, 1, 8, 60);
        gxSetTevColourOp(0, 0, 0, 0, true, 0);
        gxSetTevColourOp(2, 0, 0, 0, true, 0);
        gxSetTevColourOp(3, 0, 0, 0, true, 0);
    }
    else if (lbl_806E1BAC == 2)
    {
        gxSetTevColourOp(2, 0, 0, 0, true, 0);
        gxSetTevColourOp(3, 0, 0, 0, true, 0);
    }

    fn_801832F4(0, lbl_806E1BA8, 1);
}

template <>
void GXMaterialProgramImpl<GXMaterialProgram_8029E8F8>::Prepare(
    const glModelPacket* packet)
{
    fn_802CC978(this, packet, *(unsigned long*)packet->unknown20);
}

struct FloatColour_802951A4
{
    float c[4];
};

static inline GXColor ConvertColour_802951A4(
    const FloatColour_802951A4& source)
{
    GXColor colour;
    colour.r = (unsigned char)(source.c[0] * 255.0f);
    colour.g = (unsigned char)(source.c[1] * 255.0f);
    colour.b = (unsigned char)(source.c[2] * 255.0f);
    colour.a = (unsigned char)(source.c[3] * 255.0f);
    return colour;
}

template <>
void GXMaterialProgramImpl<GXMaterialProgram_8029E8F8>::Draw(
    const glModelPacket* packet)
{
    if (!lbl_806DF0A0)
        return;

    GXMaterialProgram_8029E8F8* program = static_cast<GXMaterialProgram_8029E8F8*>(this);
    program->BindVertexArrays(packet);
    program->BindParameters(packet);

    unsigned char* parameters = (unsigned char*)packet->unknown20;
    float colourValue = *(float*)(parameters + 24);
    float scaleX = *(float*)(parameters + 28);
    float scaleY = *(float*)(parameters + 32);
    float textureSelection = *(float*)(parameters + 36);
    if (lbl_806DF0A4 >= 0.0f)
        textureSelection = lbl_806DF0A4;

    FloatColour_802951A4 colour = {
        { colourValue, colourValue, colourValue, colourValue }
    };
    GXColor gxColour = ConvertColour_802951A4(colour);
    GXSetTevKColor(GX_KCOLOR0, gxColour);

    if (lbl_806E1BA0.x != scaleX || lbl_806E1BA0.y != scaleY)
    {
        Mtx textureMatrix;
        memcpy(textureMatrix, lbl_804E8820, sizeof(Mtx));
        textureMatrix[0][0] = 0.5f * scaleX;
        textureMatrix[1][1] = -0.5f * scaleY;
        GXLoadTexMtxImm(textureMatrix, 64, GX_MTX_3x4);
        lbl_806E1BA0.x = scaleX;
        lbl_806E1BA0.y = scaleY;
    }

    int mode;
    if (*(int*)(parameters + 40) == 0 || fn_801820FC() == 0)
    {
        mode = 0;
    }
    else if (fn_80182104(1) != 0)
    {
        mode = 3;
    }
    else
    {
        mode = 1;
        if (fn_80182118() != 0)
            mode = 2;
    }

    if (lbl_806E1BAC != mode)
    {
        Deactivate();
        fn_802951A4(mode);
    }

    unsigned long texture;
    if (lbl_806E1B98 || textureSelection < 0.2f)
        texture = lbl_8057B318[0];
    else if (textureSelection < 0.95f)
        texture = lbl_8057B318[1];
    else if (textureSelection < 1.666f)
        texture = lbl_8057B318[2];
    else if (textureSelection < 3.25f)
        texture = lbl_8057B318[3];
    else
        texture = lbl_8057B318[4];

    if (lbl_806E1BB0[0] != texture)
    {
        UnidentifiedTextureState textureState;
        textureState.texture = texture;
        textureState.textureIndex = 0xFFFF;
        textureState.flags = 3;
        textureState.unknown07 = 0;
        fn_8036BE88(3, &textureState);
        lbl_806E1BB0[0] = texture;
    }

    if (lbl_806E1B9C != packet->matrix)
    {
        lbl_806E1B9C = packet->matrix;
        nlMatrix4 model;
        nlMatrix4 modelview;
        Mtx source;
        Mtx inverse;
        glGetMatrix(packet->matrix, model);
        nlMultMatrices(modelview, model, lbl_8057B2D8);
        glxCopyMatrix(source, modelview);
        PSMTXInvXpose(source, inverse);
        GXLoadTexMtxImm(inverse, 30, GX_MTX_3x4);
    }

    fn_80183B40(packet->matrix);

    bool enableState = false;
    if (*(int*)(parameters + 44) == 1 && lbl_806DF0A8)
        enableState = true;

    if (enableState)
    {
        if (!lbl_806E1B99)
        {
            fn_801837DC(0, 0);
            lbl_806E1B99 = true;
        }
    }
    else if (lbl_806E1B99)
    {
        fn_80183A98();
        lbl_806E1B99 = false;
    }

    if (packet->displayList != 0)
        GXCallDisplayList(packet->displayList->list, packet->displayList->size);
    else if (packet->indexBuffer != 0)
        program->DrawIndexed(packet);
    else
        program->DrawDirect(packet);
}
