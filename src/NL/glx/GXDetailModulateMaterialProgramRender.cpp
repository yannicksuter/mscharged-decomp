#include <revolution/gx.h>
#include "Game/GameObjectLighting.h"
#include "NL/gl/glMaterialParameters.h"

#include "NL/gl/glView.h"
#include "NL/glx/GXDetailModulateMaterialProgram.h"
#include "NL/glx/glxTexture.h"
#include "NL/glx/glxGX.h"
#include "NL/glx/glxDisplayList.h"
#include "NL/nlMath.h"
#include "Game/UnidentifiedStaticStorage.h"

enum DetailModulateLightingMode
{
    LIGHTING_NONE,
    LIGHTING_VERTEX,
    LIGHTING_VERTEX_DOUBLE,
    LIGHTING_TEXTURE,
    LIGHTING_UNCONFIGURED
};

bool gDetailModulateEnabled = true;
bool gDetailModulateShadowsEnabled = true;

nlMatrix4 sDetailModulateViewMatrix;
bool sDetailModulateShadowsActive;
int sDetailModulateLightCount;
int sDetailModulateLightingMode;

extern "C" void glxConfigureDetailModulateLighting(int mode, GXDetailModulateMaterialProgram*)
{
    sDetailModulateLightingMode = mode;
    gxSetNumChans(1);

    if (mode != LIGHTING_NONE)
        SetGameObjectLightingEnabled(1, sDetailModulateLightCount, 1);

    if (mode == LIGHTING_TEXTURE)
    {
        gxSetNumTexGens(3);
        gxSetNumTevStages(4);

        GXTevScale colourScale = ShouldDoubleGameObjectLighting() ? GX_CS_SCALE_2 : GX_CS_SCALE_1;
        gxSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD2, GX_TEXMAP2, GX_COLOR0A0);
        gxSetTevOrder(GX_TEVSTAGE1, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
        gxSetTevOrder(GX_TEVSTAGE2, GX_TEXCOORD1, GX_TEXMAP1, GX_COLOR_NULL);
        gxSetTevOrder(GX_TEVSTAGE3, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR_NULL);
        gxSetTexCoordGen(GX_TEXCOORD2, GX_TG_SRTG, GX_TG_COLOR0, GX_IDENTITY);
        gxSetTevColourOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, colourScale, true, GX_TEVREG0);
        GXSetTevKColorSel(GX_TEVSTAGE2, GX_TEV_KCSEL_K0);

        gxSetTevColourIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ONE, GX_CC_TEXC, GX_CC_ZERO);
        gxSetTevColourIn(GX_TEVSTAGE1, GX_CC_ZERO, GX_CC_CPREV, GX_CC_TEXC, GX_CC_ZERO);
        gxSetTevColourIn(GX_TEVSTAGE2, GX_CC_ONE, GX_CC_TEXC, GX_CC_KONST, GX_CC_ZERO);
        gxSetTevColourIn(GX_TEVSTAGE3, GX_CC_ZERO, GX_CC_C0, GX_CC_CPREV, GX_CC_ZERO);

        gxSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO);
        gxSetTevAlphaIn(GX_TEVSTAGE1, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_TEXA);
        gxSetTevAlphaIn(GX_TEVSTAGE2, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_APREV);
        gxSetTevAlphaIn(GX_TEVSTAGE3, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_APREV);
    }
    else
    {
        if (mode != LIGHTING_NONE)
            SetGameObjectAmbientLightingEnabled(1);

        gxSetNumTexGens(2);
        gxSetNumTevStages(3);
        gxSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
        gxSetTevOrder(GX_TEVSTAGE1, GX_TEXCOORD1, GX_TEXMAP1, GX_COLOR_NULL);
        gxSetTevOrder(GX_TEVSTAGE2, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR_NULL);
        gxSetTevColourOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, mode == LIGHTING_VERTEX_DOUBLE ? GX_CS_SCALE_2 : GX_CS_SCALE_1, true, GX_TEVREG0);
        GXSetTevKColorSel(GX_TEVSTAGE1, GX_TEV_KCSEL_K0);

        gxSetTevColourIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_RASC, GX_CC_TEXC, GX_CC_ZERO);
        gxSetTevColourIn(GX_TEVSTAGE1, GX_CC_ONE, GX_CC_TEXC, GX_CC_KONST, GX_CC_ZERO);
        gxSetTevColourIn(GX_TEVSTAGE2, GX_CC_ZERO, GX_CC_C0, GX_CC_CPREV, GX_CC_ZERO);

        gxSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_RASA, GX_CA_TEXA, GX_CA_ZERO);
        gxSetTevAlphaIn(GX_TEVSTAGE1, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_APREV);
        gxSetTevAlphaIn(GX_TEVSTAGE2, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_APREV);
    }
}

template <>
void GXMaterialProgramImpl<GXDetailModulateMaterialProgram>::Activate(GLView* view)
{
    sDetailModulateLightingMode = LIGHTING_UNCONFIGURED;
    static_cast<GXDetailModulateMaterialProgram*>(this)->ConfigureVertexFormat(true);
    view->m_Interface->GetViewMatrix(sDetailModulateViewMatrix);
    sDetailModulateLightCount = GetGameObjectLightCount(0, 1);
    LoadGameObjectLights(sDetailModulateLightCount, view, 0);
}

template <>
void GXMaterialProgramImpl<GXDetailModulateMaterialProgram>::Deactivate()
{
    if (sDetailModulateShadowsActive)
    {
        RestoreGameObjectShadowLighting();
        sDetailModulateShadowsActive = false;
    }

    if (sDetailModulateLightingMode == LIGHTING_TEXTURE)
    {
        gxSetTevColourOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, true, GX_TEVPREV);
        gxSetTexCoordGen(GX_TEXCOORD2, GX_TG_MTX2x4, GX_TG_TEX1, GX_IDENTITY);
    }
    else
    {
        SetGameObjectAmbientLightingEnabled(0);
        gxSetTevColourOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, true, GX_TEVPREV);
    }

    SetGameObjectLightingEnabled(0, sDetailModulateLightCount, 1);
}

template <>
void GXMaterialProgramImpl<GXDetailModulateMaterialProgram>::Prepare(
    glModelPacket* packet)
{
    glSetMaterialTextureAlphaState(this, packet, static_cast<const GXDetailModulateParameters*>(packet->materialParameters)->diffuseTexture.texture);
}

template <>
void GXMaterialProgramImpl<GXDetailModulateMaterialProgram>::Draw(
    const glModelPacket* packet)
{
    if (!gDetailModulateEnabled)
        return;

    static_cast<GXDetailModulateMaterialProgram*>(this)->BindVertexArrays(packet);
    static_cast<GXDetailModulateMaterialProgram*>(this)->BindParameters(packet);

    int mode;
    if (static_cast<const GXDetailModulateParameters*>(packet->materialParameters)->lightingEnabled == 0
        || IsGameObjectLightingEnabled() == 0)
    {
        mode = LIGHTING_NONE;
    }
    else
    {
        if (ShouldUseGameObjectLightTexture(0) != 0)
            mode = LIGHTING_TEXTURE;
        else
        {
            int doubleLighting = ShouldDoubleGameObjectLighting();
            mode = LIGHTING_VERTEX;
            if (doubleLighting != 0)
                mode = LIGHTING_VERTEX_DOUBLE;
        }
    }

    if (sDetailModulateLightingMode != mode)
    {
        Deactivate();
        glxConfigureDetailModulateLighting(
            mode, static_cast<GXDetailModulateMaterialProgram*>(this));
    }

    if (sDetailModulateLightingMode == LIGHTING_TEXTURE)
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

    float detailStrength = static_cast<const GXDetailModulateParameters*>(packet->materialParameters)->detailStrength;
    nlFloatColour detailColour = { { detailStrength, detailStrength, detailStrength, detailStrength } };
    nlColour detailColour8;
    ConvertColour(detailColour8, detailColour);
    GXColor detailGXColour = { detailColour8.c[0], detailColour8.c[1], detailColour8.c[2], detailColour8.c[3] };
    GXSetTevKColor(GX_KCOLOR0, detailGXColour);

    SetGameObjectShadowModelMatrix(packet->matrix);

    bool enableShadows = false;
    if (static_cast<const GXDetailModulateParameters*>(packet->materialParameters)->shadowEnabled == 1
        && gDetailModulateShadowsEnabled)
        enableShadows = true;

    if (enableShadows)
    {
        if (!sDetailModulateShadowsActive)
        {
            ApplyGameObjectShadowLighting(0, 0);
            sDetailModulateShadowsActive = true;
        }
    }
    else if (sDetailModulateShadowsActive)
    {
        RestoreGameObjectShadowLighting();
        sDetailModulateShadowsActive = false;
    }

    if (packet->displayList != 0)
        GXCallDisplayList(packet->displayList->list, packet->displayList->size);
    else if (packet->indexBuffer != 0)
        static_cast<GXDetailModulateMaterialProgram*>(this)->DrawIndexed(packet);
    else
        static_cast<GXDetailModulateMaterialProgram*>(this)->DrawDirect(packet);
}
