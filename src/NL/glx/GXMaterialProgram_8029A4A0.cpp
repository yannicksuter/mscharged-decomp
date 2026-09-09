#include <revolution/gx.h>

#include "NL/glx/GXMaterialProgram.h"
#include "NL/gl/glLoadModel.h"

GXMaterialProgram_8029A4A0* GXMaterialProgram_8029A4A0::Instance;
bool GXMaterialProgram_8029A4A0::Initialized;

GXMaterialParameter GXMaterialProgram_8029A4A0::Parameters[4] = {
    { 0x69F44DC5, 0x01010103, 0 },
    { 0xEBAF55D2, 0x01010103, 8 },
    { 0x0ED8A325, 0x01010103, 16 },
    { 0x93014DE7, 0x01010103, 24 },
};

GXMaterialProgram_8029A4A0::GXMaterialProgram_8029A4A0()
{
    Instance = this;
    programHash = 0x2910966C;
    parameterDataSize = 32;
    parameterCount = 4;
    glRegisterMaterialProgram(this, programHash);
}

GXMaterialProgram_8029A4A0::~GXMaterialProgram_8029A4A0()
{
}

void GXMaterialProgram_8029A4A0::Initialize()
{
    if (Initialized)
        return;
    Initialized = true;
}

void GXMaterialProgram_8029A4A0::Configure(glModelPacket*)
{
}

void GXMaterialProgram_8029A4A0::ConfigureVertexFormat(bool indexed)
{
    GXClearVtxDesc();
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX1, GX_TEX_ST, GX_F32, 0);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX2, GX_TEX_ST, GX_F32, 0);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX3, GX_TEX_ST, GX_F32, 0);

    if (indexed)
    {
        GXSetVtxDesc(GX_VA_POS, GX_INDEX16);
        GXSetVtxDesc(GX_VA_CLR0, GX_INDEX16);
        GXSetVtxDesc(GX_VA_TEX0, GX_INDEX16);
        GXSetVtxDesc(GX_VA_TEX1, GX_INDEX16);
        GXSetVtxDesc(GX_VA_TEX2, GX_INDEX16);
        GXSetVtxDesc(GX_VA_TEX3, GX_INDEX16);
    }
    else
    {
        GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
        GXSetVtxDesc(GX_VA_CLR0, GX_DIRECT);
        GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);
        GXSetVtxDesc(GX_VA_TEX1, GX_DIRECT);
        GXSetVtxDesc(GX_VA_TEX2, GX_DIRECT);
        GXSetVtxDesc(GX_VA_TEX3, GX_DIRECT);
    }
}

void GXMaterialProgram_8029A4A0::BindVertexArrays(const glModelPacket* packet)
{
    glModelStream* streams = packet->streams;
    GXSetArray(GX_VA_POS, streams[0].address, 12);
    GXSetArray(GX_VA_CLR0, streams[5].address, 4);
    GXSetArray(GX_VA_TEX0, streams[1].address, 8);
    GXSetArray(GX_VA_TEX1, streams[2].address, 8);
    GXSetArray(GX_VA_TEX2, streams[3].address, 8);
    GXSetArray(GX_VA_TEX3, streams[4].address, 8);
}

void GXMaterialProgram_8029A4A0::DrawIndexed(const glModelPacket* packet)
{
    unsigned short* idxPtr = packet->indexBuffer;
    unsigned short* end = idxPtr + packet->numVertices;
    GXBegin(glx_PrimitiveTypes[(unsigned char)packet->primType], GX_VTXFMT0, (unsigned short)packet->numVertices);

    while (idxPtr < end)
    {
        WGPIPE.us = *idxPtr;
        WGPIPE.us = *idxPtr;
        WGPIPE.us = *idxPtr;
        WGPIPE.us = *idxPtr;
        WGPIPE.us = *idxPtr;
        WGPIPE.us = *idxPtr;
        ++idxPtr;
    }
}

void GXMaterialProgram_8029A4A0::DrawDirect(const glModelPacket* packet)
{
    GXBegin(glx_PrimitiveTypes[(unsigned char)packet->primType], GX_VTXFMT0, packet->numUniqueVertices);

    for (unsigned short i = 0; i < packet->numUniqueVertices; ++i)
    {
        WGPIPE.us = i;
        WGPIPE.us = i;
        WGPIPE.us = i;
        WGPIPE.us = i;
        WGPIPE.us = i;
        WGPIPE.us = i;
    }
}

void GXMaterialProgram_8029A4A0::BindParameters(const glModelPacket* packet)
{
    glx_BindTexture(0, (glTextureBinding*)(packet->unknown20));
    glx_BindTexture(1, (glTextureBinding*)((unsigned char*)packet->unknown20 + 8));
    glx_BindTexture(2, (glTextureBinding*)((unsigned char*)packet->unknown20 + 16));
    glx_BindTexture(3, (glTextureBinding*)((unsigned char*)packet->unknown20 + 24));
}

const GXMaterialParameter* GXMaterialProgram_8029A4A0::GetParameters()
{
    return Parameters;
}
