#include <revolution/gx.h>

#include "NL/glx/GXMaterialProgram.h"

GXMaterialProgram_80299490* GXMaterialProgram_80299490::Instance;
bool GXMaterialProgram_80299490::Initialized;

GXMaterialParameter GXMaterialProgram_80299490::Parameters[13] = {
    { 0x69F44DC5, 0x01010103, 0 },
    { 0xEBAF55D2, 0x01010103, 8 },
    { 0x93014DE7, 0x01010103, 16 },
    { 0x0ED8A325, 0x01010103, 24 },
    { 0xFB3B01EC, 0x02030411, 32 },
    { 0x0658BB38, 0x01010101, 40 },
    { 0xB46C81A2, 0x01010101, 44 },
    { 0x29D1D576, 0x01010101, 48 },
    { 0xCCBCF02F, 0x01010101, 52 },
    { 0x4FBDBBF2, 0x01040101, 56 },
    { 0xAD07B63E, 0x01010101, 72 },
    { 0x46CCF41D, 0x01010102, 76 },
    { 0x8E10B600, 0x01010102, 80 },
};

GXMaterialProgram_80299490::GXMaterialProgram_80299490()
{
    Instance = this;
    programHash = 0xF540EBBA;
    parameterDataSize = 84;
    parameterCount = 13;
    fn_802CB790(this, programHash);
}

GXMaterialProgram_80299490::~GXMaterialProgram_80299490()
{
}

void GXMaterialProgram_80299490::Initialize()
{
    if (Initialized)
        return;
    Initialized = true;
}

void GXMaterialProgram_80299490::Configure()
{
}

void GXMaterialProgram_80299490::ConfigureVertexFormat(bool indexed)
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

void GXMaterialProgram_80299490::BindVertexArrays(const glModelPacket* packet)
{
    glModelStream* streams = packet->streams;
    GXSetArray(GX_VA_POS, streams[0].address, 12);
    GXSetArray(GX_VA_NRM, streams[1].address, 12);
    GXSetArray(GX_VA_TEX0, streams[2].address, 4);
    GXSetArray(GX_VA_TEX1, streams[3].address, 4);
    GXSetArray(GX_VA_TEX2, streams[4].address, 4);
    GXSetArray(GX_VA_TEX3, streams[5].address, 4);
}

void GXMaterialProgram_80299490::BindParameters(const glModelPacket* packet)
{
    fn_8036BE88(0, packet->unknown20);
    fn_8036BE88(1, (unsigned char*)packet->unknown20 + 8);
    fn_8036BE88(2, (unsigned char*)packet->unknown20 + 16);
    fn_8036BE88(3, (unsigned char*)packet->unknown20 + 24);
}

const GXMaterialParameter* GXMaterialProgram_80299490::GetParameters()
{
    return Parameters;
}
