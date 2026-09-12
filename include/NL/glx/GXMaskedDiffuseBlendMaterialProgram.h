#ifndef NL_GLX_GX_MASKED_DIFFUSE_BLEND_MATERIAL_PROGRAM_H
#define NL_GLX_GX_MASKED_DIFFUSE_BLEND_MATERIAL_PROGRAM_H

#include "NL/gl/glModel.h"
#include "NL/glx/GXMaterialProgramBase.h"

struct GXMaskedDiffuseBlendParameters
{
    /* 0x00 */ glTextureBinding diffuseTexture;
    /* 0x08 */ glTextureBinding blendTexture;
    /* 0x10 */ glTextureBinding blendMaskTexture;
}; // size: 0x18

class GXMaskedDiffuseBlendMaterialProgram : public GXMaterialProgramImpl<GXMaskedDiffuseBlendMaterialProgram>
{
public:
    GXMaskedDiffuseBlendMaterialProgram();
    virtual ~GXMaskedDiffuseBlendMaterialProgram();
    virtual void Configure(glModelPacket* packet);
    virtual const GXMaterialParameter* GetParameters();
    virtual void Initialize();

    void ConfigureVertexFormat(bool indexed);
    void BindVertexArrays(const glModelPacket* packet);
    void DrawIndexed(const glModelPacket* packet);
    void DrawDirect(const glModelPacket* packet);
    void BindParameters(const glModelPacket* packet);

    static GXMaskedDiffuseBlendMaterialProgram* Instance;
    static bool Initialized;
    static GXMaterialParameter Parameters[3];
};

#endif // NL_GLX_GX_MASKED_DIFFUSE_BLEND_MATERIAL_PROGRAM_H
