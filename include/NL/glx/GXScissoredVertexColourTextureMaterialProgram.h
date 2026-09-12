#ifndef NL_GLX_GX_SCISSORED_VERTEX_COLOUR_TEXTURE_MATERIAL_PROGRAM_H
#define NL_GLX_GX_SCISSORED_VERTEX_COLOUR_TEXTURE_MATERIAL_PROGRAM_H

#include "NL/gl/glModel.h"
#include "NL/glx/GXMaterialProgramBase.h"

struct GXScissoredTextureParameters
{
    /* 0x00 */ glTextureBinding diffuseTexture;
    /* 0x08 */ float scissorX;
    /* 0x0C */ float scissorY;
    /* 0x10 */ float scissorWidth;
    /* 0x14 */ float scissorHeight;
}; // size: 0x18

class GXScissoredVertexColourTextureMaterialProgram
    : public GXMaterialProgramImpl<GXScissoredVertexColourTextureMaterialProgram>
{
public:
    GXScissoredVertexColourTextureMaterialProgram();
    virtual ~GXScissoredVertexColourTextureMaterialProgram();
    virtual void Configure(glModelPacket* packet);
    virtual const GXMaterialParameter* GetParameters();
    virtual void Initialize();

    void ConfigureVertexFormat(bool indexed);
    void BindVertexArrays(const glModelPacket* packet);
    void DrawIndexed(const glModelPacket* packet);
    void DrawDirect(const glModelPacket* packet);
    void BindParameters(const glModelPacket* packet);

    static GXScissoredVertexColourTextureMaterialProgram* Instance;
    static bool Initialized;
    static GXMaterialParameter Parameters[2];
};

#endif // NL_GLX_GX_SCISSORED_VERTEX_COLOUR_TEXTURE_MATERIAL_PROGRAM_H
