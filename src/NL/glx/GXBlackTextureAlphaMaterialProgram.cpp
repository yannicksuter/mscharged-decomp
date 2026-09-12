#include <revolution/gx.h>

#include "NL/glx/GXBlackTextureAlphaMaterialProgram.h"
#include "NL/glx/glxTexture.h"
#include "NL/glx/GXMaterialProgramInternal.h"
#include "Game/UnidentifiedStaticStorage.h"

GXBlackTextureAlphaMaterialProgram* GXBlackTextureAlphaMaterialProgram::Instance;
bool GXBlackTextureAlphaMaterialProgram::Initialized;

GXMaterialParameter GXBlackTextureAlphaMaterialProgram::Parameters[2] = {
    { 0x69F44DC5, 0x01010103, 0 }, // NLG_DIFFUSE
    { 0xFB3B01EC, 0x02030411, 8 }, // SkinMatrices
};

GXBlackTextureAlphaMaterialProgram::GXBlackTextureAlphaMaterialProgram()
{
    Instance = this;
    programHash = 0xBACEA013;
    parameterDataSize = 16;
    parameterCount = 2;
    glRegisterMaterialProgram(this, programHash);
}

GXBlackTextureAlphaMaterialProgram::~GXBlackTextureAlphaMaterialProgram()
{
}

void GXBlackTextureAlphaMaterialProgram::Initialize()
{
    if (Initialized)
        return;
    Initialized = true;
}

void GXBlackTextureAlphaMaterialProgram::Configure(glModelPacket*)
{
}

void GXBlackTextureAlphaMaterialProgram::ConfigureVertexFormat(bool indexed)
{
    GXClearVtxDesc();
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_NRM, GX_NRM_XYZ, GX_F32, 0);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_S16, 10);

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

void GXBlackTextureAlphaMaterialProgram::BindVertexArrays(const glModelPacket* packet)
{
    glModelStream* streams = packet->streams;
    GXSetArray(GX_VA_POS, streams[0].address, 12);
    GXSetArray(GX_VA_NRM, streams[1].address, 12);
    GXSetArray(GX_VA_TEX0, streams[2].address, 4);
}

void GXBlackTextureAlphaMaterialProgram::BindParameters(const glModelPacket* packet)
{
    glx_BindTexture(0, &static_cast<GXBlackTextureAlphaParameters*>(packet->materialParameters)->diffuseTexture);
}

const GXMaterialParameter* GXBlackTextureAlphaMaterialProgram::GetParameters()
{
    return Parameters;
}
