#include <revolution/gx.h>

#include "NL/glx/GXMaterialProgram.h"

GXMaterialProgram_802A5D58* GXMaterialProgram_802A5D58::Instance;
bool GXMaterialProgram_802A5D58::Initialized;

GXMaterialParameter GXMaterialProgram_802A5D58::Parameters[8] = {
    { 0x69F44DC5, 0x01010103, 0 },
    { 0xFB281C86, 0x01030181, 8 },
    { 0xFB281C87, 0x01030181, 20 },
    { 0xFB281C88, 0x01030181, 32 },
    { 0xDA88C73B, 0x01040181, 44 },
    { 0xDA88C73C, 0x01040181, 60 },
    { 0xDA88C73D, 0x01040181, 76 },
    { 0xEF0F57EB, 0x01040181, 92 },
};

GXMaterialProgram_802A5D58::GXMaterialProgram_802A5D58()
{
    Instance = this;
    programHash = 0x8D359080;
    parameterDataSize = 108;
    parameterCount = 8;
    fn_802CB790(this, programHash);
}

GXMaterialProgram_802A5D58::~GXMaterialProgram_802A5D58()
{
}

void GXMaterialProgram_802A5D58::Initialize()
{
    if (Initialized)
        return;
    Initialized = true;
}

void GXMaterialProgram_802A5D58::Configure()
{
}

void GXMaterialProgram_802A5D58::ConfigureVertexFormat(bool indexed)
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

void GXMaterialProgram_802A5D58::BindVertexArrays(const glModelPacket* packet)
{
    glModelStream* streams = packet->streams;
    GXSetArray(GX_VA_POS, streams[0].address, 12);
    GXSetArray(GX_VA_NRM, streams[1].address, 12);
    GXSetArray(GX_VA_TEX0, streams[2].address, 8);
}

void GXMaterialProgram_802A5D58::DrawIndexed(const glModelPacket* packet)
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

void GXMaterialProgram_802A5D58::DrawDirect(const glModelPacket* packet)
{
    GXBegin(lbl_80524470[(unsigned char)packet->primType], GX_VTXFMT0, packet->numUniqueVertices);

    for (unsigned short i = 0; i < packet->numUniqueVertices; ++i)
    {
        WGPIPE.us = i;
        WGPIPE.us = i;
        WGPIPE.us = i;
    }
}

void GXMaterialProgram_802A5D58::BindParameters(const glModelPacket* packet)
{
    fn_8036BE88(0, packet->unknown20);
}

const GXMaterialParameter* GXMaterialProgram_802A5D58::GetParameters()
{
    return Parameters;
}
