#include <revolution/gx.h>

#include "NL/glx/GXSkinnedUnlitTextureMaterialProgram.h"
#include "NL/glx/glxTexture.h"
#include "NL/glx/GXMaterialProgramInternal.h"
#include "NL/gl/glLoadModel.h"
#include "Game/UnidentifiedStaticStorage.h"

GXSkinnedUnlitTextureMaterialProgram* GXSkinnedUnlitTextureMaterialProgram::Instance;
bool GXSkinnedUnlitTextureMaterialProgram::Initialized;

GXMaterialParameter GXSkinnedUnlitTextureMaterialProgram::Parameters[2] = {
    { 0x69F44DC5, 0x01010103, 0 }, // NLG_DIFFUSE
    { 0xFB3B01EC, 0x02030411, 8 }, // SkinMatrices
};

GXSkinnedUnlitTextureMaterialProgram::GXSkinnedUnlitTextureMaterialProgram()
{
    Instance = this;
    programHash = 0x5D6C62BA;
    parameterDataSize = 16;
    parameterCount = 2;
    glRegisterMaterialProgram(this, programHash);
}

GXSkinnedUnlitTextureMaterialProgram::~GXSkinnedUnlitTextureMaterialProgram()
{
}

void GXSkinnedUnlitTextureMaterialProgram::Initialize()
{
    if (Initialized)
        return;
    Initialized = true;
}

void GXSkinnedUnlitTextureMaterialProgram::Configure(glModelPacket*)
{
}

void GXSkinnedUnlitTextureMaterialProgram::ConfigureVertexFormat(bool indexed)
{
    GXClearVtxDesc();
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_NRM, GX_NRM_XYZ, GX_F32, 0);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);

    if (indexed)
    {
        GXSetVtxDesc(GX_VA_POS, GX_INDEX16);
        GXSetVtxDesc(GX_VA_NRM, GX_INDEX16);
        GXSetVtxDesc(GX_VA_TEX0, GX_INDEX16);
    }
    else
    {
        GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
        GXSetVtxDesc(GX_VA_NRM, GX_DIRECT);
        GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);
    }
}

void GXSkinnedUnlitTextureMaterialProgram::BindVertexArrays(const glModelPacket* packet)
{
    glModelStream* streams = packet->streams;
    GXSetArray(GX_VA_POS, streams[0].address, 12);
    GXSetArray(GX_VA_NRM, streams[1].address, 12);
    GXSetArray(GX_VA_TEX0, streams[2].address, 8);
}

void GXSkinnedUnlitTextureMaterialProgram::BindParameters(const glModelPacket* packet)
{
    glx_BindTexture(0, &static_cast<GXSkinnedUnlitTextureParameters*>(packet->materialParameters)->diffuseTexture);
}

const GXMaterialParameter* GXSkinnedUnlitTextureMaterialProgram::GetParameters()
{
    return Parameters;
}
