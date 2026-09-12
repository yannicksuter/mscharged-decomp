#ifndef NL_GLX_GX_SPECULAR_MATERIAL_PROGRAM_H
#define NL_GLX_GX_SPECULAR_MATERIAL_PROGRAM_H

#include "NL/gl/glModel.h"
#include "NL/glx/GXMaterialProgramBase.h"
#include "NL/nlColour.h"

struct GXSpecularParameters
{
    /* 0x00 */ glTextureBinding diffuseTexture;
    /* 0x08 */ glTextureBinding detailTexture;
    /* 0x10 */ glTextureBinding glossTexture;
    /* 0x18 */ const float (*skinMatrices)[3][4];
    /* 0x1C */ unsigned long skinMatricesSize;
    /* 0x20 */ float blendAmount;
    /* 0x24 */ float alphaValue;
    /* 0x28 */ float specularLevel;
    /* 0x2C */ float specularExponent;
    /* 0x30 */ nlFloatColour specularColour;
    /* 0x40 */ unsigned long shadowLevel;
    /* 0x44 */ int lightingEnabled;
}; // size: 0x48

class GXSpecularMaterialProgram : public GXMaterialProgramImpl<GXSpecularMaterialProgram>
{
public:
    GXSpecularMaterialProgram();
    virtual ~GXSpecularMaterialProgram();
    virtual void Configure(glModelPacket* packet);
    virtual const GXMaterialParameter* GetParameters();
    virtual void Initialize();

    void ConfigureVertexFormat(bool indexed);
    void BindVertexArrays(const glModelPacket* packet);
    void BindParameters(const glModelPacket* packet);

    static GXSpecularMaterialProgram* Instance;
    static bool Initialized;
    static GXMaterialParameter Parameters[11];
};

#endif // NL_GLX_GX_SPECULAR_MATERIAL_PROGRAM_H
