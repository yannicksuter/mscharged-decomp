#include <revolution/gx.h>

#include "NL/glx/GXMaterialProgram.h"
#include "NL/glx/GXMaterialProgramInternal.h"
#include "NL/gl/glLoadModel.h"
#include "Game/UnidentifiedStaticStorage.h"

GXMaterialProgram_802A53B4* GXMaterialProgram_802A53B4::Instance;
bool GXMaterialProgram_802A53B4::Initialized;

GXMaterialParameter GXMaterialProgram_802A53B4::Parameters[3] = {
    { 0x69F44DC5, 0x01010103, 0 },
    { 0xEBAF55D2, 0x01010103, 8 },
    { 0x0658BB38, 0x01010101, 16 },
};

GXMaterialProgram_802A53B4::GXMaterialProgram_802A53B4()
{
    Instance = this;
    programHash = 0x13DF86AD;
    parameterDataSize = 20;
    parameterCount = 3;
    glRegisterMaterialProgram(this, programHash);
}

GXMaterialProgram_802A53B4::~GXMaterialProgram_802A53B4()
{
}

void GXMaterialProgram_802A53B4::Initialize()
{
    if (Initialized)
        return;
    Initialized = true;
}

void GXMaterialProgram_802A53B4::Configure(glModelPacket*)
{
}

void GXMaterialProgram_802A53B4::ConfigureVertexFormat(bool indexed)
{
    GXClearVtxDesc();
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_S16, 8);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX1, GX_TEX_ST, GX_S16, 8);

    if (indexed)
    {
        GXSetVtxDesc(GX_VA_POS, GX_INDEX16);
        GXSetVtxDesc(GX_VA_CLR0, GX_INDEX16);
        GXSetVtxDesc(GX_VA_TEX0, GX_INDEX16);
        GXSetVtxDesc(GX_VA_TEX1, GX_INDEX16);
    }
    else
    {
        GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
        GXSetVtxDesc(GX_VA_CLR0, GX_DIRECT);
        GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);
        GXSetVtxDesc(GX_VA_TEX1, GX_DIRECT);
    }
}

void GXMaterialProgram_802A53B4::BindVertexArrays(const glModelPacket* packet)
{
    glModelStream* streams = packet->streams;
    GXSetArray(GX_VA_POS, streams[0].address, 12);
    GXSetArray(GX_VA_CLR0, streams[3].address, 4);
    GXSetArray(GX_VA_TEX0, streams[1].address, 4);
    GXSetArray(GX_VA_TEX1, streams[2].address, 4);
}

void GXMaterialProgram_802A53B4::DrawIndexed(
    const glModelPacket* packet, unsigned char)
{
    unsigned short* idxPtr = packet->indexBuffer;
    unsigned short* end = idxPtr + packet->numVertices;
    GXBegin(UnidentifiedGetPrimitiveType((unsigned char)packet->primType), GX_VTXFMT0, (unsigned short)packet->numVertices);

    while (idxPtr < end)
    {
        WGPIPE.us = *idxPtr;
        WGPIPE.us = *idxPtr;
        WGPIPE.us = *idxPtr;
        WGPIPE.us = *idxPtr;
        ++idxPtr;
    }
}

void GXMaterialProgram_802A53B4::DrawDirect(
    const glModelPacket* packet, unsigned char)
{
    GXBegin(UnidentifiedGetPrimitiveType((unsigned char)packet->primType), GX_VTXFMT0, packet->numUniqueVertices);

    for (unsigned short i = 0; i < packet->numUniqueVertices; ++i)
    {
        WGPIPE.us = i;
        WGPIPE.us = i;
        WGPIPE.us = i;
        WGPIPE.us = i;
    }
}

void GXMaterialProgram_802A53B4::BindParameters(const glModelPacket* packet)
{
    glx_BindTexture(0, (glTextureBinding*)(packet->materialParameters));
    glx_BindTexture(1, (glTextureBinding*)((unsigned char*)packet->materialParameters + 8));
}

const GXMaterialParameter* GXMaterialProgram_802A53B4::GetParameters()
{
    return Parameters;
}
