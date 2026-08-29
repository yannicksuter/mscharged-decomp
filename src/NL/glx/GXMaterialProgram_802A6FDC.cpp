#include <revolution/gx.h>

#include "NL/glx/GXMaterialProgram.h"

GXMaterialProgram_802A6FDC* GXMaterialProgram_802A6FDC::Instance;
bool GXMaterialProgram_802A6FDC::Initialized;

GXMaterialProgram_802A6FDC::GXMaterialProgram_802A6FDC()
{
    Instance = this;
    programHash = 0xD701656B;
    parameterDataSize = 0;
    parameterCount = 0;
    fn_802CB790(this, programHash);
}

GXMaterialProgram_802A6FDC::~GXMaterialProgram_802A6FDC()
{
}

void GXMaterialProgram_802A6FDC::Initialize()
{
    if (Initialized)
        return;
    Initialized = true;
}

void GXMaterialProgram_802A6FDC::Configure()
{
}

void GXMaterialProgram_802A6FDC::ConfigureVertexFormat(bool indexed)
{
    GXClearVtxDesc();
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);

    if (indexed)
    {
        GXSetVtxDesc(GX_VA_POS, GX_INDEX16);
        GXSetVtxDesc(GX_VA_CLR0, GX_INDEX16);
    }
    else
    {
        GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
        GXSetVtxDesc(GX_VA_CLR0, GX_DIRECT);
    }
}

void GXMaterialProgram_802A6FDC::BindVertexArrays(const glModelPacket* packet)
{
    glModelStream* streams = packet->streams;
    GXSetArray(GX_VA_POS, streams[0].address, 12);
    GXSetArray(GX_VA_CLR0, streams[1].address, 4);
}

void GXMaterialProgram_802A6FDC::DrawIndexed(const glModelPacket* packet)
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

void GXMaterialProgram_802A6FDC::DrawDirect(const glModelPacket* packet)
{
    GXBegin(lbl_80524470[(unsigned char)packet->primType], GX_VTXFMT0, packet->numUniqueVertices);

    for (unsigned short i = 0; i < packet->numUniqueVertices; ++i)
    {
        WGPIPE.us = i;
        WGPIPE.us = i;
    }
}

void GXMaterialProgram_802A6FDC::BindParameters(const glModelPacket* packet)
{
}

const GXMaterialParameter* GXMaterialProgram_802A6FDC::GetParameters()
{
    return 0;
}
