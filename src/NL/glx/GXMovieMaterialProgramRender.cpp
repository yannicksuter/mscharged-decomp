#include <revolution/gx.h>
#include "NL/gl/glMaterialParameters.h"

#include "NL/gl/glState.h"
#include "NL/gl/glTexture.h"
#include "NL/glx/GXMovieMaterialProgram.h"
#include "NL/glx/glxTexture.h"
#include "NL/glx/glxGX.h"
#include "NL/glx/glxDisplayList.h"
#include "Game/UnidentifiedStaticStorage.h"

bool gMovieYUVEnabled = true;
bool gMovieTintEnabled = true;

unsigned long sMovieTextureY = glGetTexture("movie");
unsigned long sMovieTextureU = glGetTexture("movie_u");
unsigned long sMovieTextureV = glGetTexture("movie_v");
unsigned char sMovieTexturesLoaded;

void glxConfigureMovie()
{
    gxSetNumChans(0);

    if (gMovieYUVEnabled)
    {
        gxSetNumTexGens(2);
        gxSetNumTevStages(4);

        glTextureLoad(sMovieTextureU);
        unsigned long width = glTextureGetWidth();
        unsigned long height = glTextureGetHeight();
        gxSetTexCoordGen(GX_TEXCOORD1, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY);
        GXSetTexCoordScaleManually(GX_TEXCOORD1, GX_TRUE, width, height);

        gxSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD1, GX_TEXMAP1, GX_COLOR_NULL);
        gxSetTevOrder(GX_TEVSTAGE1, GX_TEXCOORD1, GX_TEXMAP2, GX_COLOR_NULL);
        gxSetTevOrder(GX_TEVSTAGE2, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
        gxSetTevOrder(GX_TEVSTAGE3, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR_NULL);

        gxSetTevColourIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_TEXC, GX_CC_KONST, GX_CC_C0);
        gxSetTevColourOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, false, GX_TEVPREV);
        gxSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_TEXA, GX_CA_KONST, GX_CA_A0);
        gxSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_SUB, GX_TB_ZERO, GX_CS_SCALE_1, false, GX_TEVPREV);
        GXSetTevKColorSel(GX_TEVSTAGE0, GX_TEV_KCSEL_K0);
        GXSetTevKAlphaSel(GX_TEVSTAGE0, GX_TEV_KASEL_K0_A);

        gxSetTevColourIn(GX_TEVSTAGE1, GX_CC_ZERO, GX_CC_TEXC, GX_CC_KONST, GX_CC_CPREV);
        gxSetTevColourOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_2, false, GX_TEVPREV);
        gxSetTevAlphaIn(GX_TEVSTAGE1, GX_CA_ZERO, GX_CA_TEXA, GX_CA_KONST, GX_CA_APREV);
        gxSetTevAlphaOp(GX_TEVSTAGE1, GX_TEV_SUB, GX_TB_ZERO, GX_CS_SCALE_1, false, GX_TEVPREV);
        GXSetTevKColorSel(GX_TEVSTAGE1, GX_TEV_KCSEL_K1);
        GXSetTevKAlphaSel(GX_TEVSTAGE1, GX_TEV_KASEL_K1_A);

        gxSetTevColourIn(GX_TEVSTAGE2, GX_CC_ZERO, GX_CC_TEXC, GX_CC_ONE, GX_CC_CPREV);
        gxSetTevColourOp(GX_TEVSTAGE2, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, true, GX_TEVPREV);
        gxSetTevAlphaIn(GX_TEVSTAGE2, GX_CA_TEXA, GX_CA_ZERO, GX_CA_ZERO, GX_CA_APREV);
        gxSetTevAlphaOp(GX_TEVSTAGE2, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, true, GX_TEVPREV);

        gxSetTevColourIn(GX_TEVSTAGE3, GX_CC_APREV, GX_CC_CPREV, GX_CC_KONST, GX_CC_ZERO);
        gxSetTevColourOp(GX_TEVSTAGE3, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, true, GX_TEVPREV);
        gxSetTevAlphaIn(GX_TEVSTAGE3, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO);
        gxSetTevAlphaOp(GX_TEVSTAGE3, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, true, GX_TEVPREV);
        GXSetTevKColorSel(GX_TEVSTAGE3, GX_TEV_KCSEL_K2);

        GXColorS10 yuvOffset = { (short)0xFFA6, 0, (short)0xFF8E, 0x87 };
        GXSetTevColorS10(GX_TEVREG0, yuvOffset);

        GXColor uConversion = { 0x00, 0x00, 0xE2, 0x58 };
        GXSetTevKColor(GX_KCOLOR0, uConversion);
        GXColor vConversion = { 0xB3, 0x00, 0x00, 0xB6 };
        GXSetTevKColor(GX_KCOLOR1, vConversion);
        GXColor rgbSelect = { 0xFF, 0x00, 0xFF, 0x80 };
        GXSetTevKColor(GX_KCOLOR2, rgbSelect);
    }
    else
    {
        gxSetNumTexGens(1);
        gxSetNumTevStages(1);
        gxSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);

        if (gMovieTintEnabled)
        {
            GXSetTevKColorSel(GX_TEVSTAGE0, GX_TEV_KCSEL_K0);
            GXSetTevKAlphaSel(GX_TEVSTAGE0, GX_TEV_KASEL_K0_A);
            gxSetTevColourIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_KONST, GX_CC_TEXC, GX_CC_ZERO);
            gxSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_KONST, GX_CA_TEXA, GX_CA_ZERO);
        }
        else
        {
            gxSetTevColourIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ONE, GX_CC_TEXC, GX_CC_ZERO);
            gxSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_KONST, GX_CA_TEXA, GX_CA_ZERO);
        }
    }
}

template <>
void GXMaterialProgramImpl<GXMovieMaterialProgram>::Activate(GLView*)
{
    static_cast<GXMovieMaterialProgram*>(this)->ConfigureVertexFormat(true);

    int loaded;
    if (gMovieYUVEnabled)
    {
        loaded = false;
        if (glTextureLoad(sMovieTextureY)
            && glTextureLoad(sMovieTextureU)
            && glTextureLoad(sMovieTextureV))
        {
            loaded = true;
        }
        sMovieTexturesLoaded = loaded;
    }
    else
    {
        loaded = glTextureLoad(sMovieTextureY);
        sMovieTexturesLoaded = loaded;
    }

    if ((unsigned char)loaded)
        glxConfigureMovie();
}

template <>
void GXMaterialProgramImpl<GXMovieMaterialProgram>::Deactivate()
{
    if (sMovieTexturesLoaded && gMovieYUVEnabled)
    {
        gxSetTexCoordGen(GX_TEXCOORD1, GX_TG_MTX2x4, GX_TG_TEX1, GX_IDENTITY);
        GXSetTexCoordScaleManually(GX_TEXCOORD1, GX_FALSE, 0, 0);

        for (int i = 0; i < 4; ++i)
        {
            gxSetTevColourOp(i, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, true, GX_TEVPREV);
            gxSetTevAlphaOp(i, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, true, GX_TEVPREV);
        }
    }
}

template <>
void GXMaterialProgramImpl<GXMovieMaterialProgram>::Prepare(
    glModelPacket* packet)
{
    glSetMaterialTextureAlphaState(this, packet, static_cast<const GXMovieParameters*>(packet->materialParameters)->texture.texture);
}

static inline GXColor MakeMovieTintColour(float r, float g, float b, float a)
{
    GXColor colour;
    colour.r = (unsigned char)(r * 255.0f);
    colour.g = (unsigned char)(g * 255.0f);
    colour.b = (unsigned char)(b * 255.0f);
    colour.a = (unsigned char)(a * 255.0f);
    return colour;
}

template <>
void GXMaterialProgramImpl<GXMovieMaterialProgram>::Draw(
    const glModelPacket* packet)
{
    if (!sMovieTexturesLoaded)
        return;

    if (gMovieYUVEnabled)
    {
        glTextureBinding textureU;
        textureU.texture = sMovieTextureU;
        textureU.unknown07 = 0;
        textureU.textureIndex = 0xFFFF;
        textureU.flags = 0;
        textureU.SetWrapS(true);
        textureU.SetWrapT(true);
        glx_BindTexture(1, &textureU);

        glTextureBinding textureV;
        textureV.texture = sMovieTextureV;
        textureV.textureIndex = 0xFFFF;
        textureV.flags = 0;
        textureV.SetWrapS(true);
        textureV.SetWrapT(true);
        textureV.unknown07 = 0;
        glx_BindTexture(2, &textureV);
    }
    else if (gMovieTintEnabled)
    {
        const nlFloatColour& tint = static_cast<const GXMovieParameters*>(packet->materialParameters)->tint;
        float r = tint.c[0];
        float g = tint.c[1];
        float b = tint.c[2];
        float a = tint.c[3];
        GXSetTevKColor(GX_KCOLOR0, MakeMovieTintColour(r, g, b, a));
    }

    static_cast<GXMovieMaterialProgram*>(this)->BindVertexArrays(packet);

    glTextureBinding textureY;
    textureY.texture = sMovieTextureY;
    textureY.textureIndex = 0xFFFF;
    textureY.flags = 0;
    textureY.unknown07 = 0;
    textureY.SetWrapS(true);
    textureY.SetWrapT(true);
    glx_BindTexture(0, &textureY);

    if (packet->displayList != 0)
        GXCallDisplayList(packet->displayList->list, packet->displayList->size);
    else if (packet->indexBuffer != 0)
        static_cast<GXMovieMaterialProgram*>(this)->DrawIndexed(packet);
    else
        static_cast<GXMovieMaterialProgram*>(this)->DrawDirect(packet);
}
