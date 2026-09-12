#ifndef NL_GLX_GX_MEGA_SPECULAR_MATERIAL_PROGRAM_H
#define NL_GLX_GX_MEGA_SPECULAR_MATERIAL_PROGRAM_H

#include "NL/gl/glModel.h"
#include "NL/glx/GXMaterialProgramBase.h"
#include "NL/nlColour.h"

struct GXMegaSpecularParameters
{
    /* 0x00 */ glTextureBinding diffuseTexture;
    /* 0x08 */ glTextureBinding detailTexture;
    /* 0x10 */ glTextureBinding glossTexture;
    /* 0x18 */ glTextureBinding megaTexture;
    /* 0x20 */ const float (*skinMatrices)[3][4];
    /* 0x24 */ unsigned long skinMatricesSize;
    /* 0x28 */ float blendAmount;
    /* 0x2C */ float alphaValue;
    /* 0x30 */ float specularLevel;
    /* 0x34 */ float specularExponent;
    /* 0x38 */ nlFloatColour specularColour;
    /* 0x48 */ float megaBlend;
    /* 0x4C */ unsigned long shadowLevel;
    /* 0x50 */ int lightingEnabled;
}; // size: 0x54

class GXMegaSpecularMaterialProgram : public GXMaterialProgramImpl<GXMegaSpecularMaterialProgram>
{
public:
    GXMegaSpecularMaterialProgram();
    virtual ~GXMegaSpecularMaterialProgram();
    virtual void Configure(glModelPacket* packet);
    virtual const GXMaterialParameter* GetParameters();
    virtual void Initialize();

    void ConfigureVertexFormat(bool indexed);
    void BindVertexArrays(const glModelPacket* packet);
    void BindParameters(const glModelPacket* packet);

    static GXMegaSpecularMaterialProgram* Instance;
    static bool Initialized;
    static GXMaterialParameter Parameters[13];
};

#endif // NL_GLX_GX_MEGA_SPECULAR_MATERIAL_PROGRAM_H
