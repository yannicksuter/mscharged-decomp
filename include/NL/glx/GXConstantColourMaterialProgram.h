#ifndef NL_GLX_GX_CONSTANT_COLOUR_MATERIAL_PROGRAM_H
#define NL_GLX_GX_CONSTANT_COLOUR_MATERIAL_PROGRAM_H

#include "NL/gl/glModel.h"
#include "NL/glx/GXMaterialProgramBase.h"
#include "NL/nlColour.h"

struct GXConstantColourParameters
{
    /* 0x00 */ glTextureBinding diffuseTexture;
    /* 0x08 */ nlFloatColour constantColour;
}; // size: 0x18

class GXConstantColourMaterialProgram : public GXMaterialProgramImpl<GXConstantColourMaterialProgram>
{
public:
    GXConstantColourMaterialProgram();
    virtual ~GXConstantColourMaterialProgram();
    virtual void Configure(glModelPacket* packet);
    virtual const GXMaterialParameter* GetParameters();
    virtual void Initialize();

    void ConfigureVertexFormat(bool indexed);
    void BindVertexArrays(const glModelPacket* packet);
    void DrawIndexed(const glModelPacket* packet);
    void DrawDirect(const glModelPacket* packet);
    void BindParameters(const glModelPacket* packet);

    static GXConstantColourMaterialProgram* Instance;
    static bool Initialized;
    static GXMaterialParameter Parameters[2];
};

#endif // NL_GLX_GX_CONSTANT_COLOUR_MATERIAL_PROGRAM_H
