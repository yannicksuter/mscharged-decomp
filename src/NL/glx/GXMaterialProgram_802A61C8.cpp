#include <revolution/gx.h>

#include "NL/glx/GXMaterialProgram.h"

GXMaterialProgram_802A61C8* GXMaterialProgram_802A61C8::Instance;
bool GXMaterialProgram_802A61C8::Initialized;

GXMaterialParameter GXMaterialProgram_802A61C8::Parameters[2] = {
    { 0x69F44DC5, 0x01010103, 0 },
    { 0xFB3B01EC, 0x02030411, 8 },
};

GXMaterialProgram_802A61C8::GXMaterialProgram_802A61C8()
{
    Instance = this;
    programHash = 0x5D6C62BA;
    parameterDataSize = 16;
    parameterCount = 2;
    fn_802CB790(this, programHash);
}

GXMaterialProgram_802A61C8::~GXMaterialProgram_802A61C8()
{
}

void GXMaterialProgram_802A61C8::Initialize()
{
    if (Initialized)
        return;
    Initialized = true;
}

void GXMaterialProgram_802A61C8::Configure()
{
}

void GXMaterialProgram_802A61C8::ConfigureVertexFormat(bool indexed)
{
    GXClearVtxDesc();
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_NRM, GX_NRM_XYZ, GX_F32, 0);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);

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

void GXMaterialProgram_802A61C8::BindVertexArrays(const glModelPacket* packet)
{
    glModelStream* streams = packet->streams;
    GXSetArray(GX_VA_POS, streams[0].address, 12);
    GXSetArray(GX_VA_NRM, streams[1].address, 12);
    GXSetArray(GX_VA_TEX0, streams[2].address, 8);
}

void GXMaterialProgram_802A61C8::BindParameters(const glModelPacket* packet)
{
    fn_8036BE88(0, packet->unknown20);
}

const GXMaterialParameter* GXMaterialProgram_802A61C8::GetParameters()
{
    return Parameters;
}
