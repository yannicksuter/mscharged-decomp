#ifndef NL_GLX_GX_SCROLLING_MASKED_DETAIL_BLEND_MATERIAL_PROGRAM_H
#define NL_GLX_GX_SCROLLING_MASKED_DETAIL_BLEND_MATERIAL_PROGRAM_H

#include "NL/gl/glModel.h"
#include "NL/glx/GXMaterialProgramBase.h"

struct GXScrollingMaskedDetailBlendParameters
{
    /* 0x00 */ glTextureBinding diffuseTexture;
    /* 0x08 */ glTextureBinding detailTexture;
    /* 0x10 */ glTextureBinding blendMaskTexture;
    /* 0x18 */ float diffuseScrollSpeedX;
    /* 0x1C */ float diffuseScrollSpeedY;
    /* 0x20 */ float detailScrollSpeedX;
    /* 0x24 */ float detailScrollSpeedY;
    /* 0x28 */ float blendMaskScrollSpeedX;
    /* 0x2C */ float blendMaskScrollSpeedY;
    /* 0x30 */ float blendAmount;
    /* 0x34 */ int lightingEnabled;
    /* 0x38 */ int shadowEnabled;
}; // size: 0x3C

class GXScrollingMaskedDetailBlendMaterialProgram : public GXMaterialProgramImpl<GXScrollingMaskedDetailBlendMaterialProgram>
{
public:
    GXScrollingMaskedDetailBlendMaterialProgram();
    virtual ~GXScrollingMaskedDetailBlendMaterialProgram();
    virtual void Configure(glModelPacket* packet);
    virtual const GXMaterialParameter* GetParameters();
    virtual void Initialize();

    void ConfigureVertexFormat(bool indexed);
    void BindVertexArrays(const glModelPacket* packet);
    void DrawIndexed(const glModelPacket* packet);
    void DrawDirect(const glModelPacket* packet);
    void BindParameters(const glModelPacket* packet);

    static GXScrollingMaskedDetailBlendMaterialProgram* Instance;
    static bool Initialized;
    static GXMaterialParameter Parameters[12];
};

#endif // NL_GLX_GX_SCROLLING_MASKED_DETAIL_BLEND_MATERIAL_PROGRAM_H
