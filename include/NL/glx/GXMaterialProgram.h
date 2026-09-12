#ifndef NL_GLX_GX_MATERIAL_PROGRAM_H
#define NL_GLX_GX_MATERIAL_PROGRAM_H

#include <revolution/gx/GXTypes.h>

#include "NL/gl/glModel.h"
#include "NL/glx/GXMaterialProgramBase.h"
#include "NL/glx/GXCharacterSkinCustomMaterialProgram.h"
#include "NL/glx/GXSpecularFresnelMaterialProgram.h"
#include "NL/glx/GXMegaSpecularFresnelMaterialProgram.h"
#include "NL/glx/GXCharacterDamageMaterialProgram.h"
#include "NL/glx/GXMegaDiffuseMaterialProgram.h"
#include "NL/glx/GXSpecularMaterialProgram.h"
#include "NL/glx/GXMegaSpecularMaterialProgram.h"
#include "NL/glx/GXColourFresnelMaterialProgram.h"
#include "NL/glx/GXBlackTextureAlphaMaterialProgram.h"
#include "NL/glx/GXSpecularLookupMaterialProgram.h"
#include "NL/glx/GXShadowedDiffuseMaterialProgram.h"
#include "NL/glx/GXMaskedDiffuseBlendMaterialProgram.h"
#include "NL/glx/GXSkinnedMultiLightMaterialProgram.h"
#include "NL/glx/GXMaskedDetailBlendMaterialProgram.h"
#include "NL/glx/GXShadowedDetailBlendMaterialProgram.h"
#include "NL/glx/GXScrollingShadowedDetailBlendMaterialProgram.h"
#include "NL/glx/GXSpecularDetailBlendMaterialProgram.h"
#include "NL/glx/GXScrollingMaskedDetailBlendMaterialProgram.h"
#include "NL/glx/GXScrollingDiffuseMaterialProgram.h"
#include "NL/glx/GXDetailModulateMaterialProgram.h"
#include "NL/glx/GXMaskedSpecularFresnelMaterialProgram.h"
#include "NL/glx/GXCameraScrolledOverlayMaterialProgram.h"
#include "NL/glx/GXScrollingCameraOverlayMaterialProgram.h"
#include "NL/glx/GXScrollingSpecularMaterialProgram.h"
#include "NL/glx/GXMovieMaterialProgram.h"
#include "NL/glx/GXCompactColourMaterialProgram.h"
#include "NL/glx/GXUnlitTextureMaterialProgram.h"
#include "NL/glx/GXTextureBlendMaterialProgram.h"
#include "NL/glx/GXVertexColourDetailBlendMaterialProgram.h"
#include "NL/glx/GXFixedLightMaterialProgram.h"
#include "NL/glx/GXThreeLightDiffuseMaterialProgram.h"
#include "NL/glx/GXSkinnedUnlitTextureMaterialProgram.h"
#include "NL/glx/GXVertexColourTextureMaterialProgram.h"
#include "NL/glx/GXScissoredVertexColourTextureMaterialProgram.h"
#include "NL/glx/GXVertexColourMaterialProgram.h"
#include "NL/glx/GXFloatTexturedColourMaterialProgram.h"
#include "NL/glx/GXShadowVolumeMaterialProgram.h"
#include "NL/glx/GXTextureColourAddMaterialProgram.h"
#include "NL/glx/GXFourTextureAddMaterialProgram.h"
#include "NL/gl/glMaterialParameters.h"
#include "NL/glx/glxLight.h"
#include "NL/glx/glxSkinMatrix.h"
#include "NL/glx/glxTexture.h"

class GLView;

void glx_EnableWarble(bool enabled);

struct GXCrystalMaterialParameters
{
    /* 0x00 */ glTextureBinding texture0;
    /* 0x08 */ glTextureBinding texture1;
    /* 0x10 */ glTextureBinding texture2;
}; // size: 0x18

class GXCrystalMaterialProgram : public GXMaterialProgramImpl<GXCrystalMaterialProgram>
{
public:
    GXCrystalMaterialProgram();
    virtual ~GXCrystalMaterialProgram();
    virtual void Configure(glModelPacket* packet);
    virtual const GXMaterialParameter* GetParameters()
    {
        return Parameters;
    }
    virtual void Initialize();

    void ConfigureVertexFormat(bool indexed);
    void BindVertexArrays(const glModelPacket* packet);
    void DrawIndexed(const glModelPacket* packet);
    void DrawDirect(const glModelPacket* packet);
    void BindParameters(const glModelPacket* packet);

    static GXCrystalMaterialProgram* Instance;
    static bool Initialized;
    static GXMaterialParameter Parameters[3];
};

class GXWarbleMaterialProgram : public GXMaterialProgramImpl<GXWarbleMaterialProgram>
{
public:
    GXWarbleMaterialProgram();
    virtual ~GXWarbleMaterialProgram();
    virtual void Configure(glModelPacket* packet);
    virtual const GXMaterialParameter* GetParameters();
    virtual void Initialize();

    void ConfigureVertexFormat(bool indexed);
    void BindVertexArrays(const glModelPacket* packet);
    void DrawIndexed(const glModelPacket* packet);
    void DrawDirect(const glModelPacket* packet);
    void BindParameters(const glModelPacket* packet);

    static GXWarbleMaterialProgram* Instance;
    static bool Initialized;
    static GXMaterialParameter Parameters[1];
};

class GXConstantColourMaterialProgram : public GXMaterialProgramImpl<GXConstantColourMaterialProgram>
{
public:
    GXConstantColourMaterialProgram();
    virtual ~GXConstantColourMaterialProgram();
    virtual void Configure(glModelPacket* packet);
    virtual const GXMaterialParameter* GetParameters();
    virtual void Initialize();

    void ConfigureVertexFormat(bool indexed);
    void BindVertexArrays(const glModelPacket* packet);
    void DrawIndexed(const glModelPacket* packet);
    void DrawDirect(const glModelPacket* packet);
    void BindParameters(const glModelPacket* packet);

    static GXConstantColourMaterialProgram* Instance;
    static bool Initialized;
    static GXMaterialParameter Parameters[2];
};

class GXRedColourMaterialProgram : public GXMaterialProgramImpl<GXRedColourMaterialProgram>
{
public:
    GXRedColourMaterialProgram();
    virtual ~GXRedColourMaterialProgram();
    virtual void Configure(glModelPacket* packet);
    virtual const GXMaterialParameter* GetParameters();
    virtual void Initialize();

    void ConfigureVertexFormat(bool indexed);
    void BindVertexArrays(const glModelPacket* packet);
    void DrawIndexed(const glModelPacket* packet);
    void DrawDirect(const glModelPacket* packet);
    void BindParameters(const glModelPacket* packet);

    static GXRedColourMaterialProgram* Instance;
    static bool Initialized;
};

#endif // NL_GLX_GX_MATERIAL_PROGRAM_H
