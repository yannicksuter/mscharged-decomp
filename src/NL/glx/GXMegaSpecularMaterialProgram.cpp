#include <revolution/gx.h>

#include "NL/glx/GXMegaSpecularMaterialProgram.h"
#include "NL/glx/glxTexture.h"
#include "NL/glx/GXMaterialProgramInternal.h"
#include "Game/UnidentifiedStaticStorage.h"

GXMegaSpecularMaterialProgram* GXMegaSpecularMaterialProgram::Instance;
bool GXMegaSpecularMaterialProgram::Initialized;

GXMaterialParameter GXMegaSpecularMaterialProgram::Parameters[13] = {
    { 0x69F44DC5, 0x01010103, 0 }, // NLG_DIFFUSE
    { 0xEBAF55D2, 0x01010103, 8 }, // NLG_DETAIL
    { 0x93014DE7, 0x01010103, 16 }, // NLG_GLOSS
    { 0x0ED8A325, 0x01010103, 24 }, // NLG_SHADOW (megaTexture)
    { 0xFB3B01EC, 0x02030411, 32 }, // SkinMatrices
    { 0x0658BB38, 0x01010101, 40 }, // blendAmount
    { 0xB46C81A2, 0x01010101, 44 }, // alphaValue
    { 0x29D1D576, 0x01010101, 48 }, // specularLevel
    { 0xCCBCF02F, 0x01010101, 52 }, // specularExponent
    { 0x4FBDBBF2, 0x01040101, 56 }, // specularColour
    { 0xAD07B63E, 0x01010101, 72 }, // megaBlend
    { 0x46CCF41D, 0x01010102, 76 }, // shadowLevel
    { 0x8E10B600, 0x01010102, 80 }, // lightingEnabled
};

GXMegaSpecularMaterialProgram::GXMegaSpecularMaterialProgram()
{
    Instance = this;
    programHash = 0xF540EBBA;
    parameterDataSize = 84;
    parameterCount = 13;
    glRegisterMaterialProgram(this, programHash);
}

GXMegaSpecularMaterialProgram::~GXMegaSpecularMaterialProgram()
{
}

void GXMegaSpecularMaterialProgram::Initialize()
{
    if (Initialized)
        return;
    Initialized = true;
}

void GXMegaSpecularMaterialProgram::Configure(glModelPacket*)
{
}

void GXMegaSpecularMaterialProgram::ConfigureVertexFormat(bool indexed)
{
    GXClearVtxDesc();
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_NRM, GX_NRM_XYZ, GX_F32, 0);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_S16, 10);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX1, GX_TEX_ST, GX_S16, 10);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX2, GX_TEX_ST, GX_S16, 10);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX3, GX_TEX_ST, GX_S16, 10);

    if (indexed)
    {
        GXSetVtxDesc(GX_VA_POS, GX_INDEX16);
        GXSetVtxDesc(GX_VA_NRM, GX_INDEX16);
        GXSetVtxDesc(GX_VA_TEX0, GX_INDEX16);
        GXSetVtxDesc(GX_VA_TEX1, GX_INDEX16);
        GXSetVtxDesc(GX_VA_TEX2, GX_INDEX16);
        GXSetVtxDesc(GX_VA_TEX3, GX_INDEX16);
    }
    else
    {
        GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
        GXSetVtxDesc(GX_VA_NRM, GX_DIRECT);
        GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);
        GXSetVtxDesc(GX_VA_TEX1, GX_DIRECT);
        GXSetVtxDesc(GX_VA_TEX2, GX_DIRECT);
        GXSetVtxDesc(GX_VA_TEX3, GX_DIRECT);
    }
}

void GXMegaSpecularMaterialProgram::BindVertexArrays(const glModelPacket* packet)
{
    glModelStream* streams = packet->streams;
    GXSetArray(GX_VA_POS, streams[0].address, 12);
    GXSetArray(GX_VA_NRM, streams[1].address, 12);
    GXSetArray(GX_VA_TEX0, streams[2].address, 4);
    GXSetArray(GX_VA_TEX1, streams[3].address, 4);
    GXSetArray(GX_VA_TEX2, streams[4].address, 4);
    GXSetArray(GX_VA_TEX3, streams[5].address, 4);
}

void GXMegaSpecularMaterialProgram::BindParameters(const glModelPacket* packet)
{
    glx_BindTexture(0, &static_cast<GXMegaSpecularParameters*>(packet->materialParameters)->diffuseTexture);
    glx_BindTexture(1, &static_cast<GXMegaSpecularParameters*>(packet->materialParameters)->detailTexture);
    glx_BindTexture(2, &static_cast<GXMegaSpecularParameters*>(packet->materialParameters)->glossTexture);
    glx_BindTexture(3, &static_cast<GXMegaSpecularParameters*>(packet->materialParameters)->megaTexture);
}

const GXMaterialParameter* GXMegaSpecularMaterialProgram::GetParameters()
{
    return Parameters;
}
