#include <revolution/gx.h>

#include "NL/glx/GXMaterialProgram.h"
#include "NL/gl/glLoadModel.h"

GXMaterialProgram_802987A0* GXMaterialProgram_802987A0::Instance;
bool GXMaterialProgram_802987A0::Initialized;

GXMaterialParameter GXMaterialProgram_802987A0::Parameters[16] = {
    { 0x69F44DC5, 0x01010103, 0 },
    { 0xEBAF55D2, 0x01010103, 8 },
    { 0x98A598DE, 0x01010103, 16 },
    { 0x80968AEB, 0x01010103, 24 },
    { 0x0ED8A325, 0x01010103, 32 },
    { 0xFB3B01EC, 0x02030411, 40 },
    { 0x0658BB38, 0x01010101, 48 },
    { 0xB46C81A2, 0x01010101, 52 },
    { 0xF01F1D00, 0x01010101, 56 },
    { 0xA9AE313C, 0x01010101, 60 },
    { 0x710D1571, 0x01010101, 64 },
    { 0x15CAAD1F, 0x01010102, 68 },
    { 0xAD07B63E, 0x01010101, 72 },
    { 0x46CCF41D, 0x01010102, 76 },
    { 0x8E10B600, 0x01010102, 80 },
    { 0x8E89F7EF, 0x01010102, 84 },
};

GXMaterialProgram_802987A0::GXMaterialProgram_802987A0()
{
    Instance = this;
    programHash = 0x632E7422;
    parameterDataSize = 88;
    parameterCount = 16;
    glRegisterMaterialProgram(this, programHash);
}

GXMaterialProgram_802987A0::~GXMaterialProgram_802987A0()
{
}

void GXMaterialProgram_802987A0::Initialize()
{
    if (Initialized)
        return;
    Initialized = true;
}

void GXMaterialProgram_802987A0::Configure(glModelPacket*)
{
}

void GXMaterialProgram_802987A0::ConfigureVertexFormat(bool indexed)
{
    GXClearVtxDesc();
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_NRM, GX_NRM_XYZ, GX_F32, 0);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_S16, 10);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX1, GX_TEX_ST, GX_S16, 10);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX2, GX_TEX_ST, GX_S16, 10);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX3, GX_TEX_ST, GX_S16, 10);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX4, GX_TEX_ST, GX_S16, 10);

    if (indexed)
    {
        GXSetVtxDesc(GX_VA_POS, GX_INDEX16);
        GXSetVtxDesc(GX_VA_NRM, GX_INDEX16);
        GXSetVtxDesc(GX_VA_TEX0, GX_INDEX16);
        GXSetVtxDesc(GX_VA_TEX1, GX_INDEX16);
        GXSetVtxDesc(GX_VA_TEX2, GX_INDEX16);
        GXSetVtxDesc(GX_VA_TEX3, GX_INDEX16);
        GXSetVtxDesc(GX_VA_TEX4, GX_INDEX16);
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
    }
}

void GXMaterialProgram_802987A0::BindVertexArrays(const glModelPacket* packet)
{
    glModelStream* streams = packet->streams;
    GXSetArray(GX_VA_POS, streams[0].address, 12);
    GXSetArray(GX_VA_NRM, streams[1].address, 12);
    GXSetArray(GX_VA_TEX0, streams[2].address, 4);
    GXSetArray(GX_VA_TEX1, streams[3].address, 4);
    GXSetArray(GX_VA_TEX2, streams[4].address, 4);
    GXSetArray(GX_VA_TEX3, streams[5].address, 4);
    GXSetArray(GX_VA_TEX4, streams[6].address, 4);
}

void GXMaterialProgram_802987A0::BindParameters(const glModelPacket* packet)
{
    glx_BindTexture(0, (glTextureBinding*)(packet->unknown20));
    glx_BindTexture(1, (glTextureBinding*)((unsigned char*)packet->unknown20 + 8));
    glx_BindTexture(2, (glTextureBinding*)((unsigned char*)packet->unknown20 + 16));
    glx_BindTexture(3, (glTextureBinding*)((unsigned char*)packet->unknown20 + 24));
    glx_BindTexture(4, (glTextureBinding*)((unsigned char*)packet->unknown20 + 32));
}

const GXMaterialParameter* GXMaterialProgram_802987A0::GetParameters()
{
    return Parameters;
}
