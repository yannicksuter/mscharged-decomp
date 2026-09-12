#ifndef NL_GLX_GX_BLACK_TEXTURE_ALPHA_MATERIAL_PROGRAM_H
#define NL_GLX_GX_BLACK_TEXTURE_ALPHA_MATERIAL_PROGRAM_H

#include "NL/gl/glModel.h"
#include "NL/glx/GXMaterialProgramBase.h"

struct GXBlackTextureAlphaParameters
{
    /* 0x00 */ glTextureBinding diffuseTexture;
    /* 0x08 */ const float (*skinMatrices)[3][4];
    /* 0x0C */ unsigned long skinMatricesSize;
}; // size: 0x10

class GXBlackTextureAlphaMaterialProgram : public GXMaterialProgramImpl<GXBlackTextureAlphaMaterialProgram>
{
public:
    GXBlackTextureAlphaMaterialProgram();
    virtual ~GXBlackTextureAlphaMaterialProgram();
    virtual void Configure(glModelPacket* packet);
    virtual const GXMaterialParameter* GetParameters();
    virtual void Initialize();

    void ConfigureVertexFormat(bool indexed);
    void BindVertexArrays(const glModelPacket* packet);
    void BindParameters(const glModelPacket* packet);

    static GXBlackTextureAlphaMaterialProgram* Instance;
    static bool Initialized;
    static GXMaterialParameter Parameters[2];
};

#endif // NL_GLX_GX_BLACK_TEXTURE_ALPHA_MATERIAL_PROGRAM_H
