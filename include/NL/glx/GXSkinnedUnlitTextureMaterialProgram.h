#ifndef NL_GLX_GX_SKINNED_UNLIT_TEXTURE_MATERIAL_PROGRAM_H
#define NL_GLX_GX_SKINNED_UNLIT_TEXTURE_MATERIAL_PROGRAM_H

#include "NL/gl/glModel.h"
#include "NL/glx/GXMaterialProgramBase.h"

struct GXSkinnedUnlitTextureParameters
{
    /* 0x00 */ glTextureBinding diffuseTexture;
    /* 0x08 */ const float (*skinMatrices)[3][4];
    /* 0x0C */ unsigned long skinMatrixBytes;
}; // size: 0x10

class GXSkinnedUnlitTextureMaterialProgram : public GXMaterialProgramImpl<GXSkinnedUnlitTextureMaterialProgram>
{
public:
    GXSkinnedUnlitTextureMaterialProgram();
    virtual ~GXSkinnedUnlitTextureMaterialProgram();
    virtual void Configure(glModelPacket* packet);
    virtual const GXMaterialParameter* GetParameters();
    virtual void Initialize();

    void ConfigureVertexFormat(bool indexed);
    void BindVertexArrays(const glModelPacket* packet);
    void BindParameters(const glModelPacket* packet);

    static GXSkinnedUnlitTextureMaterialProgram* Instance;
    static bool Initialized;
    static GXMaterialParameter Parameters[2];
};

#endif // NL_GLX_GX_SKINNED_UNLIT_TEXTURE_MATERIAL_PROGRAM_H
