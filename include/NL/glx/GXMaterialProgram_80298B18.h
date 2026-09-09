#ifndef NL_GLX_GX_MATERIAL_PROGRAM_80298B18_H
#define NL_GLX_GX_MATERIAL_PROGRAM_80298B18_H

#include "NL/gl/glModel.h"
#include "NL/glx/GXMaterialProgramBase.h"

struct GXMaterialProgramParameters_80298B18
{
    /* 0x00 */ glTextureBinding texture0;
    /* 0x08 */ glTextureBinding texture1;
    /* 0x10 */ glTextureBinding texture2;
    /* 0x18 */ glTextureBinding texture3;
    /* 0x20 */ glTextureBinding texture4;
    /* 0x28 */ glTextureBinding texture5;
    /* 0x30 */ const float (*matrices)[3][4];
    /* 0x34 */ unsigned long matricesSize;
    /* 0x38 */ float value56;
    /* 0x3C */ float value60;
    /* 0x40 */ float scaleX;
    /* 0x44 */ float scaleY;
    /* 0x48 */ int textureIndex;
    /* 0x4C */ float value76;
    /* 0x50 */ int value80;
    /* 0x54 */ unsigned long value84;
    /* 0x58 */ int value88;
    /* 0x5C */ int value92;
    /* 0x60 */ int value96;
    /* 0x64 */ int value100;
}; // size: 0x68

class GXMaterialProgram_80298B18 : public GXMaterialProgramImpl<GXMaterialProgram_80298B18>
{
public:
    GXMaterialProgram_80298B18();
    virtual ~GXMaterialProgram_80298B18();
    virtual void Configure(glModelPacket* packet);
    virtual const GXMaterialParameter* GetParameters();
    virtual void Initialize();

    void ConfigureVertexFormat(bool indexed);
    void BindVertexArrays(const glModelPacket* packet);
    void BindParameters(const glModelPacket* packet);

    static GXMaterialProgram_80298B18* Instance;
    static bool Initialized;
    static GXMaterialParameter Parameters[19];
};

#endif // NL_GLX_GX_MATERIAL_PROGRAM_80298B18_H
