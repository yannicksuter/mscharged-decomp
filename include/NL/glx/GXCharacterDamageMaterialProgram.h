#ifndef NL_GLX_GX_CHARACTER_DAMAGE_MATERIAL_PROGRAM_H
#define NL_GLX_GX_CHARACTER_DAMAGE_MATERIAL_PROGRAM_H

#include "NL/gl/glModel.h"
#include "NL/glx/GXMaterialProgramBase.h"

struct GXCharacterDamageParameters
{
    /* 0x00 */ glTextureBinding diffuseTexture;
    /* 0x08 */ glTextureBinding specularTexture;
    /* 0x10 */ glTextureBinding specularMaskTexture;
    /* 0x18 */ glTextureBinding megaTexture;
    /* 0x20 */ glTextureBinding damage1Texture;
    /* 0x28 */ glTextureBinding damage2Texture;
    /* 0x30 */ const float (*skinMatrices)[3][4];
    /* 0x34 */ unsigned long skinMatricesSize;
    /* 0x38 */ float alphaValue;
    /* 0x3C */ float specularAmount;
    /* 0x40 */ float specularScaleX;
    /* 0x44 */ float specularScaleY;
    /* 0x48 */ int fresnelRamp;
    /* 0x4C */ float megaBlend;
    /* 0x50 */ int unidentified50; // Retained descriptor; unused by this renderer.
    /* 0x54 */ unsigned long shadowLevel;
    /* 0x58 */ int lightingEnabled;
    /* 0x5C */ int blackOnly;
    /* 0x60 */ int damage1Enabled;
    /* 0x64 */ int damage2Enabled;
}; // size: 0x68

class GXCharacterDamageMaterialProgram : public GXMaterialProgramImpl<GXCharacterDamageMaterialProgram>
{
public:
    GXCharacterDamageMaterialProgram();
    virtual ~GXCharacterDamageMaterialProgram();
    virtual void Configure(glModelPacket* packet);
    virtual const GXMaterialParameter* GetParameters();
    virtual void Initialize();

    void ConfigureVertexFormat(bool indexed);
    void BindVertexArrays(const glModelPacket* packet);
    void BindParameters(const glModelPacket* packet);

    static GXCharacterDamageMaterialProgram* Instance;
    static bool Initialized;
    static GXMaterialParameter Parameters[19];
};

#endif // NL_GLX_GX_CHARACTER_DAMAGE_MATERIAL_PROGRAM_H
