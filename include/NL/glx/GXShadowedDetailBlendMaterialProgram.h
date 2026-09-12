#ifndef NL_GLX_GX_SHADOWED_DETAIL_BLEND_MATERIAL_PROGRAM_H
#define NL_GLX_GX_SHADOWED_DETAIL_BLEND_MATERIAL_PROGRAM_H

#include "NL/gl/glModel.h"
#include "NL/glx/GXMaterialProgramBase.h"

struct GXShadowedDetailBlendParameters
{
    /* 0x00 */ glTextureBinding diffuseTexture;
    /* 0x08 */ glTextureBinding detailTexture;
    /* 0x10 */ glTextureBinding blendMaskTexture;
    /* 0x18 */ glTextureBinding shadowTexture;
    /* 0x20 */ float blendAmount;
    /* 0x24 */ int lightingEnabled;
    /* 0x28 */ float shadowLevel;
    /* 0x2C */ int shadowEnabled;
    // Registered in the descriptor table, but unused by this renderer.
    /* 0x30 */ int unusedParameter;
}; // size: 0x34

class GXShadowedDetailBlendMaterialProgram : public GXMaterialProgramImpl<GXShadowedDetailBlendMaterialProgram>
{
public:
    GXShadowedDetailBlendMaterialProgram();
    virtual ~GXShadowedDetailBlendMaterialProgram();
    virtual void Configure(glModelPacket* packet);
    virtual const GXMaterialParameter* GetParameters();
    virtual void Initialize();

    void ConfigureVertexFormat(bool indexed);
    void BindVertexArrays(const glModelPacket* packet);
    void DrawIndexed(const glModelPacket* packet);
    void DrawDirect(const glModelPacket* packet);
    void BindParameters(const glModelPacket* packet);

    static GXShadowedDetailBlendMaterialProgram* Instance;
    static bool Initialized;
    static GXMaterialParameter Parameters[9];
};

#endif // NL_GLX_GX_SHADOWED_DETAIL_BLEND_MATERIAL_PROGRAM_H
