#include "NL/glx/GXMaterialProgram.h"
#include "NL/glx/GXMaterialProgramRegistry.h"
#include "NL/nlMemory.h"
#include "Game/UnidentifiedStaticStorage.h"

void glInitMaterialPrograms()
{
    new (nlMalloc(sizeof(GXUnlitTextureMaterialProgram), 8, false))
        GXUnlitTextureMaterialProgram;
    new (nlMalloc(sizeof(GXConstantColourMaterialProgram), 8, false))
        GXConstantColourMaterialProgram;
    new (nlMalloc(sizeof(GXTextureColourAddMaterialProgram), 8, false))
        GXTextureColourAddMaterialProgram;
    new (nlMalloc(sizeof(GXVertexColourTextureMaterialProgram), 8, false))
        GXVertexColourTextureMaterialProgram;
    new (nlMalloc(sizeof(GXCompactColourMaterialProgram), 8, false))
        GXCompactColourMaterialProgram;
    new (nlMalloc(sizeof(GXVertexColourMaterialProgram), 8, false))
        GXVertexColourMaterialProgram;
    new (nlMalloc(sizeof(GXSkinnedUnlitTextureMaterialProgram), 8, false))
        GXSkinnedUnlitTextureMaterialProgram;
    new (nlMalloc(sizeof(GXTextureBlendMaterialProgram), 8, false))
        GXTextureBlendMaterialProgram;
    new (nlMalloc(sizeof(GXVertexColourDetailBlendMaterialProgram), 8, false))
        GXVertexColourDetailBlendMaterialProgram;
    new (nlMalloc(sizeof(GXThreeLightDiffuseMaterialProgram), 8, false))
        GXThreeLightDiffuseMaterialProgram;
    new (nlMalloc(sizeof(GXFixedLightMaterialProgram), 8, false))
        GXFixedLightMaterialProgram;
    new (nlMalloc(sizeof(GXFloatTexturedColourMaterialProgram), 8, false))
        GXFloatTexturedColourMaterialProgram;
    new (nlMalloc(sizeof(GXRedColourMaterialProgram), 8, false))
        GXRedColourMaterialProgram;
    new (nlMalloc(sizeof(GXScissoredVertexColourTextureMaterialProgram), 8, false))
        GXScissoredVertexColourTextureMaterialProgram;
    new (nlMalloc(sizeof(GXShadowVolumeMaterialProgram), 8, false))
        GXShadowVolumeMaterialProgram;
    new (nlMalloc(sizeof(GXSkinnedMultiLightMaterialProgram), 8, false))
        GXSkinnedMultiLightMaterialProgram;
    new (nlMalloc(sizeof(GXCharacterSkinCustomMaterialProgram), 8, false))
        GXCharacterSkinCustomMaterialProgram;
    new (nlMalloc(sizeof(GXSpecularMaterialProgram), 8, false))
        GXSpecularMaterialProgram;
    new (nlMalloc(sizeof(GXColourFresnelMaterialProgram), 8, false))
        GXColourFresnelMaterialProgram;
    new (nlMalloc(sizeof(GXSpecularFresnelMaterialProgram), 8, false))
        GXSpecularFresnelMaterialProgram;
    new (nlMalloc(sizeof(GXScrollingDiffuseMaterialProgram), 8, false))
        GXScrollingDiffuseMaterialProgram;
    new (nlMalloc(sizeof(GXSpecularLookupMaterialProgram), 8, false))
        GXSpecularLookupMaterialProgram;
    new (nlMalloc(sizeof(GXMovieMaterialProgram), 8, false))
        GXMovieMaterialProgram;
    new (nlMalloc(sizeof(GXMaskedDetailBlendMaterialProgram), 8, false))
        GXMaskedDetailBlendMaterialProgram;
    new (nlMalloc(sizeof(GXMaskedSpecularFresnelMaterialProgram), 8, false))
        GXMaskedSpecularFresnelMaterialProgram;
    new (nlMalloc(sizeof(GXScrollingSpecularMaterialProgram), 8, false))
        GXScrollingSpecularMaterialProgram;
    new (nlMalloc(sizeof(GXDetailModulateMaterialProgram), 8, false))
        GXDetailModulateMaterialProgram;
    new (nlMalloc(sizeof(GXShadowedDetailBlendMaterialProgram), 8, false))
        GXShadowedDetailBlendMaterialProgram;
    new (nlMalloc(sizeof(GXSpecularDetailBlendMaterialProgram), 8, false))
        GXSpecularDetailBlendMaterialProgram;
    new (nlMalloc(sizeof(GXScrollingMaskedDetailBlendMaterialProgram), 8, false))
        GXScrollingMaskedDetailBlendMaterialProgram;
    new (nlMalloc(sizeof(GXScrollingShadowedDetailBlendMaterialProgram), 8, false))
        GXScrollingShadowedDetailBlendMaterialProgram;
    new (nlMalloc(sizeof(GXBlackTextureAlphaMaterialProgram), 8, false))
        GXBlackTextureAlphaMaterialProgram;
    new (nlMalloc(sizeof(GXMegaDiffuseMaterialProgram), 8, false))
        GXMegaDiffuseMaterialProgram;
    new (nlMalloc(sizeof(GXMegaSpecularFresnelMaterialProgram), 8, false))
        GXMegaSpecularFresnelMaterialProgram;
    new (nlMalloc(sizeof(GXCharacterDamageMaterialProgram), 8, false))
        GXCharacterDamageMaterialProgram;
    new (nlMalloc(sizeof(GXMegaSpecularMaterialProgram), 8, false))
        GXMegaSpecularMaterialProgram;
    new (nlMalloc(sizeof(GXFourTextureAddMaterialProgram), 8, false))
        GXFourTextureAddMaterialProgram;
    new (nlMalloc(sizeof(GXShadowedDiffuseMaterialProgram), 8, false))
        GXShadowedDiffuseMaterialProgram;
    new (nlMalloc(sizeof(GXCameraScrolledOverlayMaterialProgram), 8, false))
        GXCameraScrolledOverlayMaterialProgram;
    new (nlMalloc(sizeof(GXScrollingCameraOverlayMaterialProgram), 8, false))
        GXScrollingCameraOverlayMaterialProgram;
    new (nlMalloc(sizeof(GXCrystalMaterialProgram), 8, false))
        GXCrystalMaterialProgram;
    new (nlMalloc(sizeof(GXWarbleMaterialProgram), 8, false))
        GXWarbleMaterialProgram;
    new (nlMalloc(sizeof(GXMaskedDiffuseBlendMaterialProgram), 8, false))
        GXMaskedDiffuseBlendMaterialProgram;
}
