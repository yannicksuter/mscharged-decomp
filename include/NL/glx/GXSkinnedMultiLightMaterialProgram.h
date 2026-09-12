#ifndef NL_GLX_GX_SKINNED_MULTI_LIGHT_MATERIAL_PROGRAM_H
#define NL_GLX_GX_SKINNED_MULTI_LIGHT_MATERIAL_PROGRAM_H

#include "NL/gl/glModel.h"
#include "NL/glx/GXMaterialProgramBase.h"
#include "NL/nlColour.h"
#include "NL/nlMath.h"

struct GXSkinnedMultiLightParameters
{
    struct PointLight
    {
        nlVector3 position;
        float distance;
    };

    struct SpecularLight
    {
        nlVector3 direction;
        float exponent;
    };

    /* 0x000 */ glTextureBinding diffuseTexture;
    /* 0x008 */ glTextureBinding detailTexture;
    /* 0x010 */ glTextureBinding rampTexture;
    /* 0x018 */ const float (*skinMatrices)[3][4];
    /* 0x01C */ unsigned long skinMatricesSize;
    /* 0x020 */ float blendAmount;
    /* 0x024 */ int animateNormalTexCoords;
    /* 0x028 */ nlVector3 lightDirections[4];
    /* 0x058 */ PointLight pointLights[4];
    /* 0x098 */ SpecularLight specularLights[4];
    /* 0x0D8 */ nlFloatColour lightColours[8];
    /* 0x158 */ int useDirectionalLights;
    /* 0x15C */ unsigned char unidentified15C[12];
    /* 0x168 */ int diffuseLightCount;
    /* 0x16C */ int specularLightCount;
}; // size: 0x170

class GXSkinnedMultiLightMaterialProgram : public GXMaterialProgramImpl<GXSkinnedMultiLightMaterialProgram>
{
public:
    GXSkinnedMultiLightMaterialProgram();
    virtual ~GXSkinnedMultiLightMaterialProgram();
    virtual void Configure(glModelPacket* packet);
    virtual const GXMaterialParameter* GetParameters();
    virtual void Initialize();

    void ConfigureVertexFormat(bool indexed);
    void BindVertexArrays(const glModelPacket* packet);
    void BindParameters(const glModelPacket* packet);

    static GXSkinnedMultiLightMaterialProgram* Instance;
    static bool Initialized;
    static GXMaterialParameter Parameters[37];
};

#endif // NL_GLX_GX_SKINNED_MULTI_LIGHT_MATERIAL_PROGRAM_H
