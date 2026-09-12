#ifndef NL_GLX_GX_SHADOWED_DIFFUSE_MATERIAL_PROGRAM_H
#define NL_GLX_GX_SHADOWED_DIFFUSE_MATERIAL_PROGRAM_H

#include "NL/gl/glModel.h"
#include "NL/glx/GXMaterialProgramBase.h"

struct GXShadowedDiffuseParameters
{
    /* 0x00 */ glTextureBinding diffuseTexture;
    /* 0x08 */ int receiveShadows;
}; // size: 0x0C

class GXShadowedDiffuseMaterialProgram : public GXMaterialProgramImpl<GXShadowedDiffuseMaterialProgram>
{
public:
    GXShadowedDiffuseMaterialProgram();
    virtual ~GXShadowedDiffuseMaterialProgram();
    virtual void Configure(glModelPacket* packet);
    virtual const GXMaterialParameter* GetParameters();
    virtual void Initialize();

    void ConfigureVertexFormat(bool indexed);
    void BindVertexArrays(const glModelPacket* packet);
    void DrawIndexed(const glModelPacket* packet);
    void DrawDirect(const glModelPacket* packet);
    void BindParameters(const glModelPacket* packet);

    static GXShadowedDiffuseMaterialProgram* Instance;
    static bool Initialized;
    static GXMaterialParameter Parameters[2];
};

#endif // NL_GLX_GX_SHADOWED_DIFFUSE_MATERIAL_PROGRAM_H
