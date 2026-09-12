#ifndef NL_GLX_GX_SCROLLING_SHADOWED_DETAIL_BLEND_MATERIAL_PROGRAM_H
#define NL_GLX_GX_SCROLLING_SHADOWED_DETAIL_BLEND_MATERIAL_PROGRAM_H

#include "NL/gl/glModel.h"
#include "NL/glx/GXMaterialProgramBase.h"

struct GXScrollingShadowedDetailBlendParameters
{
    /* 0x00 */ glTextureBinding diffuseTexture;
    /* 0x08 */ glTextureBinding detailTexture;
    /* 0x10 */ glTextureBinding blendMaskTexture;
    /* 0x18 */ glTextureBinding shadowTexture;
    /* 0x20 */ float diffuseScrollSpeedX;
    /* 0x24 */ float diffuseScrollSpeedY;
    /* 0x28 */ float detailScrollSpeedX;
    /* 0x2C */ float detailScrollSpeedY;
    /* 0x30 */ float blendMaskScrollSpeedX;
    /* 0x34 */ float blendMaskScrollSpeedY;
    /* 0x38 */ float blendAmount;
    /* 0x3C */ int lightingEnabled;
    /* 0x40 */ float shadowLevel;
    /* 0x44 */ int shadowEnabled;
}; // size: 0x48

class GXScrollingShadowedDetailBlendMaterialProgram : public GXMaterialProgramImpl<GXScrollingShadowedDetailBlendMaterialProgram>
{
public:
    GXScrollingShadowedDetailBlendMaterialProgram();
    virtual ~GXScrollingShadowedDetailBlendMaterialProgram();
    virtual void Configure(glModelPacket* packet);
    virtual const GXMaterialParameter* GetParameters();
    virtual void Initialize();

    void ConfigureVertexFormat(bool indexed);
    void BindVertexArrays(const glModelPacket* packet);
    void DrawIndexed(const glModelPacket* packet);
    void DrawDirect(const glModelPacket* packet);
    void BindParameters(const glModelPacket* packet);

    static GXScrollingShadowedDetailBlendMaterialProgram* Instance;
    static bool Initialized;
    static GXMaterialParameter Parameters[14];
};

#endif // NL_GLX_GX_SCROLLING_SHADOWED_DETAIL_BLEND_MATERIAL_PROGRAM_H
