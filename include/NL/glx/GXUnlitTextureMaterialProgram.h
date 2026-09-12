#ifndef NL_GLX_GX_UNLIT_TEXTURE_MATERIAL_PROGRAM_H
#define NL_GLX_GX_UNLIT_TEXTURE_MATERIAL_PROGRAM_H

#include "NL/gl/glModel.h"
#include "NL/glx/GXMaterialProgramBase.h"

struct GXUnlitTextureParameters
{
    /* 0x00 */ glTextureBinding texture;
}; // size: 0x8

class GXUnlitTextureMaterialProgram : public GXMaterialProgramImpl<GXUnlitTextureMaterialProgram>
{
public:
    GXUnlitTextureMaterialProgram();
    virtual ~GXUnlitTextureMaterialProgram();
    virtual void Configure(glModelPacket* packet);
    virtual const GXMaterialParameter* GetParameters();
    virtual void Initialize();

    void ConfigureVertexFormat(bool indexed);
    void BindVertexArrays(const glModelPacket* packet);
    void DrawIndexed(const glModelPacket* packet);
    void DrawDirect(const glModelPacket* packet);
    void BindParameters(const glModelPacket* packet);

    static GXUnlitTextureMaterialProgram* Instance;
    static bool Initialized;
    static GXMaterialParameter Parameters[1];
};

#endif // NL_GLX_GX_UNLIT_TEXTURE_MATERIAL_PROGRAM_H
