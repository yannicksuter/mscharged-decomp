#ifndef NL_GLX_GX_SPECULAR_DETAIL_BLEND_MATERIAL_PROGRAM_H
#define NL_GLX_GX_SPECULAR_DETAIL_BLEND_MATERIAL_PROGRAM_H

#include "NL/gl/glModel.h"
#include "NL/glx/GXMaterialProgramBase.h"
#include "NL/nlColour.h"

struct GXSpecularDetailBlendParameters
{
    /* 0x00 */ glTextureBinding diffuseTexture;
    /* 0x08 */ glTextureBinding detailTexture;
    /* 0x10 */ glTextureBinding blendMaskTexture;
    /* 0x18 */ glTextureBinding glossTexture;
    /* 0x20 */ float blendAmount;
    /* 0x24 */ float specularLevel;
    /* 0x28 */ float specularExponent;
    /* 0x2C */ nlFloatColour specularColour;
    /* 0x3C */ int lightingEnabled;
    /* 0x40 */ int shadowEnabled;
}; // size: 0x44

class GXSpecularDetailBlendMaterialProgram : public GXMaterialProgramImpl<GXSpecularDetailBlendMaterialProgram>
{
public:
    GXSpecularDetailBlendMaterialProgram();
    virtual ~GXSpecularDetailBlendMaterialProgram();
    virtual void Configure(glModelPacket* packet);
    virtual const GXMaterialParameter* GetParameters();
    virtual void Initialize();

    void ConfigureVertexFormat(bool indexed);
    void BindVertexArrays(const glModelPacket* packet);
    void DrawIndexed(const glModelPacket* packet);
    void DrawDirect(const glModelPacket* packet);
    void BindParameters(const glModelPacket* packet);

    static GXSpecularDetailBlendMaterialProgram* Instance;
    static bool Initialized;
    static GXMaterialParameter Parameters[10];
};

#endif // NL_GLX_GX_SPECULAR_DETAIL_BLEND_MATERIAL_PROGRAM_H
