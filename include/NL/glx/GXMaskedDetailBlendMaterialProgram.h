#ifndef NL_GLX_GX_MASKED_DETAIL_BLEND_MATERIAL_PROGRAM_H
#define NL_GLX_GX_MASKED_DETAIL_BLEND_MATERIAL_PROGRAM_H

#include "NL/gl/glModel.h"
#include "NL/glx/GXMaterialProgramBase.h"

struct GXMaskedDetailBlendParameters
{
    /* 0x00 */ glTextureBinding diffuseTexture;
    /* 0x08 */ glTextureBinding detailTexture;
    /* 0x10 */ glTextureBinding blendMaskTexture;
    /* 0x18 */ float blendAmount;
    /* 0x1C */ int lightingEnabled;
    /* 0x20 */ int receiveShadows;
}; // size: 0x24

class GXMaskedDetailBlendMaterialProgram : public GXMaterialProgramImpl<GXMaskedDetailBlendMaterialProgram>
{
public:
    GXMaskedDetailBlendMaterialProgram();
    virtual ~GXMaskedDetailBlendMaterialProgram();
    virtual void Configure(glModelPacket* packet);
    virtual const GXMaterialParameter* GetParameters();
    virtual void Initialize();

    void ConfigureVertexFormat(bool indexed);
    void BindVertexArrays(const glModelPacket* packet);
    void DrawIndexed(const glModelPacket* packet);
    void DrawDirect(const glModelPacket* packet);
    void BindParameters(const glModelPacket* packet);

    static GXMaskedDetailBlendMaterialProgram* Instance;
    static bool Initialized;
    static GXMaterialParameter Parameters[6];
};

#endif // NL_GLX_GX_MASKED_DETAIL_BLEND_MATERIAL_PROGRAM_H
