#include <revolution/gx.h>

#include "NL/gl/glState.h"
#include "NL/gl/glTexture.h"
#include "NL/glx/GXMaterialProgram.h"
#include "NL/glx/glxGX.h"
#include "NL/glx/glxDisplayList.h"

extern "C"
{
    void fn_803A5AA4(
        int textureUnit, int unknown, unsigned short width,
        unsigned short height);
}

bool lbl_806DF0C8 = true;
bool lbl_806DF0C9 = true;

unsigned long lbl_806E1BE8 = glGetTexture("movie");
unsigned long lbl_806E1BEC = glGetTexture("movie_u");
unsigned long lbl_806E1BF0 = glGetTexture("movie_v");
unsigned char lbl_806E1BF4;

void fn_802977EC()
{
    gxSetNumChans(0);

    if (lbl_806DF0C8)
    {
        gxSetNumTexGens(2);
        gxSetNumTevStages(4);

        glTextureLoad(lbl_806E1BEC);
        unsigned long width = glTextureGetWidth();
        unsigned long height = glTextureGetHeight();
        gxSetTexCoordGen(1, 1, 4, 60);
        fn_803A5AA4(1, 1, width, height);

        gxSetTevOrder(0, 1, 1, 255);
        gxSetTevOrder(1, 1, 2, 255);
        gxSetTevOrder(2, 0, 0, 255);
        gxSetTevOrder(3, 255, 255, 255);

        gxSetTevColourIn(0, 15, 8, 14, 2);
        gxSetTevColourOp(0, 0, 0, 0, false, 0);
        gxSetTevAlphaIn(0, 7, 4, 6, 1);
        gxSetTevAlphaOp(0, 1, 0, 0, false, 0);
        GXSetTevKColorSel(GX_TEVSTAGE0, GX_TEV_KCSEL_K0);
        GXSetTevKAlphaSel(GX_TEVSTAGE0, GX_TEV_KASEL_K0_A);

        gxSetTevColourIn(1, 15, 8, 14, 0);
        gxSetTevColourOp(1, 0, 0, 1, false, 0);
        gxSetTevAlphaIn(1, 7, 4, 6, 0);
        gxSetTevAlphaOp(1, 1, 0, 0, false, 0);
        GXSetTevKColorSel(GX_TEVSTAGE1, GX_TEV_KCSEL_K1);
        GXSetTevKAlphaSel(GX_TEVSTAGE1, GX_TEV_KASEL_K1_A);

        gxSetTevColourIn(2, 15, 8, 12, 0);
        gxSetTevColourOp(2, 0, 0, 0, true, 0);
        gxSetTevAlphaIn(2, 4, 7, 7, 0);
        gxSetTevAlphaOp(2, 0, 0, 0, true, 0);

        gxSetTevColourIn(3, 1, 0, 14, 15);
        gxSetTevColourOp(3, 0, 0, 0, true, 0);
        gxSetTevAlphaIn(3, 7, 7, 7, 7);
        gxSetTevAlphaOp(3, 0, 0, 0, true, 0);
        GXSetTevKColorSel(GX_TEVSTAGE3, GX_TEV_KCSEL_K2);

        GXColorS10 movieColour = { (short)0xFFA6, 0, (short)0xFF8E, 0x87 };
        GXSetTevColorS10(GX_TEVREG0, movieColour);

        GXColor colour0 = { 0x00, 0x00, 0xE2, 0x58 };
        GXSetTevKColor(GX_KCOLOR0, colour0);
        GXColor colour1 = { 0xB3, 0x00, 0x00, 0xB6 };
        GXSetTevKColor(GX_KCOLOR1, colour1);
        GXColor colour2 = { 0xFF, 0x00, 0xFF, 0x80 };
        GXSetTevKColor(GX_KCOLOR2, colour2);
    }
    else
    {
        gxSetNumTexGens(1);
        gxSetNumTevStages(1);
        gxSetTevOrder(0, 0, 0, 255);

        if (lbl_806DF0C9)
        {
            GXSetTevKColorSel(GX_TEVSTAGE0, GX_TEV_KCSEL_K0);
            GXSetTevKAlphaSel(GX_TEVSTAGE0, GX_TEV_KASEL_K0_A);
            gxSetTevColourIn(0, 15, 14, 8, 15);
            gxSetTevAlphaIn(0, 7, 6, 4, 7);
        }
        else
        {
            gxSetTevColourIn(0, 15, 12, 8, 15);
            gxSetTevAlphaIn(0, 7, 6, 4, 7);
        }
    }
}

template <>
void GXMaterialProgramImpl<GXMaterialProgram_802A01CC>::Activate(GLView*)
{
    static_cast<GXMaterialProgram_802A01CC*>(this)->ConfigureVertexFormat(true);

    int loaded;
    if (lbl_806DF0C8)
    {
        loaded = false;
        if (glTextureLoad(lbl_806E1BE8)
            && glTextureLoad(lbl_806E1BEC)
            && glTextureLoad(lbl_806E1BF0))
        {
            loaded = true;
        }
        lbl_806E1BF4 = loaded;
    }
    else
    {
        loaded = glTextureLoad(lbl_806E1BE8);
        lbl_806E1BF4 = loaded;
    }

    if ((unsigned char)loaded)
        fn_802977EC();
}

template <>
void GXMaterialProgramImpl<GXMaterialProgram_802A01CC>::Deactivate()
{
    if (lbl_806E1BF4 && lbl_806DF0C8)
    {
        gxSetTexCoordGen(1, 1, 5, 60);
        fn_803A5AA4(1, 0, 0, 0);

        for (int i = 0; i < 4; ++i)
        {
            gxSetTevColourOp(i, 0, 0, 0, true, 0);
            gxSetTevAlphaOp(i, 0, 0, 0, true, 0);
        }
    }
}

template <>
void GXMaterialProgramImpl<GXMaterialProgram_802A01CC>::Prepare(
    const glModelPacket* packet)
{
    fn_802CC978(this, packet, *(unsigned long*)packet->unknown20);
}

static inline GXColor makeColor(const float* values)
{
    GXColor colour;
    colour.r = (unsigned char)(values[0] * 255.0f);
    colour.g = (unsigned char)(values[1] * 255.0f);
    colour.b = (unsigned char)(values[2] * 255.0f);
    colour.a = (unsigned char)(values[3] * 255.0f);
    return colour;
}

template <>
void GXMaterialProgramImpl<GXMaterialProgram_802A01CC>::Draw(
    const glModelPacket* packet)
{
    if (!lbl_806E1BF4)
        return;

    if (lbl_806DF0C8)
    {
        UnidentifiedTextureState textureU;
        textureU.texture = lbl_806E1BEC;
        textureU.textureIndex = 0xFFFF;
        textureU.flags = 0;
        textureU.unknown07 = 0;
        textureU.SetWrapS(true);
        textureU.SetWrapT(true);
        fn_8036BE88(1, &textureU);

        UnidentifiedTextureState textureV;
        textureV.texture = lbl_806E1BF0;
        textureV.textureIndex = 0xFFFF;
        textureV.flags = 0;
        textureV.unknown07 = 0;
        textureV.SetWrapS(true);
        textureV.SetWrapT(true);
        fn_8036BE88(2, &textureV);
    }
    else if (lbl_806DF0C9)
    {
        float* colour = (float*)((unsigned char*)packet->unknown20 + 8);
        GXSetTevKColor(GX_KCOLOR0, makeColor(colour));
    }

    static_cast<GXMaterialProgram_802A01CC*>(this)->BindVertexArrays(packet);

    UnidentifiedTextureState textureY;
    textureY.texture = lbl_806E1BE8;
    textureY.textureIndex = 0xFFFF;
    textureY.flags = 0;
    textureY.unknown07 = 0;
    textureY.SetWrapS(true);
    textureY.SetWrapT(true);
    fn_8036BE88(0, &textureY);

    if (packet->displayList != 0)
        GXCallDisplayList(packet->displayList->list, packet->displayList->size);
    else if (packet->indexBuffer != 0)
        static_cast<GXMaterialProgram_802A01CC*>(this)->DrawIndexed(packet);
    else
        static_cast<GXMaterialProgram_802A01CC*>(this)->DrawDirect(packet);
}
