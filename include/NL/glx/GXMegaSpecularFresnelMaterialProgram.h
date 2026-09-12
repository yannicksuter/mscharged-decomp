#ifndef NL_GLX_GX_MEGA_SPECULAR_FRESNEL_MATERIAL_PROGRAM_H
#define NL_GLX_GX_MEGA_SPECULAR_FRESNEL_MATERIAL_PROGRAM_H

#include "NL/gl/glModel.h"
#include "NL/glx/GXMaterialProgramBase.h"

struct GXMegaSpecularFresnelParameters
{
    /* 0x00 */ glTextureBinding diffuseTexture;
    /* 0x08 */ glTextureBinding detailTexture;
    /* 0x10 */ glTextureBinding specularTexture;
    /* 0x18 */ glTextureBinding specularMaskTexture;
    /* 0x20 */ glTextureBinding megaTexture;
    /* 0x28 */ const float (*skinMatrices)[3][4];
    /* 0x2C */ unsigned long skinMatricesSize;
    /* 0x30 */ float blendAmount;
    /* 0x34 */ float alphaValue;
    /* 0x38 */ float specularAmount;
    /* 0x3C */ float specularScaleX;
    /* 0x40 */ float specularScaleY;
    /* 0x44 */ int fresnelRamp;
    /* 0x48 */ float megaBlend;
    /* 0x4C */ unsigned long shadowLevel;
    /* 0x50 */ int lightingEnabled;
    /* 0x54 */ int blackOnly; // Retained descriptor; unused by this renderer.
}; // size: 0x58

class GXMegaSpecularFresnelMaterialProgram : public GXMaterialProgramImpl<GXMegaSpecularFresnelMaterialProgram>
{
public:
    GXMegaSpecularFresnelMaterialProgram();
    virtual ~GXMegaSpecularFresnelMaterialProgram();
    virtual void Configure(glModelPacket* packet);
    virtual const GXMaterialParameter* GetParameters();
    virtual void Initialize();

    void ConfigureVertexFormat(bool indexed);
    void BindVertexArrays(const glModelPacket* packet);
    void BindParameters(const glModelPacket* packet);

    static GXMegaSpecularFresnelMaterialProgram* Instance;
    static bool Initialized;
    static GXMaterialParameter Parameters[16];
};

#endif // NL_GLX_GX_MEGA_SPECULAR_FRESNEL_MATERIAL_PROGRAM_H
