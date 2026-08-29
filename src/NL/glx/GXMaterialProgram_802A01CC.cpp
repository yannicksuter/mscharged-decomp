#include <revolution/gx.h>

#include "NL/glx/GXMaterialProgram.h"

GXMaterialProgram_802A01CC* GXMaterialProgram_802A01CC::Instance;
bool GXMaterialProgram_802A01CC::Initialized;

GXMaterialParameter GXMaterialProgram_802A01CC::Parameters[2] = {
    { 0x69F44DC5, 0x01010103, 0 },
    { 0xEE9D919D, 0x01040101, 8 },
};

GXMaterialProgram_802A01CC::GXMaterialProgram_802A01CC()
{
    Instance = this;
    programHash = 0xEC35CAAB;
    parameterDataSize = 24;
    parameterCount = 2;
    fn_802CB790(this, programHash);
}

GXMaterialProgram_802A01CC::~GXMaterialProgram_802A01CC()
{
}

void GXMaterialProgram_802A01CC::Initialize()
{
    if (Initialized)
        return;
    Initialized = true;
}

void GXMaterialProgram_802A01CC::Configure()
{
}

void GXMaterialProgram_802A01CC::ConfigureVertexFormat(bool indexed)
{
    GXClearVtxDesc();
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);

    if (indexed)
    {
        GXSetVtxDesc(GX_VA_POS, GX_INDEX16);
        GXSetVtxDesc(GX_VA_TEX0, GX_INDEX16);
    }
    else
    {
        GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
        GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);
    }
}

void GXMaterialProgram_802A01CC::BindVertexArrays(const glModelPacket* packet)
{
    glModelStream* streams = packet->streams;
    GXSetArray(GX_VA_POS, streams[0].address, 12);
    GXSetArray(GX_VA_TEX0, streams[1].address, 8);
}

void GXMaterialProgram_802A01CC::DrawIndexed(const glModelPacket* packet)
{
    unsigned short* idxPtr = packet->indexBuffer;
    unsigned short* end = idxPtr + packet->numVertices;
    GXBegin(lbl_80524470[(unsigned char)packet->primType], GX_VTXFMT0, (unsigned short)packet->numVertices);

    while (idxPtr < end)
    {
        WGPIPE.us = *idxPtr;
        WGPIPE.us = *idxPtr;
        ++idxPtr;
    }
}

void GXMaterialProgram_802A01CC::DrawDirect(const glModelPacket* packet)
{
    GXBegin(lbl_80524470[(unsigned char)packet->primType], GX_VTXFMT0, packet->numUniqueVertices);

    for (unsigned short i = 0; i < packet->numUniqueVertices; ++i)
    {
        WGPIPE.us = i;
        WGPIPE.us = i;
    }
}

const GXMaterialParameter* GXMaterialProgram_802A01CC::GetParameters()
{
    return Parameters;
}
