#include <revolution/gx.h>

#include "NL/glx/GXMaterialProgram.h"

GXMaterialProgram_802A6848* GXMaterialProgram_802A6848::Instance;
bool GXMaterialProgram_802A6848::Initialized;

GXMaterialProgram_802A6848::GXMaterialProgram_802A6848()
{
    Instance = this;
    programHash = 0xDC56470F;
    parameterDataSize = 0;
    parameterCount = 0;
    fn_802CB790(this, programHash);
}

GXMaterialProgram_802A6848::~GXMaterialProgram_802A6848()
{
}

void GXMaterialProgram_802A6848::Initialize()
{
    if (Initialized)
        return;
    Initialized = true;
}

void GXMaterialProgram_802A6848::Configure()
{
}

void GXMaterialProgram_802A6848::ConfigureVertexFormat(bool indexed)
{
    GXClearVtxDesc();
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);

    if (indexed)
    {
        GXSetVtxDesc(GX_VA_POS, GX_INDEX16);
    }
    else
    {
        GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
    }
}

void GXMaterialProgram_802A6848::BindVertexArrays(const glModelPacket* packet)
{
    GXSetArray(GX_VA_POS, packet->streams[0].address, 12);
}

void GXMaterialProgram_802A6848::DrawIndexed(const glModelPacket* packet)
{
    unsigned short* idxPtr = packet->indexBuffer;
    unsigned short* end = idxPtr + packet->numVertices;
    GXBegin(lbl_80524470[(unsigned char)packet->primType], GX_VTXFMT0, (unsigned short)packet->numVertices);

    while (idxPtr < end)
    {
        WGPIPE.us = *idxPtr;
        ++idxPtr;
    }
}

void GXMaterialProgram_802A6848::DrawDirect(const glModelPacket* packet)
{
    GXBegin(lbl_80524470[(unsigned char)packet->primType], GX_VTXFMT0, packet->numUniqueVertices);

    for (unsigned short i = 0; i < packet->numUniqueVertices; ++i)
    {
        WGPIPE.us = i;
    }
}

void GXMaterialProgram_802A6848::BindParameters(const glModelPacket* packet)
{
}

const GXMaterialParameter* GXMaterialProgram_802A6848::GetParameters()
{
    return 0;
}
