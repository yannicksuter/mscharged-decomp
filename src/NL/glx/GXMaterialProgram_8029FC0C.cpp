#include <revolution/gx.h>

#include "NL/glx/GXMaterialProgram.h"

GXMaterialProgram_8029FC0C* GXMaterialProgram_8029FC0C::Instance;
bool GXMaterialProgram_8029FC0C::Initialized;

GXMaterialParameter GXMaterialProgram_8029FC0C::Parameters[10] = {
    { 0x69F44DC5, 0x01010103, 0 },
    { 0x93014DE7, 0x01010103, 8 },
    { 0x0658BB38, 0x01010101, 16 },
    { 0xCCBCF02F, 0x01010101, 20 },
    { 0x4FBDBBF2, 0x01040101, 24 },
    { 0xDAEE3577, 0x01010101, 40 },
    { 0xDEF8C698, 0x01010101, 44 },
    { 0x67570BFE, 0x01010102, 48 },
    { 0x8E10B600, 0x01010102, 52 },
    { 0x26839970, 0x01010102, 56 },
};

GXMaterialProgram_8029FC0C::GXMaterialProgram_8029FC0C()
{
    Instance = this;
    programHash = 0x3ECCD955;
    parameterDataSize = 60;
    parameterCount = 10;
    fn_802CB790(this, programHash);
}

GXMaterialProgram_8029FC0C::~GXMaterialProgram_8029FC0C()
{
}

void GXMaterialProgram_8029FC0C::Initialize()
{
    if (Initialized)
        return;
    Initialized = true;
}

void GXMaterialProgram_8029FC0C::Configure()
{
}

void GXMaterialProgram_8029FC0C::ConfigureVertexFormat(bool indexed)
{
    GXClearVtxDesc();
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_NRM, GX_NRM_XYZ, GX_S8, 6);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_S16, 10);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX1, GX_TEX_ST, GX_S16, 10);

    if (indexed)
    {
        GXSetVtxDesc(GX_VA_POS, GX_INDEX16);
        GXSetVtxDesc(GX_VA_NRM, GX_INDEX16);
        GXSetVtxDesc(GX_VA_CLR0, GX_INDEX16);
        GXSetVtxDesc(GX_VA_TEX0, GX_INDEX16);
        GXSetVtxDesc(GX_VA_TEX1, GX_INDEX16);
    }
    else
    {
        GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
        GXSetVtxDesc(GX_VA_NRM, GX_DIRECT);
        GXSetVtxDesc(GX_VA_CLR0, GX_DIRECT);
        GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);
        GXSetVtxDesc(GX_VA_TEX1, GX_DIRECT);
    }
}

void GXMaterialProgram_8029FC0C::BindVertexArrays(const glModelPacket* packet)
{
    glModelStream* streams = packet->streams;
    GXSetArray(GX_VA_POS, streams[0].address, 12);
    GXSetArray(GX_VA_NRM, streams[1].address, 3);
    GXSetArray(GX_VA_CLR0, streams[4].address, 4);
    GXSetArray(GX_VA_TEX0, streams[2].address, 4);
    GXSetArray(GX_VA_TEX1, streams[3].address, 4);
}

void GXMaterialProgram_8029FC0C::DrawIndexed(const glModelPacket* packet)
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
        WGPIPE.us = *idxPtr;
        ++idxPtr;
    }
}

void GXMaterialProgram_8029FC0C::DrawDirect(const glModelPacket* packet)
{
    GXBegin(lbl_80524470[(unsigned char)packet->primType], GX_VTXFMT0, packet->numUniqueVertices);

    for (unsigned short i = 0; i < packet->numUniqueVertices; ++i)
    {
        WGPIPE.us = i;
        WGPIPE.us = i;
        WGPIPE.us = i;
        WGPIPE.us = i;
        WGPIPE.us = i;
    }
}

void GXMaterialProgram_8029FC0C::BindParameters(const glModelPacket* packet)
{
    fn_8036BE88(0, packet->unknown20);
    fn_8036BE88(1, (unsigned char*)packet->unknown20 + 8);
}

const GXMaterialParameter* GXMaterialProgram_8029FC0C::GetParameters()
{
    return Parameters;
}
