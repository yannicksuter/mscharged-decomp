#ifndef NL_GLX_GX_TEXTURE_BLEND_MATERIAL_PROGRAM_H
#define NL_GLX_GX_TEXTURE_BLEND_MATERIAL_PROGRAM_H

#include "NL/gl/glModel.h"
#include "NL/glx/GXMaterialProgramBase.h"

struct GXTextureBlendParameters
{
    /* 0x00 */ glTextureBinding diffuseTexture;
    /* 0x08 */ glTextureBinding detailTexture;
    /* 0x10 */ float blendAmount;
}; // size: 0x14

class GXTextureBlendMaterialProgram : public GXMaterialProgramImpl<GXTextureBlendMaterialProgram>
{
public:
    GXTextureBlendMaterialProgram();
    virtual ~GXTextureBlendMaterialProgram();
    virtual void Configure(glModelPacket* packet);
    virtual const GXMaterialParameter* GetParameters();
    virtual void Initialize();

    void ConfigureVertexFormat(bool indexed);
    void BindVertexArrays(const glModelPacket* packet);
    void DrawIndexed(const glModelPacket* packet);
    void DrawDirect(const glModelPacket* packet);
    void BindParameters(const glModelPacket* packet);

    static GXTextureBlendMaterialProgram* Instance;
    static bool Initialized;
    static GXMaterialParameter Parameters[3];
};

#endif // NL_GLX_GX_TEXTURE_BLEND_MATERIAL_PROGRAM_H
