#include <revolution/gx.h>

#include "NL/glx/GXMaterialProgram.h"

GXMaterialProgram_80299A90* GXMaterialProgram_80299A90::Instance;
bool GXMaterialProgram_80299A90::Initialized;

GXMaterialParameter GXMaterialProgram_80299A90::Parameters[2] = {
    { 0x69F44DC5, 0x01010103, 0 },
    { 0xFB3B01EC, 0x02030411, 8 },
};

GXMaterialProgram_80299A90::GXMaterialProgram_80299A90()
{
    Instance = this;
    programHash = 0xBACEA013;
    parameterDataSize = 16;
    parameterCount = 2;
    fn_802CB790(this, programHash);
}

GXMaterialProgram_80299A90::~GXMaterialProgram_80299A90()
{
}

void GXMaterialProgram_80299A90::Initialize()
{
    if (Initialized)
        return;
    Initialized = true;
}

void GXMaterialProgram_80299A90::Configure()
{
}

void GXMaterialProgram_80299A90::ConfigureVertexFormat(bool indexed)
{
    GXClearVtxDesc();
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_NRM, GX_NRM_XYZ, GX_F32, 0);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_S16, 10);

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

void GXMaterialProgram_80299A90::BindVertexArrays(const glModelPacket* packet)
{
    glModelStream* streams = packet->streams;
    GXSetArray(GX_VA_POS, streams[0].address, 12);
    GXSetArray(GX_VA_NRM, streams[1].address, 12);
    GXSetArray(GX_VA_TEX0, streams[2].address, 4);
}

void GXMaterialProgram_80299A90::BindParameters(const glModelPacket* packet)
{
    fn_8036BE88(0, packet->unknown20);
}

const GXMaterialParameter* GXMaterialProgram_80299A90::GetParameters()
{
    return Parameters;
}
