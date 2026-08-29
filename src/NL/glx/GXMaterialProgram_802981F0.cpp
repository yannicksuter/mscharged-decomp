#include <revolution/gx.h>

#include "NL/glx/GXMaterialProgram.h"

GXMaterialProgram_802981F0* GXMaterialProgram_802981F0::Instance;
bool GXMaterialProgram_802981F0::Initialized;

GXMaterialParameter GXMaterialProgram_802981F0::Parameters[7] = {
    { 0x69F44DC5, 0x01010103, 0 },
    { 0xEBAF55D2, 0x01010103, 8 },
    { 0xFB3B01EC, 0x02030411, 16 },
    { 0x0658BB38, 0x01010101, 24 },
    { 0xB46C81A2, 0x01010101, 28 },
    { 0x46CCF41D, 0x01010102, 32 },
    { 0x8E10B600, 0x01010102, 36 },
};

GXMaterialProgram_802981F0::GXMaterialProgram_802981F0()
{
    Instance = this;
    programHash = 0x041C3281;
    parameterDataSize = 40;
    parameterCount = 7;
    fn_802CB790(this, programHash);
}

GXMaterialProgram_802981F0::~GXMaterialProgram_802981F0()
{
}

void GXMaterialProgram_802981F0::Initialize()
{
    if (Initialized)
        return;
    Initialized = true;
}

void GXMaterialProgram_802981F0::Configure()
{
}

void GXMaterialProgram_802981F0::ConfigureVertexFormat(bool indexed)
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

void GXMaterialProgram_802981F0::BindVertexArrays(const glModelPacket* packet)
{
    glModelStream* streams = packet->streams;
    GXSetArray(GX_VA_POS, streams[0].address, 12);
    GXSetArray(GX_VA_NRM, streams[1].address, 12);
    GXSetArray(GX_VA_TEX0, streams[2].address, 4);
    GXSetArray(GX_VA_TEX1, streams[3].address, 4);
}

void GXMaterialProgram_802981F0::BindParameters(const glModelPacket* packet)
{
    fn_8036BE88(0, packet->unknown20);
    fn_8036BE88(1, (unsigned char*)packet->unknown20 + 8);
}

const GXMaterialParameter* GXMaterialProgram_802981F0::GetParameters()
{
    return Parameters;
}
