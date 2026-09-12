#ifndef NL_GLX_GX_VERTEX_COLOUR_DETAIL_BLEND_MATERIAL_PROGRAM_H
#define NL_GLX_GX_VERTEX_COLOUR_DETAIL_BLEND_MATERIAL_PROGRAM_H

#include "NL/gl/glModel.h"
#include "NL/glx/GXMaterialProgramBase.h"

struct GXVertexColourDetailBlendParameters
{
    /* 0x00 */ glTextureBinding diffuseTexture;
    /* 0x08 */ glTextureBinding detailTexture;
    /* 0x10 */ float blendAmount;
}; // size: 0x14

class GXVertexColourDetailBlendMaterialProgram : public GXMaterialProgramImpl<GXVertexColourDetailBlendMaterialProgram>
{
public:
    GXVertexColourDetailBlendMaterialProgram();
    virtual ~GXVertexColourDetailBlendMaterialProgram();
    virtual void Configure(glModelPacket* packet);
    virtual const GXMaterialParameter* GetParameters();
    virtual void Initialize();

    void ConfigureVertexFormat(bool indexed);
    void BindVertexArrays(const glModelPacket* packet);
    void DrawIndexed(const glModelPacket* packet);
    void DrawDirect(const glModelPacket* packet);
    void BindParameters(const glModelPacket* packet);

    static GXVertexColourDetailBlendMaterialProgram* Instance;
    static bool Initialized;
    static GXMaterialParameter Parameters[3];
};

#endif // NL_GLX_GX_VERTEX_COLOUR_DETAIL_BLEND_MATERIAL_PROGRAM_H
