#include <revolution/gx.h>

#include "NL/glx/GXMaterialProgram.h"

GXMaterialProgram_8029DE3C* GXMaterialProgram_8029DE3C::Instance;
bool GXMaterialProgram_8029DE3C::Initialized;

GXMaterialParameter GXMaterialProgram_8029DE3C::Parameters[8] = {
    { 0x69F44DC5, 0x01010103, 0 },
    { 0xDAEE3577, 0x01010101, 8 },
    { 0xDEF8C698, 0x01010101, 12 },
    { 0x26839970, 0x01010102, 16 },
    { 0xE516A611, 0x01010102, 20 },
    { 0x8E10B600, 0x01010102, 24 },
    { 0x5D55478A, 0x01010102, 28 },
    { 0x02D52538, 0x01010102, 32 },
};

GXMaterialProgram_8029DE3C::GXMaterialProgram_8029DE3C()
{
    Instance = this;
    programHash = 0x2169DB5C;
    parameterDataSize = 36;
    parameterCount = 8;
    fn_802CB790(this, programHash);
}

GXMaterialProgram_8029DE3C::~GXMaterialProgram_8029DE3C()
{
}

void GXMaterialProgram_8029DE3C::Initialize()
{
    if (Initialized)
        return;
    Initialized = true;
}

void GXMaterialProgram_8029DE3C::Configure()
{
}

void GXMaterialProgram_8029DE3C::ConfigureVertexFormat(bool indexed)
{
    GXClearVtxDesc();
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_NRM, GX_NRM_XYZ, GX_S8, 6);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_S16, 10);

    if (indexed)
    {
        GXSetVtxDesc(GX_VA_POS, GX_INDEX16);
        GXSetVtxDesc(GX_VA_NRM, GX_INDEX16);
        GXSetVtxDesc(GX_VA_CLR0, GX_INDEX16);
        GXSetVtxDesc(GX_VA_TEX0, GX_INDEX16);
    }
    else
    {
        GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
        GXSetVtxDesc(GX_VA_NRM, GX_DIRECT);
        GXSetVtxDesc(GX_VA_CLR0, GX_DIRECT);
        GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);
    }
}

void GXMaterialProgram_8029DE3C::BindVertexArrays(const glModelPacket* packet)
{
    glModelStream* streams = packet->streams;
    GXSetArray(GX_VA_POS, streams[0].address, 12);
    GXSetArray(GX_VA_NRM, streams[1].address, 3);
    GXSetArray(GX_VA_CLR0, streams[3].address, 4);
    GXSetArray(GX_VA_TEX0, streams[2].address, 4);
}

void GXMaterialProgram_8029DE3C::DrawIndexed(const glModelPacket* packet)
{
    unsigned short* idxPtr = packet->indexBuffer;
    unsigned short* end = idxPtr + packet->numVertices;
    GXBegin(lbl_80524470[(unsigned char)packet->primType], GX_VTXFMT0, (unsigned short)packet->numVertices);

    while (idxPtr < end)
    {
        WGPIPE.us = *idxPtr;
        WGPIPE.us = *idxPtr;
        WGPIPE.us = *idxPtr;
        WGPIPE.us = *idxPtr;
        ++idxPtr;
    }
}

void GXMaterialProgram_8029DE3C::DrawDirect(const glModelPacket* packet)
{
    GXBegin(lbl_80524470[(unsigned char)packet->primType], GX_VTXFMT0, packet->numUniqueVertices);

    for (unsigned short i = 0; i < packet->numUniqueVertices; ++i)
    {
        WGPIPE.us = i;
        WGPIPE.us = i;
        WGPIPE.us = i;
        WGPIPE.us = i;
    }
}

void GXMaterialProgram_8029DE3C::BindParameters(const glModelPacket* packet)
{
    fn_8036BE88(0, packet->unknown20);
}

const GXMaterialParameter* GXMaterialProgram_8029DE3C::GetParameters()
{
    return Parameters;
}
