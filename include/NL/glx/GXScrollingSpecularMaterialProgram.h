#ifndef NL_GLX_GX_SCROLLING_SPECULAR_MATERIAL_PROGRAM_H
#define NL_GLX_GX_SCROLLING_SPECULAR_MATERIAL_PROGRAM_H

#include "NL/gl/glModel.h"
#include "NL/glx/GXMaterialProgramBase.h"
#include "NL/nlColour.h"

struct GXScrollingSpecularParameters
{
    /* 0x00 */ glTextureBinding diffuseTexture;
    /* 0x08 */ glTextureBinding specularTexture;
    /* 0x10 */ float specularLevel;
    /* 0x14 */ float specularExponent;
    /* 0x18 */ nlFloatColour specularColour;
    /* 0x28 */ float scrollSpeedX;
    /* 0x2C */ float scrollSpeedY;
    /* 0x30 */ int scrollSpecularTexture;
    /* 0x34 */ int lightingEnabled;
    /* 0x38 */ int shadowEnabled;
}; // size: 0x3C

class GXScrollingSpecularMaterialProgram : public GXMaterialProgramImpl<GXScrollingSpecularMaterialProgram>
{
public:
    GXScrollingSpecularMaterialProgram();
    virtual ~GXScrollingSpecularMaterialProgram();
    virtual void Configure(glModelPacket* packet);
    virtual const GXMaterialParameter* GetParameters();
    virtual void Initialize();

    void ConfigureVertexFormat(bool indexed);
    void BindVertexArrays(const glModelPacket* packet);
    void DrawIndexed(const glModelPacket* packet);
    void DrawDirect(const glModelPacket* packet);
    void BindParameters(const glModelPacket* packet);

    static GXScrollingSpecularMaterialProgram* Instance;
    static bool Initialized;
    static GXMaterialParameter Parameters[10];
};

#endif // NL_GLX_GX_SCROLLING_SPECULAR_MATERIAL_PROGRAM_H
