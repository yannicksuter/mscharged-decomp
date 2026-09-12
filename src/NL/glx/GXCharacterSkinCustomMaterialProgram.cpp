#include <revolution/gx.h>

#include "NL/glx/GXCharacterSkinCustomMaterialProgram.h"
#include "NL/glx/GXMaterialProgramInternal.h"
#include "NL/glx/glxTexture.h"
#include "Game/UnidentifiedStaticStorage.h"

GXCharacterSkinCustomMaterialProgram* GXCharacterSkinCustomMaterialProgram::Instance;
bool GXCharacterSkinCustomMaterialProgram::Initialized;

GXMaterialParameter GXCharacterSkinCustomMaterialProgram::Parameters[7] = {
    { 0x69F44DC5, 0x01010103, 0 }, // NLG_DIFFUSE
    { 0xEBAF55D2, 0x01010103, 8 }, // NLG_DETAIL
    { 0xFB3B01EC, 0x02030411, 16 }, // SkinMatrices
    { 0x0658BB38, 0x01010101, 24 }, // blendAmount
    { 0xB46C81A2, 0x01010101, 28 }, // alphaValue
    { 0x46CCF41D, 0x01010102, 32 }, // shadowLevel
    { 0x8E10B600, 0x01010102, 36 }, // lightingEnabled
};

GXCharacterSkinCustomMaterialProgram::GXCharacterSkinCustomMaterialProgram()
{
    Instance = this;
    programHash = 0x041C3281;
    parameterDataSize = 40;
    parameterCount = 7;
    glRegisterMaterialProgram(this, programHash);
}

GXCharacterSkinCustomMaterialProgram::~GXCharacterSkinCustomMaterialProgram()
{
}

void GXCharacterSkinCustomMaterialProgram::Initialize()
{
    if (Initialized)
        return;
    Initialized = true;
}

void GXCharacterSkinCustomMaterialProgram::Configure(glModelPacket*)
{
}

void GXCharacterSkinCustomMaterialProgram::ConfigureVertexFormat(bool indexed)
{
    GXClearVtxDesc();
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_NRM, GX_NRM_XYZ, GX_F32, 0);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_S16, 10);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX1, GX_TEX_ST, GX_S16, 10);

    if (indexed)
    {
        GXSetVtxDesc(GX_VA_POS, GX_INDEX16);
        GXSetVtxDesc(GX_VA_NRM, GX_INDEX16);
        GXSetVtxDesc(GX_VA_TEX0, GX_INDEX16);
        GXSetVtxDesc(GX_VA_TEX1, GX_INDEX16);
    }
    else
    {
        GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
        GXSetVtxDesc(GX_VA_NRM, GX_DIRECT);
        GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);
        GXSetVtxDesc(GX_VA_TEX1, GX_DIRECT);
    }
}

void GXCharacterSkinCustomMaterialProgram::BindVertexArrays(const glModelPacket* packet)
{
    glModelStream* streams = packet->streams;
    GXSetArray(GX_VA_POS, streams[0].address, 12);
    GXSetArray(GX_VA_NRM, streams[1].address, 12);
    GXSetArray(GX_VA_TEX0, streams[2].address, 4);
    GXSetArray(GX_VA_TEX1, streams[3].address, 4);
}

void GXCharacterSkinCustomMaterialProgram::BindParameters(const glModelPacket* packet)
{
    glx_BindTexture(0, &static_cast<GXCharacterSkinCustomParameters*>(packet->materialParameters)->diffuseTexture);
    glx_BindTexture(1, &static_cast<GXCharacterSkinCustomParameters*>(packet->materialParameters)->detailTexture);
}

const GXMaterialParameter* GXCharacterSkinCustomMaterialProgram::GetParameters()
{
    return Parameters;
}
