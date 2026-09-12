#ifndef NL_GLX_GX_TEXTURE_COLOUR_ADD_MATERIAL_PROGRAM_H
#define NL_GLX_GX_TEXTURE_COLOUR_ADD_MATERIAL_PROGRAM_H

#include "NL/gl/glModel.h"
#include "NL/glx/GXMaterialProgramBase.h"
#include "NL/nlColour.h"

struct GXTextureColourAddParameters
{
    /* 0x00 */ glTextureBinding texture;
    /* 0x08 */ nlFloatColour colour;
}; // size: 0x18

class GXTextureColourAddMaterialProgram : public GXMaterialProgramImpl<GXTextureColourAddMaterialProgram>
{
public:
    GXTextureColourAddMaterialProgram();
    virtual ~GXTextureColourAddMaterialProgram();
    virtual void Configure(glModelPacket* packet);
    virtual const GXMaterialParameter* GetParameters();
    virtual void Initialize();

    void ConfigureVertexFormat(bool indexed);
    void BindVertexArrays(const glModelPacket* packet);
    void DrawIndexed(const glModelPacket* packet);
    void DrawDirect(const glModelPacket* packet);
    void BindParameters(const glModelPacket* packet);

    static GXTextureColourAddMaterialProgram* Instance;
    static bool Initialized;
    static GXMaterialParameter Parameters[2];
};

#endif // NL_GLX_GX_TEXTURE_COLOUR_ADD_MATERIAL_PROGRAM_H
