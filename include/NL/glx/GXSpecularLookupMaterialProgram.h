#ifndef NL_GLX_GX_SPECULAR_LOOKUP_MATERIAL_PROGRAM_H
#define NL_GLX_GX_SPECULAR_LOOKUP_MATERIAL_PROGRAM_H

#include "NL/gl/glModel.h"
#include "NL/glx/GXMaterialProgramBase.h"

struct GXSpecularLookupParameters
{
    /* 0x00 */ glTextureBinding diffuseTexture;
    /* 0x08 */ glTextureBinding specularTexture;
    /* 0x10 */ glTextureBinding glossTexture;
}; // size: 0x18

class GXSpecularLookupMaterialProgram : public GXMaterialProgramImpl<GXSpecularLookupMaterialProgram>
{
public:
    GXSpecularLookupMaterialProgram();
    virtual ~GXSpecularLookupMaterialProgram();
    virtual void Configure(glModelPacket* packet);
    virtual const GXMaterialParameter* GetParameters();
    virtual void Initialize();

    void ConfigureVertexFormat(bool indexed);
    void BindVertexArrays(const glModelPacket* packet);
    void DrawIndexed(const glModelPacket* packet);
    void DrawDirect(const glModelPacket* packet);
    void BindParameters(const glModelPacket* packet);

    static GXSpecularLookupMaterialProgram* Instance;
    static bool Initialized;
    static GXMaterialParameter Parameters[3];
};

#endif // NL_GLX_GX_SPECULAR_LOOKUP_MATERIAL_PROGRAM_H
