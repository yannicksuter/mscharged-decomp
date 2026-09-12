#ifndef NL_GLX_GX_VERTEX_COLOUR_TEXTURE_MATERIAL_PROGRAM_H
#define NL_GLX_GX_VERTEX_COLOUR_TEXTURE_MATERIAL_PROGRAM_H

#include "NL/gl/glModel.h"
#include "NL/glx/GXMaterialProgramBase.h"

struct GXVertexColourTextureParameters
{
    /* 0x00 */ glTextureBinding diffuseTexture;
}; // size: 0x8

class GXVertexColourTextureMaterialProgram : public GXMaterialProgramImpl<GXVertexColourTextureMaterialProgram>
{
public:
    GXVertexColourTextureMaterialProgram();
    virtual ~GXVertexColourTextureMaterialProgram();
    virtual void Configure(glModelPacket* packet);
    virtual const GXMaterialParameter* GetParameters();
    virtual void Initialize();

    void ConfigureVertexFormat(bool indexed);
    void BindVertexArrays(const glModelPacket* packet);
    void DrawIndexed(const glModelPacket* packet);
    void DrawDirect(const glModelPacket* packet);
    void BindParameters(const glModelPacket* packet);

    static GXVertexColourTextureMaterialProgram* Instance;
    static bool Initialized;
    static GXMaterialParameter Parameters[1];
};

#endif // NL_GLX_GX_VERTEX_COLOUR_TEXTURE_MATERIAL_PROGRAM_H
