#ifndef NL_GLX_GX_FOUR_TEXTURE_ADD_MATERIAL_PROGRAM_H
#define NL_GLX_GX_FOUR_TEXTURE_ADD_MATERIAL_PROGRAM_H

#include "NL/gl/glModel.h"
#include "NL/glx/GXMaterialProgramBase.h"

struct GXFourTextureAddParameters
{
    /* 0x00 */ glTextureBinding diffuseTexture;
    /* 0x08 */ glTextureBinding detailTexture;
    /* 0x10 */ glTextureBinding shadowTexture;
    /* 0x18 */ glTextureBinding glossTexture;
}; // size: 0x20

class GXFourTextureAddMaterialProgram : public GXMaterialProgramImpl<GXFourTextureAddMaterialProgram>
{
public:
    GXFourTextureAddMaterialProgram();
    virtual ~GXFourTextureAddMaterialProgram();
    virtual void Configure(glModelPacket* packet);
    virtual const GXMaterialParameter* GetParameters();
    virtual void Initialize();

    void ConfigureVertexFormat(bool indexed);
    void BindVertexArrays(const glModelPacket* packet);
    void DrawIndexed(const glModelPacket* packet);
    void DrawDirect(const glModelPacket* packet);
    void BindParameters(const glModelPacket* packet);

    static GXFourTextureAddMaterialProgram* Instance;
    static bool Initialized;
    static GXMaterialParameter Parameters[4];
};

#endif // NL_GLX_GX_FOUR_TEXTURE_ADD_MATERIAL_PROGRAM_H
