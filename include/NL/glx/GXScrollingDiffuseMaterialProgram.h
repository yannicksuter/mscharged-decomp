#ifndef NL_GLX_GX_SCROLLING_DIFFUSE_MATERIAL_PROGRAM_H
#define NL_GLX_GX_SCROLLING_DIFFUSE_MATERIAL_PROGRAM_H

#include "NL/gl/glModel.h"
#include "NL/glx/GXMaterialProgramBase.h"

struct GXScrollingDiffuseParameters
{
    /* 0x00 */ glTextureBinding diffuseTexture;
    /* 0x08 */ float scrollSpeedX;
    /* 0x0C */ float scrollSpeedY;
    /* 0x10 */ int shadowEnabled;
    /* 0x14 */ int textureWrapEnabled;
    /* 0x18 */ int lightingEnabled;
    /* 0x1C */ int disableCulling;
    /* 0x20 */ int forceDepthWrite;
}; // size: 0x24

class GXScrollingDiffuseMaterialProgram : public GXMaterialProgramImpl<GXScrollingDiffuseMaterialProgram>
{
public:
    GXScrollingDiffuseMaterialProgram();
    virtual ~GXScrollingDiffuseMaterialProgram();
    virtual void Configure(glModelPacket* packet);
    virtual const GXMaterialParameter* GetParameters();
    virtual void Initialize();

    void ConfigureVertexFormat(bool indexed);
    void BindVertexArrays(const glModelPacket* packet);
    void DrawIndexed(const glModelPacket* packet);
    void DrawDirect(const glModelPacket* packet);
    void BindParameters(const glModelPacket* packet);

    static GXScrollingDiffuseMaterialProgram* Instance;
    static bool Initialized;
    static GXMaterialParameter Parameters[8];
};

#endif // NL_GLX_GX_SCROLLING_DIFFUSE_MATERIAL_PROGRAM_H
