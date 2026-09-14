#ifndef NL_GLX_GX_CRYSTAL_MATERIAL_PROGRAM_H
#define NL_GLX_GX_CRYSTAL_MATERIAL_PROGRAM_H

#include "NL/gl/glModel.h"
#include "NL/glx/GXMaterialProgramBase.h"

struct GXCrystalMaterialParameters
{
    /* 0x00 */ glTextureBinding detailTexture;
    /* 0x08 */ glTextureBinding diffuseTexture;
    /* 0x10 */ glTextureBinding rampTexture;
}; // size: 0x18

class GXCrystalMaterialProgram : public GXMaterialProgramImpl<GXCrystalMaterialProgram>
{
public:
    GXCrystalMaterialProgram();
    virtual ~GXCrystalMaterialProgram();
    virtual void Configure(glModelPacket* packet);
    virtual const GXMaterialParameter* GetParameters()
    {
        return Parameters;
    }
    virtual void Initialize();

    void ConfigureVertexFormat(bool indexed);
    void BindVertexArrays(const glModelPacket* packet);
    void DrawIndexed(const glModelPacket* packet);
    void DrawDirect(const glModelPacket* packet);
    void BindParameters(const glModelPacket* packet);

    static GXCrystalMaterialProgram* Instance;
    static GXMaterialParameter Parameters[3];
};

#endif // NL_GLX_GX_CRYSTAL_MATERIAL_PROGRAM_H
