#include <revolution/gx.h>

#include "NL/glx/GXMaterialProgram.h"

GXMaterialProgram_80298B18* GXMaterialProgram_80298B18::Instance;
bool GXMaterialProgram_80298B18::Initialized;

GXMaterialParameter GXMaterialProgram_80298B18::Parameters[19] = {
    { 0x69F44DC5, 0x01010103, 0 },
    { 0x98A598DE, 0x01010103, 8 },
    { 0x80968AEB, 0x01010103, 16 },
    { 0x7955E3EB, 0x01010103, 24 },
    { 0x57C6B8CF, 0x01010103, 32 },
    { 0x57C6B8D0, 0x01010103, 40 },
    { 0xFB3B01EC, 0x02030411, 48 },
    { 0xB46C81A2, 0x01010101, 56 },
    { 0xF01F1D00, 0x01010101, 60 },
    { 0xA9AE313C, 0x01010101, 64 },
    { 0x710D1571, 0x01010101, 68 },
    { 0x15CAAD1F, 0x01010102, 72 },
    { 0xAD07B63E, 0x01010101, 76 },
    { 0x89DEEB79, 0x01010102, 80 },
    { 0x46CCF41D, 0x01010102, 84 },
    { 0x8E10B600, 0x01010102, 88 },
    { 0x8E89F7EF, 0x01010102, 92 },
    { 0x28E823DA, 0x01010102, 96 },
    { 0x1529F8BB, 0x01010102, 100 },
};

GXMaterialProgram_80298B18::GXMaterialProgram_80298B18()
{
    Instance = this;
    programHash = 0x1ACE1D01;
    parameterDataSize = 104;
    parameterCount = 19;
    fn_802CB790(this, programHash);
}

GXMaterialProgram_80298B18::~GXMaterialProgram_80298B18()
{
}

void GXMaterialProgram_80298B18::Initialize()
{
    if (Initialized)
        return;
    Initialized = true;
}

void GXMaterialProgram_80298B18::Configure()
{
}

void GXMaterialProgram_80298B18::ConfigureVertexFormat(bool indexed)
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

void GXMaterialProgram_80298B18::BindVertexArrays(const glModelPacket* packet)
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

void GXMaterialProgram_80298B18::BindParameters(const glModelPacket* packet)
{
    fn_8036BE88(0, packet->unknown20);
    fn_8036BE88(1, (unsigned char*)packet->unknown20 + 8);
    fn_8036BE88(2, (unsigned char*)packet->unknown20 + 16);
    fn_8036BE88(3, (unsigned char*)packet->unknown20 + 24);
    fn_8036BE88(4, (unsigned char*)packet->unknown20 + 32);
    fn_8036BE88(5, (unsigned char*)packet->unknown20 + 40);
}

const GXMaterialParameter* GXMaterialProgram_80298B18::GetParameters()
{
    return Parameters;
}
