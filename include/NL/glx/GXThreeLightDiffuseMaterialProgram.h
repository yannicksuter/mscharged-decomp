#ifndef NL_GLX_GX_THREE_LIGHT_DIFFUSE_MATERIAL_PROGRAM_H
#define NL_GLX_GX_THREE_LIGHT_DIFFUSE_MATERIAL_PROGRAM_H

#include "NL/gl/glModel.h"
#include "NL/glx/GXMaterialProgramBase.h"
#include "NL/nlColour.h"
#include "NL/nlMath.h"

struct GXThreeLightDiffuseParameters
{
    /* 0x00 */ glTextureBinding diffuseTexture;
    /* 0x08 */ nlVector3 lightDirections[3];
    /* 0x2C */ nlFloatColour lightColours[3];
    /* 0x5C */ nlFloatColour ambientColour;
}; // size: 0x6C

class GXThreeLightDiffuseMaterialProgram : public GXMaterialProgramImpl<GXThreeLightDiffuseMaterialProgram>
{
public:
    GXThreeLightDiffuseMaterialProgram();
    virtual ~GXThreeLightDiffuseMaterialProgram();
    virtual void Configure(glModelPacket* packet);
    virtual const GXMaterialParameter* GetParameters();
    virtual void Initialize();

    void ConfigureVertexFormat(bool indexed);
    void BindVertexArrays(const glModelPacket* packet);
    void DrawIndexed(const glModelPacket* packet);
    void DrawDirect(const glModelPacket* packet);
    void BindParameters(const glModelPacket* packet);

    static GXThreeLightDiffuseMaterialProgram* Instance;
    static bool Initialized;
    static GXMaterialParameter Parameters[8];
};

#endif // NL_GLX_GX_THREE_LIGHT_DIFFUSE_MATERIAL_PROGRAM_H
