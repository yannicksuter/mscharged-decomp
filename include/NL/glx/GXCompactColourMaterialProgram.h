#ifndef NL_GLX_GX_COMPACT_COLOUR_MATERIAL_PROGRAM_H
#define NL_GLX_GX_COMPACT_COLOUR_MATERIAL_PROGRAM_H

#include "NL/gl/glModel.h"
#include "NL/glx/GXMaterialProgramBase.h"

struct GXCompactColourParameters
{
    /* 0x00 */ glTextureBinding texture;
}; // size: 0x8

class GXCompactColourMaterialProgram : public GXMaterialProgramImpl<GXCompactColourMaterialProgram>
{
public:
    GXCompactColourMaterialProgram();
    virtual ~GXCompactColourMaterialProgram();
    virtual void Configure(glModelPacket* packet);
    virtual const GXMaterialParameter* GetParameters();
    virtual void Initialize();

    void ConfigureVertexFormat(bool indexed);
    void BindVertexArrays(const glModelPacket* packet);
    void DrawIndexed(const glModelPacket* packet);
    void DrawDirect(const glModelPacket* packet);
    void BindParameters(const glModelPacket* packet);

    static GXCompactColourMaterialProgram* Instance;
    static bool Initialized;
    static GXMaterialParameter Parameters[1];
};

#endif // NL_GLX_GX_COMPACT_COLOUR_MATERIAL_PROGRAM_H
