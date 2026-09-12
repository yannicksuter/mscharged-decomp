#ifndef NL_GLX_GX_COLOUR_FRESNEL_MATERIAL_PROGRAM_H
#define NL_GLX_GX_COLOUR_FRESNEL_MATERIAL_PROGRAM_H

#include "NL/gl/glModel.h"
#include "NL/glx/GXMaterialProgramBase.h"

struct GXColourFresnelParameters
{
    /* 0x00 */ glTextureBinding diffuseTexture;
    /* 0x08 */ glTextureBinding detailTexture;
    /* 0x10 */ glTextureBinding shadowTexture;
    /* 0x18 */ const float (*skinMatrices)[3][4];
    /* 0x1C */ unsigned long skinMatricesSize;
    /* 0x20 */ float blendAmount;
    /* 0x24 */ float alphaValue;
    /* 0x28 */ int colourFresnelRamp;
    /* 0x2C */ int lightingEnabled;
}; // size: 0x30

class GXColourFresnelMaterialProgram : public GXMaterialProgramImpl<GXColourFresnelMaterialProgram>
{
public:
    GXColourFresnelMaterialProgram();
    virtual ~GXColourFresnelMaterialProgram();
    virtual void Configure(glModelPacket* packet);
    virtual const GXMaterialParameter* GetParameters();
    virtual void Initialize();

    void ConfigureVertexFormat(bool indexed);
    void BindVertexArrays(const glModelPacket* packet);
    void BindParameters(const glModelPacket* packet);

    static GXColourFresnelMaterialProgram* Instance;
    static bool Initialized;
    static GXMaterialParameter Parameters[8];
};

#endif // NL_GLX_GX_COLOUR_FRESNEL_MATERIAL_PROGRAM_H
