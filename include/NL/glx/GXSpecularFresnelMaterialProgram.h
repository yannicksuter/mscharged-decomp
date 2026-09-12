#ifndef NL_GLX_GX_SPECULAR_FRESNEL_MATERIAL_PROGRAM_H
#define NL_GLX_GX_SPECULAR_FRESNEL_MATERIAL_PROGRAM_H

#include "NL/gl/glModel.h"
#include "NL/glx/GXMaterialProgramBase.h"

struct GXSpecularFresnelParameters
{
    /* 0x00 */ glTextureBinding diffuseTexture;
    /* 0x08 */ glTextureBinding detailTexture;
    /* 0x10 */ glTextureBinding specularTexture;
    /* 0x18 */ glTextureBinding specularMaskTexture;
    /* 0x20 */ const float (*skinMatrices)[3][4];
    /* 0x24 */ unsigned long skinMatricesSize;
    /* 0x28 */ float blendAmount;
    /* 0x2C */ float alphaValue;
    /* 0x30 */ float specularAmount;
    /* 0x34 */ float specularScaleX;
    /* 0x38 */ float specularScaleY;
    /* 0x3C */ int fresnelRamp;
    /* 0x40 */ unsigned long shadowLevel;
    /* 0x44 */ int lightingEnabled;
}; // size: 0x48

class GXSpecularFresnelMaterialProgram : public GXMaterialProgramImpl<GXSpecularFresnelMaterialProgram>
{
public:
    GXSpecularFresnelMaterialProgram();
    virtual ~GXSpecularFresnelMaterialProgram();
    virtual void Configure(glModelPacket* packet);
    virtual const GXMaterialParameter* GetParameters();
    virtual void Initialize();

    void ConfigureVertexFormat(bool indexed);
    void BindVertexArrays(const glModelPacket* packet);
    void BindParameters(const glModelPacket* packet);

    static GXSpecularFresnelMaterialProgram* Instance;
    static bool Initialized;
    static GXMaterialParameter Parameters[13];
};

#endif // NL_GLX_GX_SPECULAR_FRESNEL_MATERIAL_PROGRAM_H
