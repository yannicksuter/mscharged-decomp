#ifndef NL_GLX_GX_SHADOW_VOLUME_MATERIAL_PROGRAM_H
#define NL_GLX_GX_SHADOW_VOLUME_MATERIAL_PROGRAM_H

#include "NL/gl/glModel.h"
#include "NL/glx/GXMaterialProgramBase.h"

struct GXShadowVolumeParameters
{
    /* 0x00 */ glTextureBinding diffuseTexture;
    /* 0x08 */ int useFixedColour;
}; // size: 0xC

class GXShadowVolumeMaterialProgram : public GXMaterialProgramImpl<GXShadowVolumeMaterialProgram>
{
public:
    GXShadowVolumeMaterialProgram();
    virtual ~GXShadowVolumeMaterialProgram();
    virtual void Configure(glModelPacket* packet);
    virtual const GXMaterialParameter* GetParameters();
    virtual void Initialize();

    void ConfigureVertexFormat(bool indexed);
    void BindVertexArrays(const glModelPacket* packet);
    void DrawIndexed(const glModelPacket* packet);
    void DrawDirect(const glModelPacket* packet);
    void BindParameters(const glModelPacket* packet);

    static GXShadowVolumeMaterialProgram* Instance;
    static bool Initialized;
    static GXMaterialParameter Parameters[2];
};

#endif // NL_GLX_GX_SHADOW_VOLUME_MATERIAL_PROGRAM_H
