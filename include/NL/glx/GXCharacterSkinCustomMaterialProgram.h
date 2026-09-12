#ifndef NL_GLX_GX_CHARACTER_SKIN_CUSTOM_MATERIAL_PROGRAM_H
#define NL_GLX_GX_CHARACTER_SKIN_CUSTOM_MATERIAL_PROGRAM_H

#include "NL/gl/glModel.h"
#include "NL/glx/GXMaterialProgramBase.h"

struct GXCharacterSkinCustomParameters
{
    /* 0x00 */ glTextureBinding diffuseTexture;
    /* 0x08 */ glTextureBinding detailTexture;
    /* 0x10 */ const float (*skinMatrices)[3][4];
    /* 0x14 */ unsigned long skinMatricesSize;
    /* 0x18 */ float blendAmount;
    /* 0x1C */ float alphaValue;
    /* 0x20 */ unsigned long shadowLevel;
    /* 0x24 */ int lightingEnabled;
}; // size: 0x28

class GXCharacterSkinCustomMaterialProgram : public GXMaterialProgramImpl<GXCharacterSkinCustomMaterialProgram>
{
public:
    GXCharacterSkinCustomMaterialProgram();
    virtual ~GXCharacterSkinCustomMaterialProgram();
    virtual void Configure(glModelPacket* packet);
    virtual const GXMaterialParameter* GetParameters();
    virtual void Initialize();

    void ConfigureVertexFormat(bool indexed);
    void BindVertexArrays(const glModelPacket* packet);
    void BindParameters(const glModelPacket* packet);

    static GXCharacterSkinCustomMaterialProgram* Instance;
    static bool Initialized;
    static GXMaterialParameter Parameters[7];
};

#endif // NL_GLX_GX_CHARACTER_SKIN_CUSTOM_MATERIAL_PROGRAM_H
