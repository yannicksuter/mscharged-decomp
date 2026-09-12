#include <revolution/gx.h>

#include "NL/glx/GXCharacterDamageMaterialProgram.h"
#include "NL/glx/glxTexture.h"
#include "NL/glx/GXMaterialProgramInternal.h"
#include "Game/UnidentifiedStaticStorage.h"

GXCharacterDamageMaterialProgram* GXCharacterDamageMaterialProgram::Instance;
bool GXCharacterDamageMaterialProgram::Initialized;

GXMaterialParameter GXCharacterDamageMaterialProgram::Parameters[19] = {
    { 0x69F44DC5, 0x01010103, 0 }, // NLG_DIFFUSE
    { 0x98A598DE, 0x01010103, 8 }, // NLG_SPECULAR
    { 0x80968AEB, 0x01010103, 16 }, // NLG_MASK
    { 0x7955E3EB, 0x01010103, 24 }, // megaTexture
    { 0x57C6B8CF, 0x01010103, 32 }, // damage1Texture
    { 0x57C6B8D0, 0x01010103, 40 }, // damage2Texture
    { 0xFB3B01EC, 0x02030411, 48 }, // SkinMatrices
    { 0xB46C81A2, 0x01010101, 56 }, // alphaValue
    { 0xF01F1D00, 0x01010101, 60 }, // specularAmount
    { 0xA9AE313C, 0x01010101, 64 }, // specularScaleX
    { 0x710D1571, 0x01010101, 68 }, // specularScaleY
    { 0x15CAAD1F, 0x01010102, 72 }, // fresnelRamp
    { 0xAD07B63E, 0x01010101, 76 }, // megaBlend
    { 0x89DEEB79, 0x01010102, 80 }, // Unidentified control
    { 0x46CCF41D, 0x01010102, 84 }, // shadowLevel
    { 0x8E10B600, 0x01010102, 88 }, // lightingEnabled
    { 0x8E89F7EF, 0x01010102, 92 }, // blackOnly
    { 0x28E823DA, 0x01010102, 96 }, // damage1Enabled
    { 0x1529F8BB, 0x01010102, 100 }, // damage2Enabled
};

GXCharacterDamageMaterialProgram::GXCharacterDamageMaterialProgram()
{
    Instance = this;
    programHash = 0x1ACE1D01;
    parameterDataSize = 104;
    parameterCount = 19;
    glRegisterMaterialProgram(this, programHash);
}

GXCharacterDamageMaterialProgram::~GXCharacterDamageMaterialProgram()
{
}

void GXCharacterDamageMaterialProgram::Initialize()
{
    if (Initialized)
        return;
    Initialized = true;
}

void GXCharacterDamageMaterialProgram::Configure(glModelPacket*)
{
}

void GXCharacterDamageMaterialProgram::ConfigureVertexFormat(bool indexed)
{
    GXClearVtxDesc();
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_NRM, GX_NRM_XYZ, GX_F32, 0);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_S16, 10);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX1, GX_TEX_ST, GX_S16, 10);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX2, GX_TEX_ST, GX_S16, 10);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX3, GX_TEX_ST, GX_S16, 10);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX4, GX_TEX_ST, GX_S16, 10);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX5, GX_TEX_ST, GX_S16, 10);

    if (indexed)
    {
        GXSetVtxDesc(GX_VA_POS, GX_INDEX16);
        GXSetVtxDesc(GX_VA_NRM, GX_INDEX16);
        GXSetVtxDesc(GX_VA_TEX0, GX_INDEX16);
        GXSetVtxDesc(GX_VA_TEX1, GX_INDEX16);
        GXSetVtxDesc(GX_VA_TEX2, GX_INDEX16);
        GXSetVtxDesc(GX_VA_TEX3, GX_INDEX16);
        GXSetVtxDesc(GX_VA_TEX4, GX_INDEX16);
        GXSetVtxDesc(GX_VA_TEX5, GX_INDEX16);
    }
    else
    {
        GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
        GXSetVtxDesc(GX_VA_NRM, GX_DIRECT);
        GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);
        GXSetVtxDesc(GX_VA_TEX1, GX_DIRECT);
        GXSetVtxDesc(GX_VA_TEX2, GX_DIRECT);
        GXSetVtxDesc(GX_VA_TEX3, GX_DIRECT);
        GXSetVtxDesc(GX_VA_TEX4, GX_DIRECT);
        GXSetVtxDesc(GX_VA_TEX5, GX_DIRECT);
    }
}

void GXCharacterDamageMaterialProgram::BindVertexArrays(const glModelPacket* packet)
{
    glModelStream* streams = packet->streams;
    GXSetArray(GX_VA_POS, streams[0].address, 12);
    GXSetArray(GX_VA_NRM, streams[1].address, 12);
    GXSetArray(GX_VA_TEX0, streams[2].address, 4);
    GXSetArray(GX_VA_TEX1, streams[3].address, 4);
    GXSetArray(GX_VA_TEX2, streams[4].address, 4);
    GXSetArray(GX_VA_TEX3, streams[5].address, 4);
    GXSetArray(GX_VA_TEX4, streams[6].address, 4);
    GXSetArray(GX_VA_TEX5, streams[7].address, 4);
}

void GXCharacterDamageMaterialProgram::BindParameters(const glModelPacket* packet)
{
    glx_BindTexture(0, &static_cast<GXCharacterDamageParameters*>(packet->materialParameters)->diffuseTexture);
    glx_BindTexture(1, &static_cast<GXCharacterDamageParameters*>(packet->materialParameters)->specularTexture);
    glx_BindTexture(2, &static_cast<GXCharacterDamageParameters*>(packet->materialParameters)->specularMaskTexture);
    glx_BindTexture(3, &static_cast<GXCharacterDamageParameters*>(packet->materialParameters)->megaTexture);
    glx_BindTexture(4, &static_cast<GXCharacterDamageParameters*>(packet->materialParameters)->damage1Texture);
    glx_BindTexture(5, &static_cast<GXCharacterDamageParameters*>(packet->materialParameters)->damage2Texture);
}

const GXMaterialParameter* GXCharacterDamageMaterialProgram::GetParameters()
{
    return Parameters;
}
