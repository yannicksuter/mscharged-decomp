#ifndef NL_GLX_GX_DETAIL_MODULATE_MATERIAL_PROGRAM_H
#define NL_GLX_GX_DETAIL_MODULATE_MATERIAL_PROGRAM_H

#include "NL/gl/glModel.h"
#include "NL/glx/GXMaterialProgramBase.h"

struct GXDetailModulateParameters
{
    /* 0x00 */ glTextureBinding diffuseTexture;
    /* 0x08 */ glTextureBinding detailTexture;
    /* 0x10 */ float detailStrength;
    /* 0x14 */ int lightingEnabled;
    /* 0x18 */ int shadowEnabled;
}; // size: 0x1C

class GXDetailModulateMaterialProgram : public GXMaterialProgramImpl<GXDetailModulateMaterialProgram>
{
public:
    GXDetailModulateMaterialProgram();
    virtual ~GXDetailModulateMaterialProgram();
    virtual void Configure(glModelPacket* packet);
    virtual const GXMaterialParameter* GetParameters();
    virtual void Initialize();

    void ConfigureVertexFormat(bool indexed);
    void BindVertexArrays(const glModelPacket* packet);
    void DrawIndexed(const glModelPacket* packet);
    void DrawDirect(const glModelPacket* packet);
    void BindParameters(const glModelPacket* packet);

    static GXDetailModulateMaterialProgram* Instance;
    static bool Initialized;
    static GXMaterialParameter Parameters[5];
};


#endif // NL_GLX_GX_DETAIL_MODULATE_MATERIAL_PROGRAM_H
