#include <revolution/gx.h>

#include "NL/glx/GXMaterialProgram.h"

GXMaterialProgram_802A58E8* GXMaterialProgram_802A58E8::Instance;
bool GXMaterialProgram_802A58E8::Initialized;

GXMaterialParameter GXMaterialProgram_802A58E8::Parameters[4] = {
    { 0x69F44DC5, 0x01010103, 0 },
    { 0xFB2ECAB9, 0x01040181, 8 },
    { 0xDA88C73B, 0x01040181, 24 },
    { 0xEF0F57EB, 0x01040181, 40 },
};

GXMaterialProgram_802A58E8::GXMaterialProgram_802A58E8()
{
    Instance = this;
    programHash = 0xAD0DB7E9;
    parameterDataSize = 56;
    parameterCount = 4;
    fn_802CB790(this, programHash);
}

GXMaterialProgram_802A58E8::~GXMaterialProgram_802A58E8()
{
}

void GXMaterialProgram_802A58E8::Initialize()
{
    if (Initialized)
        return;
    Initialized = true;
}

void GXMaterialProgram_802A58E8::Configure()
{
}

void GXMaterialProgram_802A58E8::ConfigureVertexFormat(bool indexed)
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

void GXMaterialProgram_802A58E8::BindVertexArrays(const glModelPacket* packet)
{
    glModelStream* streams = packet->streams;
    GXSetArray(GX_VA_POS, streams[0].address, 12);
    GXSetArray(GX_VA_NRM, streams[1].address, 12);
    GXSetArray(GX_VA_TEX0, streams[2].address, 8);
}

void GXMaterialProgram_802A58E8::DrawIndexed(const glModelPacket* packet)
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

void GXMaterialProgram_802A58E8::DrawDirect(const glModelPacket* packet)
{
    GXBegin(lbl_80524470[(unsigned char)packet->primType], GX_VTXFMT0, packet->numUniqueVertices);

    for (unsigned short i = 0; i < packet->numUniqueVertices; ++i)
    {
        WGPIPE.us = i;
        WGPIPE.us = i;
        WGPIPE.us = i;
    }
}

void GXMaterialProgram_802A58E8::BindParameters(const glModelPacket* packet)
{
    fn_8036BE88(0, packet->unknown20);
}

const GXMaterialParameter* GXMaterialProgram_802A58E8::GetParameters()
{
    return Parameters;
}
