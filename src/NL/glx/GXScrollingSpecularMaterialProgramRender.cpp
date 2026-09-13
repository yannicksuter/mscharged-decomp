#include <revolution/gx.h>
#include "Game/GameObjectLighting.h"
#include "NL/gl/glMaterialParameters.h"

#include "NL/gl/glView.h"
#include "NL/glx/GXScrollingSpecularMaterialProgram.h"
#include "NL/glx/GXMaterialProgramTextureAnimation.h"
#include "NL/glx/glxTexture.h"
#include "NL/glx/glxGX.h"
#include "NL/glx/glxGXColour.h"
#include "NL/glx/glxDisplayList.h"
#include "NL/nlMath.h"
#include "Game/UnidentifiedStaticStorage.h"

bool gScrollingSpecularEnabled = true;
bool gScrollingSpecularHighlightsEnabled = true;
bool gScrollingSpecularAnimationEnabled = true;
bool gScrollingSpecularShadowsEnabled = true;

enum ScrollingSpecularLightingMode
{
    DIFFUSE_LIGHTING_DISABLED = 0,
    VERTEX_LIGHTING = 1,
    DOUBLED_VERTEX_LIGHTING = 2,
    TEXTURE_LIGHTING = 3,
    LIGHTING_UNCONFIGURED = 4,
};

nlMatrix4 sScrollingSpecularViewMatrix;
bool sScrollingSpecularShadowsActive;
int sScrollingSpecularLightCount;
float sScrollingSpecularExponent;
nlVector2 sScrollingSpecularResetValues;
int sScrollingSpecularLightingMode;

extern "C" void glxConfigureScrollingSpecularLighting(int mode)
{
    sScrollingSpecularLightingMode = mode;
    gxSetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX3x4, GX_TG_TEX0, GX_TEXMTX0);
    SetGameObjectSpecularLightingEnabled(1, sScrollingSpecularLightCount);
    if (mode == DIFFUSE_LIGHTING_DISABLED)
    {
        SetGameObjectLightingEnabled(0, sScrollingSpecularLightCount, 1);
    }
    else
    {
        SetGameObjectLightingEnabled(1, sScrollingSpecularLightCount, 1);
    }

    if (mode == TEXTURE_LIGHTING)
    {
        int diffuseLightingScale = ShouldDoubleGameObjectLighting() != 0;
        gxSetNumChans(2);
        gxSetNumTexGens(3);
        gxSetNumTevStages(5);
        gxSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD1, GX_TEXMAP1, GX_COLOR_NULL);
        gxSetTevOrder(GX_TEVSTAGE1, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR1A1);
        gxSetTevOrder(GX_TEVSTAGE2, GX_TEXCOORD2, GX_TEXMAP2, GX_COLOR0A0);
        gxSetTevOrder(GX_TEVSTAGE3, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
        gxSetTevOrder(GX_TEVSTAGE4, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR_NULL);
        gxSetTexCoordGen(GX_TEXCOORD2, GX_TG_SRTG, GX_TG_COLOR0, GX_IDENTITY);
        gxSetTevColourOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, true, GX_TEVREG0);
        gxSetTevColourOp(GX_TEVSTAGE3, GX_TEV_ADD, GX_TB_ZERO, diffuseLightingScale, true, GX_TEVPREV);
        GXSetTevKColorSel(GX_TEVSTAGE0, GX_TEV_KCSEL_K0);
        GXSetTevKColorSel(GX_TEVSTAGE4, GX_TEV_KCSEL_K1);
        gxSetTevColourIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_KONST, GX_CC_TEXC, GX_CC_ZERO);
        gxSetTevColourIn(GX_TEVSTAGE1, GX_CC_ZERO, GX_CC_CPREV, GX_CC_RASC, GX_CC_ZERO);
        gxSetTevColourIn(GX_TEVSTAGE2, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_TEXC);
        gxSetTevColourIn(GX_TEVSTAGE3, GX_CC_ZERO, GX_CC_CPREV, GX_CC_TEXC, GX_CC_ZERO);
        gxSetTevColourIn(GX_TEVSTAGE4, GX_CC_ZERO, GX_CC_KONST, GX_CC_C0, GX_CC_CPREV);
        gxSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO);
        gxSetTevAlphaIn(GX_TEVSTAGE1, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO);
        gxSetTevAlphaIn(GX_TEVSTAGE2, GX_CA_ZERO, GX_CA_RASA, GX_CA_TEXA, GX_CA_ZERO);
        gxSetTevAlphaIn(GX_TEVSTAGE3, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_APREV);
        gxSetTevAlphaIn(GX_TEVSTAGE4, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_APREV);
    }
    else
    {
        if ((unsigned int)(mode - VERTEX_LIGHTING) <= 1)
            SetGameObjectAmbientLightingEnabled(1);

        if (mode == DOUBLED_VERTEX_LIGHTING)
        {
            gxSetNumChans(2);
            gxSetNumTexGens(2);
            gxSetNumTevStages(5);
            gxSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD1, GX_TEXMAP1, GX_COLOR_NULL);
            gxSetTevOrder(GX_TEVSTAGE1, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR1A1);
            gxSetTevOrder(GX_TEVSTAGE2, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR_NULL);
            gxSetTevOrder(GX_TEVSTAGE3, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
            gxSetTevOrder(GX_TEVSTAGE4, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR_NULL);
            gxSetTevColourOp(GX_TEVSTAGE2, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, true, GX_TEVREG0);
            gxSetTevColourOp(GX_TEVSTAGE3, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_2, true, GX_TEVPREV);
            GXSetTevKColorSel(GX_TEVSTAGE0, GX_TEV_KCSEL_K0);
            GXSetTevKColorSel(GX_TEVSTAGE2, GX_TEV_KCSEL_K1);
            gxSetTevColourIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_KONST, GX_CC_TEXC, GX_CC_ZERO);
            gxSetTevColourIn(GX_TEVSTAGE1, GX_CC_ZERO, GX_CC_CPREV, GX_CC_RASC, GX_CC_ZERO);
            gxSetTevColourIn(GX_TEVSTAGE2, GX_CC_ZERO, GX_CC_CPREV, GX_CC_KONST, GX_CC_ZERO);
            gxSetTevColourIn(GX_TEVSTAGE3, GX_CC_ZERO, GX_CC_RASC, GX_CC_TEXC, GX_CC_ZERO);
            gxSetTevColourIn(GX_TEVSTAGE4, GX_CC_ZERO, GX_CC_ONE, GX_CC_CPREV, GX_CC_C0);
            gxSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO);
            gxSetTevAlphaIn(GX_TEVSTAGE1, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO);
            gxSetTevAlphaIn(GX_TEVSTAGE2, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO);
            gxSetTevAlphaIn(GX_TEVSTAGE3, GX_CA_ZERO, GX_CA_RASA, GX_CA_TEXA, GX_CA_ZERO);
            gxSetTevAlphaIn(GX_TEVSTAGE4, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_APREV);
        }
        else
        {
            gxSetNumChans(2);
            gxSetNumTexGens(2);
            gxSetNumTevStages(4);
            gxSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD1, GX_TEXMAP1, GX_COLOR_NULL);
            gxSetTevOrder(GX_TEVSTAGE1, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR1A1);
            gxSetTevOrder(GX_TEVSTAGE2, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR_NULL);
            gxSetTevOrder(GX_TEVSTAGE3, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
            GXSetTevKColorSel(GX_TEVSTAGE0, GX_TEV_KCSEL_K0);
            GXSetTevKColorSel(GX_TEVSTAGE2, GX_TEV_KCSEL_K1);
            gxSetTevColourIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_KONST, GX_CC_TEXC, GX_CC_ZERO);
            gxSetTevColourIn(GX_TEVSTAGE1, GX_CC_ZERO, GX_CC_CPREV, GX_CC_RASC, GX_CC_ZERO);
            gxSetTevColourIn(GX_TEVSTAGE2, GX_CC_ZERO, GX_CC_CPREV, GX_CC_KONST, GX_CC_ZERO);
            gxSetTevColourIn(GX_TEVSTAGE3, GX_CC_ZERO, GX_CC_RASC, GX_CC_TEXC, GX_CC_CPREV);
            gxSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO);
            gxSetTevAlphaIn(GX_TEVSTAGE1, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO);
            gxSetTevAlphaIn(GX_TEVSTAGE2, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO);
            gxSetTevAlphaIn(GX_TEVSTAGE3, GX_CA_ZERO, GX_CA_RASA, GX_CA_TEXA, GX_CA_ZERO);
        }
    }
}

template <>
void GXMaterialProgramImpl<GXScrollingSpecularMaterialProgram>::Activate(GLView* view)
{
    sScrollingSpecularLightingMode = LIGHTING_UNCONFIGURED;
    sScrollingSpecularResetValues.x = 10000000000.0f;
    sScrollingSpecularResetValues.y = 10000000000.0f;
    static_cast<GXScrollingSpecularMaterialProgram*>(this)->ConfigureVertexFormat(true);
    view->m_Interface->GetViewMatrix(sScrollingSpecularViewMatrix);
    sScrollingSpecularLightCount = GetGameObjectLightCount(0, 1);
    LoadGameObjectLights(sScrollingSpecularLightCount, view, 0);
    sScrollingSpecularExponent = 0.0f;
}

template <>
void GXMaterialProgramImpl<GXScrollingSpecularMaterialProgram>::Deactivate()
{
    if (sScrollingSpecularShadowsActive)
    {
        RestoreGameObjectShadowLighting();
        sScrollingSpecularShadowsActive = false;
    }

    gxSetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY);
    gxSetTexCoordGen(GX_TEXCOORD1, GX_TG_MTX2x4, GX_TG_TEX1, GX_IDENTITY);
    SetGameObjectAmbientLightingEnabled(0);
    gxSetNumChans(1);
    SetGameObjectLightingEnabled(0, sScrollingSpecularLightCount, 1);
    SetGameObjectSpecularLightingEnabled(0, sScrollingSpecularLightCount);

    if (sScrollingSpecularLightingMode == TEXTURE_LIGHTING)
    {
        gxSetTexCoordGen(GX_TEXCOORD2, GX_TG_MTX2x4, GX_TG_TEX2, GX_IDENTITY);
        gxSetTevColourOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, true, GX_TEVPREV);
        gxSetTevColourOp(GX_TEVSTAGE3, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, true, GX_TEVPREV);
    }
    else if (sScrollingSpecularLightingMode == DOUBLED_VERTEX_LIGHTING)
    {
        gxSetTevColourOp(GX_TEVSTAGE2, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, true, GX_TEVPREV);
        gxSetTevColourOp(GX_TEVSTAGE3, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, true, GX_TEVPREV);
    }
}

template <>
void GXMaterialProgramImpl<GXScrollingSpecularMaterialProgram>::Prepare(
    glModelPacket* packet)
{
    glSetMaterialTextureAlphaState(this, packet, static_cast<const GXScrollingSpecularParameters*>(packet->materialParameters)->diffuseTexture.texture);
}

template <>
void GXMaterialProgramImpl<GXScrollingSpecularMaterialProgram>::Draw(
    const glModelPacket* packet)
{
    if (!gScrollingSpecularEnabled)
        return;

    GXScrollingSpecularMaterialProgram* program = static_cast<GXScrollingSpecularMaterialProgram*>(this);
    program->BindVertexArrays(packet);
    program->BindParameters(packet);

    ScrollingSpecularLightingMode mode;
    if (static_cast<const GXScrollingSpecularParameters*>(packet->materialParameters)->lightingEnabled == 0
        || IsGameObjectLightingEnabled() == 0)
    {
        mode = DIFFUSE_LIGHTING_DISABLED;
    }
    else if (ShouldUseGameObjectLightTexture(1) != 0)
    {
        mode = TEXTURE_LIGHTING;
    }
    else
    {
        mode = (ShouldDoubleGameObjectLighting() != 0)
            ? DOUBLED_VERTEX_LIGHTING
            : VERTEX_LIGHTING;
    }

    if (mode != sScrollingSpecularLightingMode)
    {
        Deactivate();
        glxConfigureScrollingSpecularLighting(mode);
    }

    if (sScrollingSpecularLightingMode == TEXTURE_LIGHTING)
    {
        glTextureBinding texture;
        texture.texture = 0;
        texture.textureIndex = 0xFFFF;
        texture.flags = 0;
        texture.unknown07 = 0;
        texture.texture = GetGameObjectLightTexture();
        texture.textureIndex = 0xFFFF;
        texture.SetWrapS(true);
        texture.SetWrapT(true);
        texture.unknown07 = 0;
        glx_BindTexture(2, &texture);
    }

    float specularLevel = gScrollingSpecularHighlightsEnabled
        ? static_cast<const GXScrollingSpecularParameters*>(packet->materialParameters)->specularLevel
        : 0.0f;
    const GXScrollingSpecularParameters* parameters =
        static_cast<const GXScrollingSpecularParameters*>(packet->materialParameters);
    float specularExponent = parameters->specularExponent;
    int scrollSpecularTexture = parameters->scrollSpecularTexture;

    nlFloatColour levelColour = { { specularLevel, specularLevel, specularLevel, specularLevel } };
    gxSetTevKColour(GX_KCOLOR0, levelColour);

    const nlFloatColour& specularColour = parameters->specularColour;
    gxSetTevKColour(GX_KCOLOR1, specularColour);

    nlVector2 scrollSpeed;
    scrollSpeed.x = gScrollingSpecularAnimationEnabled
        ? static_cast<const GXScrollingSpecularParameters*>(packet->materialParameters)->scrollSpeedX
        : 0.0f;
    scrollSpeed.y = gScrollingSpecularAnimationEnabled
        ? static_cast<const GXScrollingSpecularParameters*>(packet->materialParameters)->scrollSpeedY
        : 0.0f;
    glxLoadScrollingTextureMatrix(GX_TEXMTX0, scrollSpeed);

    if (scrollSpecularTexture == 1)
        gxSetTexCoordGen(GX_TEXCOORD1, GX_TG_MTX3x4, GX_TG_TEX1, GX_TEXMTX0);
    else
        gxSetTexCoordGen(GX_TEXCOORD1, GX_TG_MTX2x4, GX_TG_TEX1, GX_IDENTITY);

    if (sScrollingSpecularExponent != specularExponent && specularLevel != 0.0f)
    {
        sScrollingSpecularExponent = specularExponent;
        for (int i = 0; i < sScrollingSpecularLightCount; ++i)
        {
            GameObjectLight* light = GetGameObjectLight(i, 0);
            LoadGameObjectSpecularLight(i, light, specularExponent, sScrollingSpecularViewMatrix);
        }
    }

    SetGameObjectShadowModelMatrix(packet->matrix);

    bool enableShadows = false;
    if (static_cast<const GXScrollingSpecularParameters*>(packet->materialParameters)->shadowEnabled == 1
        && gScrollingSpecularShadowsEnabled)
        enableShadows = true;

    if (enableShadows)
    {
        if (!sScrollingSpecularShadowsActive)
        {
            ApplyGameObjectShadowLighting(0, 0);
            sScrollingSpecularShadowsActive = true;
        }
    }
    else if (sScrollingSpecularShadowsActive)
    {
        RestoreGameObjectShadowLighting();
        sScrollingSpecularShadowsActive = false;
    }

    if (packet->displayList != 0)
        GXCallDisplayList(packet->displayList->list, packet->displayList->size);
    else if (packet->indexBuffer != 0)
        program->DrawIndexed(packet);
    else
        program->DrawDirect(packet);
}
