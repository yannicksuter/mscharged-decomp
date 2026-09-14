#include <revolution/gx.h>
#include "Game/GameObjectLighting.h"
#include "NL/gl/glMaterialParameters.h"

#include "NL/gl/glView.h"
#include "NL/glx/GXSpecularDetailBlendMaterialProgram.h"
#include "NL/glx/glxGX.h"
#include "NL/glx/glxGXColour.h"
#include "NL/glx/glxDisplayList.h"
#include "NL/nlMath.h"
#include "Game/UnidentifiedStaticStorage.h"

enum SpecularDetailBlendLightingMode
{
    DIFFUSE_LIGHTING_DISABLED,
    DIFFUSE_LIGHTING_ENABLED,
    LIGHTING_UNCONFIGURED
};

bool gSpecularDetailBlendEnabled = true;
bool gSpecularDetailBlendShadowsEnabled = true;

nlMatrix4 sSpecularDetailBlendViewMatrix;
bool sSpecularDetailBlendShadowsActive;
int sSpecularDetailBlendLightCount;
float sSpecularDetailBlendSpecularExponent;
int sSpecularDetailBlendLightingMode;

extern "C" void glxConfigureSpecularDetailBlendLighting(int mode)
{
    sSpecularDetailBlendLightingMode = mode;
    SetGameObjectSpecularLightingEnabled(1, sSpecularDetailBlendLightCount);

    if (mode == DIFFUSE_LIGHTING_ENABLED)
    {
        SetGameObjectAmbientLightingEnabled(1);
        SetGameObjectLightingEnabled(1, sSpecularDetailBlendLightCount, 1);
    }

    gxSetNumChans(2);
    gxSetNumTexGens(4);
    gxSetNumTevStages(8);

    gxSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD2, GX_TEXMAP2, GX_COLOR_NULL);
    gxSetTevOrder(GX_TEVSTAGE1, GX_TEXCOORD1, GX_TEXMAP1, GX_COLOR_NULL);
    gxSetTevOrder(GX_TEVSTAGE2, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
    gxSetTevOrder(GX_TEVSTAGE3, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR0A0);
    gxSetTevOrder(GX_TEVSTAGE4, GX_TEXCOORD3, GX_TEXMAP3, GX_COLOR_NULL);
    gxSetTevOrder(GX_TEVSTAGE5, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR1A1);
    gxSetTevOrder(GX_TEVSTAGE6, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR_NULL);
    gxSetTevOrder(GX_TEVSTAGE7, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR_NULL);

    gxSetTevColourOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, true, GX_TEVREG0);
    gxSetTevColourOp(GX_TEVSTAGE3, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, true, GX_TEVREG1);
    GXSetTevKColorSel(GX_TEVSTAGE0, GX_TEV_KCSEL_K0);
    GXSetTevKColorSel(GX_TEVSTAGE4, GX_TEV_KCSEL_K1);
    GXSetTevKColorSel(GX_TEVSTAGE6, GX_TEV_KCSEL_K2);

    gxSetTevColourIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_KONST, GX_CC_TEXC, GX_CC_ZERO);
    gxSetTevColourIn(GX_TEVSTAGE1, GX_CC_ZERO, GX_CC_TEXC, GX_CC_C0, GX_CC_ZERO);
    gxSetTevColourIn(GX_TEVSTAGE2, GX_CC_TEXC, GX_CC_ZERO, GX_CC_C0, GX_CC_CPREV);
    gxSetTevColourIn(GX_TEVSTAGE3, GX_CC_ZERO, GX_CC_CPREV, GX_CC_RASC, GX_CC_ZERO);
    gxSetTevColourIn(GX_TEVSTAGE4, GX_CC_ZERO, GX_CC_KONST, GX_CC_TEXC, GX_CC_ZERO);
    gxSetTevColourIn(GX_TEVSTAGE5, GX_CC_ZERO, GX_CC_CPREV, GX_CC_RASC, GX_CC_ZERO);
    gxSetTevColourIn(GX_TEVSTAGE6, GX_CC_ZERO, GX_CC_CPREV, GX_CC_KONST, GX_CC_ZERO);
    gxSetTevColourIn(GX_TEVSTAGE7, GX_CC_ZERO, GX_CC_ONE, GX_CC_C1, GX_CC_CPREV);

    gxSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO);
    gxSetTevAlphaIn(GX_TEVSTAGE1, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO);
    gxSetTevAlphaIn(GX_TEVSTAGE2, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_TEXA);
    gxSetTevAlphaIn(GX_TEVSTAGE3, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_APREV);
    gxSetTevAlphaIn(GX_TEVSTAGE4, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_APREV);
    gxSetTevAlphaIn(GX_TEVSTAGE5, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_APREV);
    gxSetTevAlphaIn(GX_TEVSTAGE6, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_APREV);
    gxSetTevAlphaIn(GX_TEVSTAGE7, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_APREV);
}

template <>
void GXMaterialProgramImpl<GXSpecularDetailBlendMaterialProgram>::Activate(GLView* view)
{
    sSpecularDetailBlendLightingMode = LIGHTING_UNCONFIGURED;
    static_cast<GXSpecularDetailBlendMaterialProgram*>(this)->ConfigureVertexFormat(true);
    view->m_Interface->GetViewMatrix(sSpecularDetailBlendViewMatrix);
    sSpecularDetailBlendLightCount = GetGameObjectLightCount(0, 1);
    LoadGameObjectLights(sSpecularDetailBlendLightCount, view, 0);
    sSpecularDetailBlendSpecularExponent = 0.0f;
}

template <>
void GXMaterialProgramImpl<GXSpecularDetailBlendMaterialProgram>::Deactivate()
{
    if (sSpecularDetailBlendShadowsActive)
    {
        RestoreGameObjectShadowLighting();
        sSpecularDetailBlendShadowsActive = false;
    }

    SetGameObjectAmbientLightingEnabled(0);
    SetGameObjectLightingEnabled(0, sSpecularDetailBlendLightCount, 1);
    SetGameObjectSpecularLightingEnabled(0, sSpecularDetailBlendLightCount);
    gxSetNumChans(1);
    gxSetNumTexGens(1);
    gxSetNumTevStages(1);
    gxSetTevColourOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, true, GX_TEVPREV);
    gxSetTevColourOp(GX_TEVSTAGE3, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, true, GX_TEVPREV);
}

template <>
void GXMaterialProgramImpl<GXSpecularDetailBlendMaterialProgram>::Prepare(
    glModelPacket* packet)
{
    glSetMaterialTextureAlphaState(this, packet, static_cast<const GXSpecularDetailBlendParameters*>(packet->materialParameters)->diffuseTexture.texture);
}

template <>
void GXMaterialProgramImpl<GXSpecularDetailBlendMaterialProgram>::Draw(
    const glModelPacket* packet)
{
    if (!gSpecularDetailBlendEnabled)
        return;

    static_cast<GXSpecularDetailBlendMaterialProgram*>(this)->BindVertexArrays(packet);
    static_cast<GXSpecularDetailBlendMaterialProgram*>(this)->BindParameters(packet);

    int lightIndex;
    int mode;
    if (static_cast<const GXSpecularDetailBlendParameters*>(packet->materialParameters)->lightingEnabled == 0
        || IsGameObjectLightingEnabled() == 0)
        mode = DIFFUSE_LIGHTING_DISABLED;
    else
        mode = DIFFUSE_LIGHTING_ENABLED;

    if (sSpecularDetailBlendLightingMode != mode)
    {
        Deactivate();
        glxConfigureSpecularDetailBlendLighting(mode);
    }

    const GXSpecularDetailBlendParameters* parameters = static_cast<const GXSpecularDetailBlendParameters*>(packet->materialParameters);
    float blendAmount = parameters->blendAmount;
    float detailWeight = 1.0f - blendAmount;
    float specularLevel = parameters->specularLevel;
    float specularExponent = parameters->specularExponent;

    nlFloatColour detailColour = { { detailWeight, detailWeight, detailWeight, detailWeight } };
    gxSetTevKColour(GX_KCOLOR0, detailColour);

    nlFloatColour specularLevelColour = { { specularLevel, specularLevel, specularLevel, specularLevel } };
    gxSetTevKColour(GX_KCOLOR1, specularLevelColour);

    gxSetTevKColour(GX_KCOLOR2, parameters->specularColour);

    if (sSpecularDetailBlendSpecularExponent != specularExponent && specularLevel != 0.0f)
    {
        sSpecularDetailBlendSpecularExponent = specularExponent;
        for (lightIndex = 0; lightIndex < sSpecularDetailBlendLightCount; ++lightIndex)
        {
            GameObjectLight* light = GetGameObjectLight(lightIndex, 0);
            LoadGameObjectSpecularLight(lightIndex, light, specularExponent, sSpecularDetailBlendViewMatrix);
        }
    }

    SetGameObjectShadowModelMatrix(packet->matrix);

    bool enableShadows = false;
    if (static_cast<const GXSpecularDetailBlendParameters*>(packet->materialParameters)->shadowEnabled == 1 && gSpecularDetailBlendShadowsEnabled)
        enableShadows = true;

    if (enableShadows)
    {
        if (!sSpecularDetailBlendShadowsActive)
        {
            ApplyGameObjectShadowLighting(0, 0);
            sSpecularDetailBlendShadowsActive = true;
        }
    }
    else if (sSpecularDetailBlendShadowsActive)
    {
        RestoreGameObjectShadowLighting();
        sSpecularDetailBlendShadowsActive = false;
    }

    if (packet->displayList != 0)
        GXCallDisplayList(packet->displayList->list, packet->displayList->size);
    else if (packet->indexBuffer != 0)
        static_cast<GXSpecularDetailBlendMaterialProgram*>(this)->DrawIndexed(packet);
    else
        static_cast<GXSpecularDetailBlendMaterialProgram*>(this)->DrawDirect(packet);
}
