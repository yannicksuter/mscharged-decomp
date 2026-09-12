#ifndef NL_GLX_GX_MASKED_SPECULAR_FRESNEL_MATERIAL_PROGRAM_H
#define NL_GLX_GX_MASKED_SPECULAR_FRESNEL_MATERIAL_PROGRAM_H

#include "NL/gl/glModel.h"
#include "NL/glx/GXMaterialProgramBase.h"

struct GXMaskedSpecularFresnelParameters
{
    /* 0x00 */ glTextureBinding diffuseTexture;
    /* 0x08 */ glTextureBinding specularTexture;
    /* 0x10 */ glTextureBinding specularMaskTexture;
    /* 0x18 */ float specularAmount;
    /* 0x1C */ float specularScaleX;
    /* 0x20 */ float specularScaleY;
    /* 0x24 */ float fresnelRamp;
    /* 0x28 */ int lightingEnabled;
    /* 0x2C */ int shadowEnabled;
}; // size: 0x30

class GXMaskedSpecularFresnelMaterialProgram : public GXMaterialProgramImpl<GXMaskedSpecularFresnelMaterialProgram>
{
public:
    GXMaskedSpecularFresnelMaterialProgram();
    virtual ~GXMaskedSpecularFresnelMaterialProgram();
    virtual void Configure(glModelPacket* packet);
    virtual const GXMaterialParameter* GetParameters();
    virtual void Initialize();

    void ConfigureVertexFormat(bool indexed);
    void BindVertexArrays(const glModelPacket* packet);
    void DrawIndexed(const glModelPacket* packet);
    void DrawDirect(const glModelPacket* packet);
    void BindParameters(const glModelPacket* packet);

    static GXMaskedSpecularFresnelMaterialProgram* Instance;
    static bool Initialized;
    static GXMaterialParameter Parameters[9];
};


#endif // NL_GLX_GX_MASKED_SPECULAR_FRESNEL_MATERIAL_PROGRAM_H
