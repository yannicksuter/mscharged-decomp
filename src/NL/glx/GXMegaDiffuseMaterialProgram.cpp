#include <revolution/gx.h>

#include "NL/glx/GXMegaDiffuseMaterialProgram.h"
#include "NL/glx/glxTexture.h"
#include "NL/glx/GXMaterialProgramInternal.h"
#include "Game/UnidentifiedStaticStorage.h"

GXMegaDiffuseMaterialProgram* GXMegaDiffuseMaterialProgram::Instance;
bool GXMegaDiffuseMaterialProgram::Initialized;

GXMaterialParameter GXMegaDiffuseMaterialProgram::Parameters[9] = {
    { 0x69F44DC5, 0x01010103, 0 }, // NLG_DIFFUSE
    { 0xEBAF55D2, 0x01010103, 8 }, // NLG_DETAIL
    { 0x0ED8A325, 0x01010103, 16 }, // NLG_SHADOW (megaTexture)
    { 0xFB3B01EC, 0x02030411, 24 }, // SkinMatrices
    { 0x0658BB38, 0x01010101, 32 }, // blendAmount
    { 0xB46C81A2, 0x01010101, 36 }, // alphaValue
    { 0xAD07B63E, 0x01010101, 40 }, // megaBlend
    { 0x46CCF41D, 0x01010102, 44 }, // shadowLevel
    { 0x8E10B600, 0x01010102, 48 }, // lightingEnabled
};

GXMegaDiffuseMaterialProgram::GXMegaDiffuseMaterialProgram()
{
    Instance = this;
    programHash = 0x44410B9B;
    parameterDataSize = 52;
    parameterCount = 9;
    glRegisterMaterialProgram(this, programHash);
}

GXMegaDiffuseMaterialProgram::~GXMegaDiffuseMaterialProgram()
{
}

void GXMegaDiffuseMaterialProgram::Initialize()
{
    if (Initialized)
        return;
    Initialized = true;
}

void GXMegaDiffuseMaterialProgram::Configure(glModelPacket*)
{
}

void GXMegaDiffuseMaterialProgram::ConfigureVertexFormat(bool indexed)
{
    GXClearVtxDesc();
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_NRM, GX_NRM_XYZ, GX_F32, 0);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_S16, 10);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX1, GX_TEX_ST, GX_S16, 10);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX2, GX_TEX_ST, GX_S16, 10);

    if (indexed)
    {
        GXSetVtxDesc(GX_VA_POS, GX_INDEX16);
        GXSetVtxDesc(GX_VA_NRM, GX_INDEX16);
        GXSetVtxDesc(GX_VA_TEX0, GX_INDEX16);
        GXSetVtxDesc(GX_VA_TEX1, GX_INDEX16);
        GXSetVtxDesc(GX_VA_TEX2, GX_INDEX16);
    }
    else
    {
        GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
        GXSetVtxDesc(GX_VA_NRM, GX_DIRECT);
        GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);
        GXSetVtxDesc(GX_VA_TEX1, GX_DIRECT);
        GXSetVtxDesc(GX_VA_TEX2, GX_DIRECT);
    }
}

void GXMegaDiffuseMaterialProgram::BindVertexArrays(const glModelPacket* packet)
{
    glModelStream* streams = packet->streams;
    GXSetArray(GX_VA_POS, streams[0].address, 12);
    GXSetArray(GX_VA_NRM, streams[1].address, 12);
    GXSetArray(GX_VA_TEX0, streams[2].address, 4);
    GXSetArray(GX_VA_TEX1, streams[3].address, 4);
    GXSetArray(GX_VA_TEX2, streams[4].address, 4);
}

void GXMegaDiffuseMaterialProgram::BindParameters(const glModelPacket* packet)
{
    glx_BindTexture(0, &static_cast<GXMegaDiffuseParameters*>(packet->materialParameters)->diffuseTexture);
    glx_BindTexture(1, &static_cast<GXMegaDiffuseParameters*>(packet->materialParameters)->detailTexture);
    glx_BindTexture(2, &static_cast<GXMegaDiffuseParameters*>(packet->materialParameters)->megaTexture);
}

const GXMaterialParameter* GXMegaDiffuseMaterialProgram::GetParameters()
{
    return Parameters;
}
