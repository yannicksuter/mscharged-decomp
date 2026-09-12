#ifndef NL_GLX_GX_FIXED_LIGHT_MATERIAL_PROGRAM_H
#define NL_GLX_GX_FIXED_LIGHT_MATERIAL_PROGRAM_H

#include "NL/gl/glModel.h"
#include "NL/glx/GXMaterialProgramBase.h"
#include "NL/nlColour.h"
#include "NL/nlMath.h"

struct GXFixedLightParameters
{
    /* 0x00 */ glTextureBinding diffuseTexture;
    /* 0x08 */ nlVector4 lightPosition;
    /* 0x18 */ nlFloatColour lightColour;
    /* 0x28 */ nlFloatColour ambientColour;
}; // size: 0x38

class GXFixedLightMaterialProgram : public GXMaterialProgramImpl<GXFixedLightMaterialProgram>
{
public:
    GXFixedLightMaterialProgram();
    virtual ~GXFixedLightMaterialProgram();
    virtual void Configure(glModelPacket* packet);
    virtual const GXMaterialParameter* GetParameters();
    virtual void Initialize();

    void ConfigureVertexFormat(bool indexed);
    void BindVertexArrays(const glModelPacket* packet);
    void DrawIndexed(const glModelPacket* packet);
    void DrawDirect(const glModelPacket* packet);
    void BindParameters(const glModelPacket* packet);

    static GXFixedLightMaterialProgram* Instance;
    static bool Initialized;
    static GXMaterialParameter Parameters[4];
};

#endif // NL_GLX_GX_FIXED_LIGHT_MATERIAL_PROGRAM_H
