#include <revolution/gx.h>

#include "NL/glx/GXSpecularFresnelMaterialProgram.h"
#include "NL/glx/glxTexture.h"
#include "NL/glx/GXMaterialProgramInternal.h"
#include "Game/UnidentifiedStaticStorage.h"

GXSpecularFresnelMaterialProgram* GXSpecularFresnelMaterialProgram::Instance;
bool GXSpecularFresnelMaterialProgram::Initialized;

GXMaterialParameter GXSpecularFresnelMaterialProgram::Parameters[13] = {
    { 0x69F44DC5, 0x01010103, 0 }, // NLG_DIFFUSE
    { 0xEBAF55D2, 0x01010103, 8 }, // NLG_DETAIL
    { 0x98A598DE, 0x01010103, 16 }, // NLG_SPECULAR
    { 0x80968AEB, 0x01010103, 24 }, // NLG_MASK
    { 0xFB3B01EC, 0x02030411, 32 }, // SkinMatrices
    { 0x0658BB38, 0x01010101, 40 }, // blendAmount
    { 0xB46C81A2, 0x01010101, 44 }, // alphaValue
    { 0xF01F1D00, 0x01010101, 48 }, // specularAmount
    { 0xA9AE313C, 0x01010101, 52 }, // specularScaleX
    { 0x710D1571, 0x01010101, 56 }, // specularScaleY
    { 0x15CAAD1F, 0x01010102, 60 }, // fresnelRamp
    { 0x46CCF41D, 0x01010102, 64 }, // shadowLevel
    { 0x8E10B600, 0x01010102, 68 }, // lightingEnabled
};

GXSpecularFresnelMaterialProgram::GXSpecularFresnelMaterialProgram()
{
    Instance = this;
    programHash = 0x46B46F88;
    parameterDataSize = 72;
    parameterCount = 13;
    glRegisterMaterialProgram(this, programHash);
}

GXSpecularFresnelMaterialProgram::~GXSpecularFresnelMaterialProgram()
{
}

void GXSpecularFresnelMaterialProgram::Initialize()
{
    if (Initialized)
        return;
    Initialized = true;
}

void GXSpecularFresnelMaterialProgram::Configure(glModelPacket*)
{
}

void GXSpecularFresnelMaterialProgram::ConfigureVertexFormat(bool indexed)
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

void GXSpecularFresnelMaterialProgram::BindVertexArrays(const glModelPacket* packet)
{
    glModelStream* streams = packet->streams;
    GXSetArray(GX_VA_POS, streams[0].address, 12);
    GXSetArray(GX_VA_NRM, streams[1].address, 12);
    GXSetArray(GX_VA_TEX0, streams[2].address, 4);
    GXSetArray(GX_VA_TEX1, streams[3].address, 4);
    GXSetArray(GX_VA_TEX2, streams[4].address, 4);
    GXSetArray(GX_VA_TEX3, streams[5].address, 4);
}

void GXSpecularFresnelMaterialProgram::BindParameters(const glModelPacket* packet)
{
    glx_BindTexture(0, &static_cast<GXSpecularFresnelParameters*>(packet->materialParameters)->diffuseTexture);
    glx_BindTexture(1, &static_cast<GXSpecularFresnelParameters*>(packet->materialParameters)->detailTexture);
    glx_BindTexture(2, &static_cast<GXSpecularFresnelParameters*>(packet->materialParameters)->specularTexture);
    glx_BindTexture(3, &static_cast<GXSpecularFresnelParameters*>(packet->materialParameters)->specularMaskTexture);
}

const GXMaterialParameter* GXSpecularFresnelMaterialProgram::GetParameters()
{
    return Parameters;
}
