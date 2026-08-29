#include <revolution/gx.h>

#include "NL/glx/GXMaterialProgram.h"

GXMaterialProgram_8029AFC4* GXMaterialProgram_8029AFC4::Instance;
bool GXMaterialProgram_8029AFC4::Initialized;

GXMaterialParameter GXMaterialProgram_8029AFC4::Parameters[2] = {
    { 0x69F44DC5, 0x01010103, 0 },
    { 0xEADBCE0A, 0x01010102, 8 },
};

GXMaterialProgram_8029AFC4::GXMaterialProgram_8029AFC4()
{
    Instance = this;
    programHash = 0x257C2FF3;
    parameterDataSize = 12;
    parameterCount = 2;
    fn_802CB790(this, programHash);
}

GXMaterialProgram_8029AFC4::~GXMaterialProgram_8029AFC4()
{
}

void GXMaterialProgram_8029AFC4::Initialize()
{
    if (Initialized)
        return;
    Initialized = true;
}

void GXMaterialProgram_8029AFC4::Configure()
{
}

void GXMaterialProgram_8029AFC4::ConfigureVertexFormat(bool indexed)
{
    GXClearVtxDesc();
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_S16, 10);

    if (indexed)
    {
        GXSetVtxDesc(GX_VA_POS, GX_INDEX16);
        GXSetVtxDesc(GX_VA_CLR0, GX_INDEX16);
        GXSetVtxDesc(GX_VA_TEX0, GX_INDEX16);
    }
    else
    {
        GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
        GXSetVtxDesc(GX_VA_CLR0, GX_DIRECT);
        GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);
    }
}

void GXMaterialProgram_8029AFC4::BindVertexArrays(const glModelPacket* packet)
{
    glModelStream* streams = packet->streams;
    GXSetArray(GX_VA_POS, streams[0].address, 12);
    GXSetArray(GX_VA_CLR0, streams[2].address, 4);
    GXSetArray(GX_VA_TEX0, streams[1].address, 4);
}

void GXMaterialProgram_8029AFC4::DrawIndexed(const glModelPacket* packet)
{
    unsigned short* idxPtr = packet->indexBuffer;
    unsigned short* end = idxPtr + packet->numVertices;
    GXBegin(lbl_80524470[(unsigned char)packet->primType], GX_VTXFMT0, (unsigned short)packet->numVertices);

    while (idxPtr < end)
    {
        WGPIPE.us = *idxPtr;
        WGPIPE.us = *idxPtr;
        WGPIPE.us = *idxPtr;
        ++idxPtr;
    }
}

void GXMaterialProgram_8029AFC4::DrawDirect(const glModelPacket* packet)
{
    GXBegin(lbl_80524470[(unsigned char)packet->primType], GX_VTXFMT0, packet->numUniqueVertices);

    for (unsigned short i = 0; i < packet->numUniqueVertices; ++i)
    {
        WGPIPE.us = i;
        WGPIPE.us = i;
        WGPIPE.us = i;
    }
}

void GXMaterialProgram_8029AFC4::BindParameters(const glModelPacket* packet)
{
    fn_8036BE88(0, packet->unknown20);
}

const GXMaterialParameter* GXMaterialProgram_8029AFC4::GetParameters()
{
    return Parameters;
}
