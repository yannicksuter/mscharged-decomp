#ifndef NL_GLX_GX_MATERIAL_PROGRAM_H
#define NL_GLX_GX_MATERIAL_PROGRAM_H

#include <revolution/gx/GXTypes.h>

#include "NL/gl/glModel.h"
#include "NL/glx/GXMaterialProgramBase.h"
#include "NL/glx/GXMaterialProgram_80298B18.h"
#include "NL/gl/glMaterialParameters.h"
#include "NL/glx/glxLight.h"
#include "NL/glx/glxSkinMatrix.h"
#include "NL/glx/glxTexture.h"

class GLView;

void glx_EnableWarble(bool enabled);

extern GXPrimitive glx_PrimitiveTypes[6];



struct GXMaterialProgramParameters_802A6B6C
{
    /* 0x00 */ unsigned long texture;
    /* 0x04 */ unsigned long padding;
    /* 0x08 */ float scissorX;
    /* 0x0C */ float scissorY;
    /* 0x10 */ float scissorWidth;
    /* 0x14 */ float scissorHeight;
}; // size: 0x18

class GXMaterialProgram_802A6B6C
    : public GXMaterialProgramImpl<GXMaterialProgram_802A6B6C>
{
public:
    GXMaterialProgram_802A6B6C();
    virtual ~GXMaterialProgram_802A6B6C();
    virtual void Configure(glModelPacket* packet);
    virtual const GXMaterialParameter* GetParameters();
    virtual void Initialize();

    void ConfigureVertexFormat(bool indexed);
    void BindVertexArrays(const glModelPacket* packet);
    void DrawIndexed(const glModelPacket* packet);
    void DrawDirect(const glModelPacket* packet);
    void BindParameters(const glModelPacket* packet);

    static GXMaterialProgram_802A6B6C* Instance;
    static bool Initialized;
    static GXMaterialParameter Parameters[2];
};

class GXMaterialProgram_802981F0 : public GXMaterialProgramImpl<GXMaterialProgram_802981F0>
{
public:
    GXMaterialProgram_802981F0();
    virtual ~GXMaterialProgram_802981F0();
    virtual void Configure(glModelPacket* packet);
    virtual const GXMaterialParameter* GetParameters();
    virtual void Initialize();

    void ConfigureVertexFormat(bool indexed);
    void BindVertexArrays(const glModelPacket* packet);
    void BindParameters(const glModelPacket* packet);

    static GXMaterialProgram_802981F0* Instance;
    static bool Initialized;
    static GXMaterialParameter Parameters[7];
};

class GXMaterialProgram_80298478 : public GXMaterialProgramImpl<GXMaterialProgram_80298478>
{
public:
    GXMaterialProgram_80298478();
    virtual ~GXMaterialProgram_80298478();
    virtual void Configure(glModelPacket* packet);
    virtual const GXMaterialParameter* GetParameters();
    virtual void Initialize();

    void ConfigureVertexFormat(bool indexed);
    void BindVertexArrays(const glModelPacket* packet);
    void BindParameters(const glModelPacket* packet);

    static GXMaterialProgram_80298478* Instance;
    static bool Initialized;
    static GXMaterialParameter Parameters[13];
};

class GXMaterialProgram_802987A0 : public GXMaterialProgramImpl<GXMaterialProgram_802987A0>
{
public:
    GXMaterialProgram_802987A0();
    virtual ~GXMaterialProgram_802987A0();
    virtual void Configure(glModelPacket* packet);
    virtual const GXMaterialParameter* GetParameters();
    virtual void Initialize();

    void ConfigureVertexFormat(bool indexed);
    void BindVertexArrays(const glModelPacket* packet);
    void BindParameters(const glModelPacket* packet);

    static GXMaterialProgram_802987A0* Instance;
    static bool Initialized;
    static GXMaterialParameter Parameters[16];
};


class GXMaterialProgram_80298EE0 : public GXMaterialProgramImpl<GXMaterialProgram_80298EE0>
{
public:
    GXMaterialProgram_80298EE0();
    virtual ~GXMaterialProgram_80298EE0();
    virtual void Configure(glModelPacket* packet);
    virtual const GXMaterialParameter* GetParameters();
    virtual void Initialize();

    void ConfigureVertexFormat(bool indexed);
    void BindVertexArrays(const glModelPacket* packet);
    void BindParameters(const glModelPacket* packet);

    static GXMaterialProgram_80298EE0* Instance;
    static bool Initialized;
    static GXMaterialParameter Parameters[9];
};

class GXMaterialProgram_802991B8 : public GXMaterialProgramImpl<GXMaterialProgram_802991B8>
{
public:
    GXMaterialProgram_802991B8();
    virtual ~GXMaterialProgram_802991B8();
    virtual void Configure(glModelPacket* packet);
    virtual const GXMaterialParameter* GetParameters();
    virtual void Initialize();

    void ConfigureVertexFormat(bool indexed);
    void BindVertexArrays(const glModelPacket* packet);
    void BindParameters(const glModelPacket* packet);

    static GXMaterialProgram_802991B8* Instance;
    static bool Initialized;
    static GXMaterialParameter Parameters[11];
};

class GXMaterialProgram_80299490 : public GXMaterialProgramImpl<GXMaterialProgram_80299490>
{
public:
    GXMaterialProgram_80299490();
    virtual ~GXMaterialProgram_80299490();
    virtual void Configure(glModelPacket* packet);
    virtual const GXMaterialParameter* GetParameters();
    virtual void Initialize();

    void ConfigureVertexFormat(bool indexed);
    void BindVertexArrays(const glModelPacket* packet);
    void BindParameters(const glModelPacket* packet);

    static GXMaterialProgram_80299490* Instance;
    static bool Initialized;
    static GXMaterialParameter Parameters[13];
};

class GXMaterialProgram_802997B8 : public GXMaterialProgramImpl<GXMaterialProgram_802997B8>
{
public:
    GXMaterialProgram_802997B8();
    virtual ~GXMaterialProgram_802997B8();
    virtual void Configure(glModelPacket* packet);
    virtual const GXMaterialParameter* GetParameters();
    virtual void Initialize();

    void ConfigureVertexFormat(bool indexed);
    void BindVertexArrays(const glModelPacket* packet);
    void BindParameters(const glModelPacket* packet);

    static GXMaterialProgram_802997B8* Instance;
    static bool Initialized;
    static GXMaterialParameter Parameters[8];
};

class GXMaterialProgram_80299A90 : public GXMaterialProgramImpl<GXMaterialProgram_80299A90>
{
public:
    GXMaterialProgram_80299A90();
    virtual ~GXMaterialProgram_80299A90();
    virtual void Configure(glModelPacket* packet);
    virtual const GXMaterialParameter* GetParameters();
    virtual void Initialize();

    void ConfigureVertexFormat(bool indexed);
    void BindVertexArrays(const glModelPacket* packet);
    void BindParameters(const glModelPacket* packet);

    static GXMaterialProgram_80299A90* Instance;
    static bool Initialized;
    static GXMaterialParameter Parameters[2];
};

struct GXCrystalMaterialParameters
{
    /* 0x00 */ glTextureBinding texture0;
    /* 0x08 */ glTextureBinding texture1;
    /* 0x10 */ glTextureBinding texture2;
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
    static bool Initialized;
    static GXMaterialParameter Parameters[3];
};

class GXMaterialProgram_8029A4A0 : public GXMaterialProgramImpl<GXMaterialProgram_8029A4A0>
{
public:
    GXMaterialProgram_8029A4A0();
    virtual ~GXMaterialProgram_8029A4A0();
    virtual void Configure(glModelPacket* packet);
    virtual const GXMaterialParameter* GetParameters();
    virtual void Initialize();

    void ConfigureVertexFormat(bool indexed);
    void BindVertexArrays(const glModelPacket* packet);
    void DrawIndexed(const glModelPacket* packet);
    void DrawDirect(const glModelPacket* packet);
    void BindParameters(const glModelPacket* packet);

    static GXMaterialProgram_8029A4A0* Instance;
    static bool Initialized;
    static GXMaterialParameter Parameters[4];
};

class GXMaterialProgram_8029AB0C : public GXMaterialProgramImpl<GXMaterialProgram_8029AB0C>
{
public:
    GXMaterialProgram_8029AB0C();
    virtual ~GXMaterialProgram_8029AB0C();
    virtual void Configure(glModelPacket* packet);
    virtual const GXMaterialParameter* GetParameters();
    virtual void Initialize();

    void ConfigureVertexFormat(bool indexed);
    void BindVertexArrays(const glModelPacket* packet);
    void DrawIndexed(const glModelPacket* packet);
    void DrawDirect(const glModelPacket* packet);
    void BindParameters(const glModelPacket* packet);

    static GXMaterialProgram_8029AB0C* Instance;
    static bool Initialized;
    static GXMaterialParameter Parameters[3];
};

class GXMaterialProgram_8029AFC4 : public GXMaterialProgramImpl<GXMaterialProgram_8029AFC4>
{
public:
    GXMaterialProgram_8029AFC4();
    virtual ~GXMaterialProgram_8029AFC4();
    virtual void Configure(glModelPacket* packet);
    virtual const GXMaterialParameter* GetParameters();
    virtual void Initialize();

    void ConfigureVertexFormat(bool indexed);
    void BindVertexArrays(const glModelPacket* packet);
    void DrawIndexed(const glModelPacket* packet);
    void DrawDirect(const glModelPacket* packet);
    void BindParameters(const glModelPacket* packet);

    static GXMaterialProgram_8029AFC4* Instance;
    static bool Initialized;
    static GXMaterialParameter Parameters[2];
};

class GXMaterialProgram_8029B434 : public GXMaterialProgramImpl<GXMaterialProgram_8029B434>
{
public:
    GXMaterialProgram_8029B434();
    virtual ~GXMaterialProgram_8029B434();
    virtual void Configure(glModelPacket* packet);
    virtual const GXMaterialParameter* GetParameters();
    virtual void Initialize();

    void ConfigureVertexFormat(bool indexed);
    void BindVertexArrays(const glModelPacket* packet);
    void DrawIndexed(const glModelPacket* packet);
    void DrawDirect(const glModelPacket* packet);
    void BindParameters(const glModelPacket* packet);

    static GXMaterialProgram_8029B434* Instance;
    static bool Initialized;
    static GXMaterialParameter Parameters[3];
};

class GXMaterialProgram_8029BA04 : public GXMaterialProgramImpl<GXMaterialProgram_8029BA04>
{
public:
    GXMaterialProgram_8029BA04();
    virtual ~GXMaterialProgram_8029BA04();
    virtual void Configure(glModelPacket* packet);
    virtual const GXMaterialParameter* GetParameters();
    virtual void Initialize();

    void ConfigureVertexFormat(bool indexed);
    void BindVertexArrays(const glModelPacket* packet);
    void BindParameters(const glModelPacket* packet);

    static GXMaterialProgram_8029BA04* Instance;
    static bool Initialized;
    static GXMaterialParameter Parameters[37];
};

class GXMaterialProgram_8029BC9C : public GXMaterialProgramImpl<GXMaterialProgram_8029BC9C>
{
public:
    GXMaterialProgram_8029BC9C();
    virtual ~GXMaterialProgram_8029BC9C();
    virtual void Configure(glModelPacket* packet);
    virtual const GXMaterialParameter* GetParameters();
    virtual void Initialize();

    void ConfigureVertexFormat(bool indexed);
    void BindVertexArrays(const glModelPacket* packet);
    void DrawIndexed(const glModelPacket* packet);
    void DrawDirect(const glModelPacket* packet);
    void BindParameters(const glModelPacket* packet);

    static GXMaterialProgram_8029BC9C* Instance;
    static bool Initialized;
    static GXMaterialParameter Parameters[6];
};

class GXMaterialProgram_8029C2F8 : public GXMaterialProgramImpl<GXMaterialProgram_8029C2F8>
{
public:
    GXMaterialProgram_8029C2F8();
    virtual ~GXMaterialProgram_8029C2F8();
    virtual void Configure(glModelPacket* packet);
    virtual const GXMaterialParameter* GetParameters();
    virtual void Initialize();

    void ConfigureVertexFormat(bool indexed);
    void BindVertexArrays(const glModelPacket* packet);
    void DrawIndexed(const glModelPacket* packet);
    void DrawDirect(const glModelPacket* packet);
    void BindParameters(const glModelPacket* packet);

    static GXMaterialProgram_8029C2F8* Instance;
    static bool Initialized;
    static GXMaterialParameter Parameters[9];
};

class GXMaterialProgram_8029C9F0 : public GXMaterialProgramImpl<GXMaterialProgram_8029C9F0>
{
public:
    GXMaterialProgram_8029C9F0();
    virtual ~GXMaterialProgram_8029C9F0();
    virtual void Configure(glModelPacket* packet);
    virtual const GXMaterialParameter* GetParameters();
    virtual void Initialize();

    void ConfigureVertexFormat(bool indexed);
    void BindVertexArrays(const glModelPacket* packet);
    void DrawIndexed(const glModelPacket* packet);
    void DrawDirect(const glModelPacket* packet);
    void BindParameters(const glModelPacket* packet);

    static GXMaterialProgram_8029C9F0* Instance;
    static bool Initialized;
    static GXMaterialParameter Parameters[14];
};

class GXMaterialProgram_8029D0E8 : public GXMaterialProgramImpl<GXMaterialProgram_8029D0E8>
{
public:
    GXMaterialProgram_8029D0E8();
    virtual ~GXMaterialProgram_8029D0E8();
    virtual void Configure(glModelPacket* packet);
    virtual const GXMaterialParameter* GetParameters();
    virtual void Initialize();

    void ConfigureVertexFormat(bool indexed);
    void BindVertexArrays(const glModelPacket* packet);
    void DrawIndexed(const glModelPacket* packet);
    void DrawDirect(const glModelPacket* packet);
    void BindParameters(const glModelPacket* packet);

    static GXMaterialProgram_8029D0E8* Instance;
    static bool Initialized;
    static GXMaterialParameter Parameters[10];
};

class GXMaterialProgram_8029D7E0 : public GXMaterialProgramImpl<GXMaterialProgram_8029D7E0>
{
public:
    GXMaterialProgram_8029D7E0();
    virtual ~GXMaterialProgram_8029D7E0();
    virtual void Configure(glModelPacket* packet);
    virtual const GXMaterialParameter* GetParameters();
    virtual void Initialize();

    void ConfigureVertexFormat(bool indexed);
    void BindVertexArrays(const glModelPacket* packet);
    void DrawIndexed(const glModelPacket* packet);
    void DrawDirect(const glModelPacket* packet);
    void BindParameters(const glModelPacket* packet);

    static GXMaterialProgram_8029D7E0* Instance;
    static bool Initialized;
    static GXMaterialParameter Parameters[12];
};

class GXMaterialProgram_8029DE3C : public GXMaterialProgramImpl<GXMaterialProgram_8029DE3C>
{
public:
    GXMaterialProgram_8029DE3C();
    virtual ~GXMaterialProgram_8029DE3C();
    virtual void Configure(glModelPacket* packet);
    virtual const GXMaterialParameter* GetParameters();
    virtual void Initialize();

    void ConfigureVertexFormat(bool indexed);
    void BindVertexArrays(const glModelPacket* packet);
    void DrawIndexed(const glModelPacket* packet);
    void DrawDirect(const glModelPacket* packet);
    void BindParameters(const glModelPacket* packet);

    static GXMaterialProgram_8029DE3C* Instance;
    static bool Initialized;
    static GXMaterialParameter Parameters[8];
};

class GXMaterialProgram_8029E338 : public GXMaterialProgramImpl<GXMaterialProgram_8029E338>
{
public:
    GXMaterialProgram_8029E338();
    virtual ~GXMaterialProgram_8029E338();
    virtual void Configure(glModelPacket* packet);
    virtual const GXMaterialParameter* GetParameters();
    virtual void Initialize();

    void ConfigureVertexFormat(bool indexed);
    void BindVertexArrays(const glModelPacket* packet);
    void DrawIndexed(const glModelPacket* packet);
    void DrawDirect(const glModelPacket* packet);
    void BindParameters(const glModelPacket* packet);

    static GXMaterialProgram_8029E338* Instance;
    static bool Initialized;
    static GXMaterialParameter Parameters[5];
};

class GXMaterialProgram_8029E8F8 : public GXMaterialProgramImpl<GXMaterialProgram_8029E8F8>
{
public:
    GXMaterialProgram_8029E8F8();
    virtual ~GXMaterialProgram_8029E8F8();
    virtual void Configure(glModelPacket* packet);
    virtual const GXMaterialParameter* GetParameters();
    virtual void Initialize();

    void ConfigureVertexFormat(bool indexed);
    void BindVertexArrays(const glModelPacket* packet);
    void DrawIndexed(const glModelPacket* packet);
    void DrawDirect(const glModelPacket* packet);
    void BindParameters(const glModelPacket* packet);

    static GXMaterialProgram_8029E8F8* Instance;
    static bool Initialized;
    static GXMaterialParameter Parameters[9];
};

class GXMaterialProgram_8029EF54 : public GXMaterialProgramImpl<GXMaterialProgram_8029EF54>
{
public:
    GXMaterialProgram_8029EF54();
    virtual ~GXMaterialProgram_8029EF54();
    virtual void Configure(glModelPacket* packet);
    virtual const GXMaterialParameter* GetParameters();
    virtual void Initialize();

    void ConfigureVertexFormat(bool indexed);
    void BindVertexArrays(const glModelPacket* packet);
    void DrawIndexed(const glModelPacket* packet);
    void DrawDirect(const glModelPacket* packet);
    void BindParameters(const glModelPacket* packet);

    static GXMaterialProgram_8029EF54* Instance;
    static bool Initialized;
    static GXMaterialParameter Parameters[9];
};

class GXMaterialProgram_8029F5B0 : public GXMaterialProgramImpl<GXMaterialProgram_8029F5B0>
{
public:
    GXMaterialProgram_8029F5B0();
    virtual ~GXMaterialProgram_8029F5B0();
    virtual void Configure(glModelPacket* packet);
    virtual const GXMaterialParameter* GetParameters();
    virtual void Initialize();

    void ConfigureVertexFormat(bool indexed);
    void BindVertexArrays(const glModelPacket* packet);
    void DrawIndexed(const glModelPacket* packet);
    void DrawDirect(const glModelPacket* packet);
    void BindParameters(const glModelPacket* packet);

    static GXMaterialProgram_8029F5B0* Instance;
    static bool Initialized;
    static GXMaterialParameter Parameters[12];
};

class GXMaterialProgram_8029FC0C : public GXMaterialProgramImpl<GXMaterialProgram_8029FC0C>
{
public:
    GXMaterialProgram_8029FC0C();
    virtual ~GXMaterialProgram_8029FC0C();
    virtual void Configure(glModelPacket* packet);
    virtual const GXMaterialParameter* GetParameters();
    virtual void Initialize();

    void ConfigureVertexFormat(bool indexed);
    void BindVertexArrays(const glModelPacket* packet);
    void DrawIndexed(const glModelPacket* packet);
    void DrawDirect(const glModelPacket* packet);
    void BindParameters(const glModelPacket* packet);

    static GXMaterialProgram_8029FC0C* Instance;
    static bool Initialized;
    static GXMaterialParameter Parameters[10];
};

class GXMaterialProgram_802A01CC : public GXMaterialProgramImpl<GXMaterialProgram_802A01CC>
{
public:
    GXMaterialProgram_802A01CC();
    virtual ~GXMaterialProgram_802A01CC();
    virtual void Configure(glModelPacket* packet);
    virtual const GXMaterialParameter* GetParameters();
    virtual void Initialize();

    void ConfigureVertexFormat(bool indexed);
    void BindVertexArrays(const glModelPacket* packet);
    void DrawIndexed(const glModelPacket* packet);
    void DrawDirect(const glModelPacket* packet);

    static GXMaterialProgram_802A01CC* Instance;
    static bool Initialized;
    static GXMaterialParameter Parameters[2];
};

class GXWarbleMaterialProgram : public GXMaterialProgramImpl<GXWarbleMaterialProgram>
{
public:
    GXWarbleMaterialProgram();
    virtual ~GXWarbleMaterialProgram();
    virtual void Configure(glModelPacket* packet);
    virtual const GXMaterialParameter* GetParameters();
    virtual void Initialize();

    void ConfigureVertexFormat(bool indexed);
    void BindVertexArrays(const glModelPacket* packet);
    void DrawIndexed(const glModelPacket* packet);
    void DrawDirect(const glModelPacket* packet);
    void BindParameters(const glModelPacket* packet);

    static GXWarbleMaterialProgram* Instance;
    static bool Initialized;
    static GXMaterialParameter Parameters[1];
};

class GXMaterialProgram_802A3EF0 : public GXMaterialProgramImpl<GXMaterialProgram_802A3EF0>
{
public:
    GXMaterialProgram_802A3EF0();
    virtual ~GXMaterialProgram_802A3EF0();
    virtual void Configure(glModelPacket* packet);
    virtual const GXMaterialParameter* GetParameters();
    virtual void Initialize();

    void ConfigureVertexFormat(bool indexed);
    void BindVertexArrays(const glModelPacket* packet);
    void DrawIndexed(const glModelPacket* packet);
    void DrawDirect(const glModelPacket* packet);
    void BindParameters(const glModelPacket* packet);

    static GXMaterialProgram_802A3EF0* Instance;
    static bool Initialized;
    static GXMaterialParameter Parameters[1];
};

class GXConstantColourMaterialProgram : public GXMaterialProgramImpl<GXConstantColourMaterialProgram>
{
public:
    GXConstantColourMaterialProgram();
    virtual ~GXConstantColourMaterialProgram();
    virtual void Configure(glModelPacket* packet);
    virtual const GXMaterialParameter* GetParameters();
    virtual void Initialize();

    void ConfigureVertexFormat(bool indexed);
    void BindVertexArrays(const glModelPacket* packet);
    void DrawIndexed(const glModelPacket* packet);
    void DrawDirect(const glModelPacket* packet);
    void BindParameters(const glModelPacket* packet);

    static GXConstantColourMaterialProgram* Instance;
    static bool Initialized;
    static GXMaterialParameter Parameters[2];
};

class GXMaterialProgram_802A4744 : public GXMaterialProgramImpl<GXMaterialProgram_802A4744>
{
public:
    GXMaterialProgram_802A4744();
    virtual ~GXMaterialProgram_802A4744();
    virtual void Configure(glModelPacket* packet);
    virtual const GXMaterialParameter* GetParameters();
    virtual void Initialize();

    void ConfigureVertexFormat(bool indexed);
    void BindVertexArrays(const glModelPacket* packet);
    void DrawIndexed(const glModelPacket* packet);
    void DrawDirect(const glModelPacket* packet);
    void BindParameters(const glModelPacket* packet);

    static GXMaterialProgram_802A4744* Instance;
    static bool Initialized;
    static GXMaterialParameter Parameters[2];
};

class GXMaterialProgram_802A4B28 : public GXMaterialProgramImpl<GXMaterialProgram_802A4B28>
{
public:
    GXMaterialProgram_802A4B28();
    virtual ~GXMaterialProgram_802A4B28();
    virtual void Configure(glModelPacket* packet);
    virtual const GXMaterialParameter* GetParameters();
    virtual void Initialize();

    void ConfigureVertexFormat(bool indexed);
    void BindVertexArrays(const glModelPacket* packet);
    void DrawIndexed(const glModelPacket* packet);
    void DrawDirect(const glModelPacket* packet);
    void BindParameters(const glModelPacket* packet);

    static GXMaterialProgram_802A4B28* Instance;
    static bool Initialized;
    static GXMaterialParameter Parameters[1];
};

class GXMaterialProgram_802A4F0C : public GXMaterialProgramImpl<GXMaterialProgram_802A4F0C>
{
public:
    GXMaterialProgram_802A4F0C();
    virtual ~GXMaterialProgram_802A4F0C();
    virtual void Configure(glModelPacket* packet);
    virtual const GXMaterialParameter* GetParameters();
    virtual void Initialize();

    void ConfigureVertexFormat(bool indexed);
    void BindVertexArrays(const glModelPacket* packet);
    void DrawIndexed(const glModelPacket* packet);
    void DrawDirect(const glModelPacket* packet);
    void BindParameters(const glModelPacket* packet);

    static GXMaterialProgram_802A4F0C* Instance;
    static bool Initialized;
    static GXMaterialParameter Parameters[3];
};

class GXMaterialProgram_802A53B4 : public GXMaterialProgramImpl<GXMaterialProgram_802A53B4>
{
public:
    GXMaterialProgram_802A53B4();
    virtual ~GXMaterialProgram_802A53B4();
    virtual void Configure(glModelPacket* packet);
    virtual const GXMaterialParameter* GetParameters();
    virtual void Initialize();

    void ConfigureVertexFormat(bool indexed);
    void BindVertexArrays(const glModelPacket* packet);
    void DrawIndexed(const glModelPacket* packet, unsigned char colour);
    void DrawDirect(const glModelPacket* packet, unsigned char colour);
    void BindParameters(const glModelPacket* packet);

    static GXMaterialProgram_802A53B4* Instance;
    static bool Initialized;
    static GXMaterialParameter Parameters[3];
};

class GXMaterialProgram_802A58E8 : public GXMaterialProgramImpl<GXMaterialProgram_802A58E8>
{
public:
    GXMaterialProgram_802A58E8();
    virtual ~GXMaterialProgram_802A58E8();
    virtual void Configure(glModelPacket* packet);
    virtual const GXMaterialParameter* GetParameters();
    virtual void Initialize();

    void ConfigureVertexFormat(bool indexed);
    void BindVertexArrays(const glModelPacket* packet);
    void DrawIndexed(const glModelPacket* packet);
    void DrawDirect(const glModelPacket* packet);
    void BindParameters(const glModelPacket* packet);

    static GXMaterialProgram_802A58E8* Instance;
    static bool Initialized;
    static GXMaterialParameter Parameters[4];
};

class GXMaterialProgram_802A5D58 : public GXMaterialProgramImpl<GXMaterialProgram_802A5D58>
{
public:
    GXMaterialProgram_802A5D58();
    virtual ~GXMaterialProgram_802A5D58();
    virtual void Configure(glModelPacket* packet);
    virtual const GXMaterialParameter* GetParameters();
    virtual void Initialize();

    void ConfigureVertexFormat(bool indexed);
    void BindVertexArrays(const glModelPacket* packet);
    void DrawIndexed(const glModelPacket* packet);
    void DrawDirect(const glModelPacket* packet);
    void BindParameters(const glModelPacket* packet);

    static GXMaterialProgram_802A5D58* Instance;
    static bool Initialized;
    static GXMaterialParameter Parameters[8];
};

class GXMaterialProgram_802A61C8 : public GXMaterialProgramImpl<GXMaterialProgram_802A61C8>
{
public:
    GXMaterialProgram_802A61C8();
    virtual ~GXMaterialProgram_802A61C8();
    virtual void Configure(glModelPacket* packet);
    virtual const GXMaterialParameter* GetParameters();
    virtual void Initialize();

    void ConfigureVertexFormat(bool indexed);
    void BindVertexArrays(const glModelPacket* packet);
    void BindParameters(const glModelPacket* packet);

    static GXMaterialProgram_802A61C8* Instance;
    static bool Initialized;
    static GXMaterialParameter Parameters[2];
};

class GXMaterialProgram_802A63D8 : public GXMaterialProgramImpl<GXMaterialProgram_802A63D8>
{
public:
    GXMaterialProgram_802A63D8();
    virtual ~GXMaterialProgram_802A63D8();
    virtual void Configure(glModelPacket* packet);
    virtual const GXMaterialParameter* GetParameters();
    virtual void Initialize();

    void ConfigureVertexFormat(bool indexed);
    void BindVertexArrays(const glModelPacket* packet);
    void DrawIndexed(const glModelPacket* packet);
    void DrawDirect(const glModelPacket* packet);
    void BindParameters(const glModelPacket* packet);

    static GXMaterialProgram_802A63D8* Instance;
    static bool Initialized;
    static GXMaterialParameter Parameters[1];
};

class GXMaterialProgram_802A6848 : public GXMaterialProgramImpl<GXMaterialProgram_802A6848>
{
public:
    GXMaterialProgram_802A6848();
    virtual ~GXMaterialProgram_802A6848();
    virtual void Configure(glModelPacket* packet);
    virtual const GXMaterialParameter* GetParameters();
    virtual void Initialize();

    void ConfigureVertexFormat(bool indexed);
    void BindVertexArrays(const glModelPacket* packet);
    void DrawIndexed(const glModelPacket* packet);
    void DrawDirect(const glModelPacket* packet);
    void BindParameters(const glModelPacket* packet);

    static GXMaterialProgram_802A6848* Instance;
    static bool Initialized;
};

class GXMaterialProgram_802A6FDC : public GXMaterialProgramImpl<GXMaterialProgram_802A6FDC>
{
public:
    GXMaterialProgram_802A6FDC();
    virtual ~GXMaterialProgram_802A6FDC();
    virtual void Configure(glModelPacket* packet);
    virtual const GXMaterialParameter* GetParameters();
    virtual void Initialize();

    void ConfigureVertexFormat(bool indexed);
    void BindVertexArrays(const glModelPacket* packet);
    void DrawIndexed(const glModelPacket* packet);
    void DrawDirect(const glModelPacket* packet);
    void BindParameters(const glModelPacket* packet);

    static GXMaterialProgram_802A6FDC* Instance;
    static bool Initialized;
};

class GXShadowVolumeMaterialProgram : public GXMaterialProgramImpl<GXShadowVolumeMaterialProgram>
{
public:
    GXShadowVolumeMaterialProgram();
    virtual ~GXShadowVolumeMaterialProgram();
    virtual void Configure(glModelPacket* packet);
    virtual const GXMaterialParameter* GetParameters();
    virtual void Initialize();

    void ConfigureVertexFormat(bool indexed);
    void BindVertexArrays(const glModelPacket* packet);
    void DrawIndexed(const glModelPacket* packet);
    void DrawDirect(const glModelPacket* packet);
    void BindParameters(const glModelPacket* packet);

    static GXShadowVolumeMaterialProgram* Instance;
    static bool Initialized;
    static GXMaterialParameter Parameters[2];
};

class GXMaterialProgram_802A7820 : public GXMaterialProgramImpl<GXMaterialProgram_802A7820>
{
public:
    GXMaterialProgram_802A7820();
    virtual ~GXMaterialProgram_802A7820();
    virtual void Configure(glModelPacket* packet);
    virtual const GXMaterialParameter* GetParameters();
    virtual void Initialize();

    void ConfigureVertexFormat(bool indexed);
    void BindVertexArrays(const glModelPacket* packet);
    void DrawIndexed(const glModelPacket* packet);
    void DrawDirect(const glModelPacket* packet);
    void BindParameters(const glModelPacket* packet);

    static GXMaterialProgram_802A7820* Instance;
    static bool Initialized;
    static GXMaterialParameter Parameters[1];
};

#endif // NL_GLX_GX_MATERIAL_PROGRAM_H
