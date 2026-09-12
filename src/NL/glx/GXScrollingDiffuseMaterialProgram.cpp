#include <revolution/gx.h>

#include "NL/glx/GXScrollingDiffuseMaterialProgram.h"
#include "NL/glx/glxTexture.h"
#include "NL/glx/GXMaterialProgramInternal.h"
#include "NL/gl/glLoadModel.h"
#include "Game/UnidentifiedStaticStorage.h"

GXScrollingDiffuseMaterialProgram* GXScrollingDiffuseMaterialProgram::Instance;
bool GXScrollingDiffuseMaterialProgram::Initialized;

GXMaterialParameter GXScrollingDiffuseMaterialProgram::Parameters[8] = {
    { 0x69F44DC5, 0x01010103, 0 },
    { 0xDAEE3577, 0x01010101, 8 },
    { 0xDEF8C698, 0x01010101, 12 },
    { 0x26839970, 0x01010102, 16 },
    { 0xE516A611, 0x01010102, 20 },
    { 0x8E10B600, 0x01010102, 24 },
    { 0x5D55478A, 0x01010102, 28 },
    { 0x02D52538, 0x01010102, 32 },
};

GXScrollingDiffuseMaterialProgram::GXScrollingDiffuseMaterialProgram()
{
    Instance = this;
    programHash = 0x2169DB5C;
    parameterDataSize = 36;
    parameterCount = 8;
    glRegisterMaterialProgram(this, programHash);
}

GXScrollingDiffuseMaterialProgram::~GXScrollingDiffuseMaterialProgram()
{
}

void GXScrollingDiffuseMaterialProgram::Initialize()
{
    if (Initialized)
        return;
    Initialized = true;
}

void GXScrollingDiffuseMaterialProgram::Configure(glModelPacket*)
{
}

void GXScrollingDiffuseMaterialProgram::ConfigureVertexFormat(bool indexed)
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

void GXScrollingDiffuseMaterialProgram::BindVertexArrays(const glModelPacket* packet)
{
    glModelStream* streams = packet->streams;
    GXSetArray(GX_VA_POS, streams[0].address, 12);
    GXSetArray(GX_VA_NRM, streams[1].address, 3);
    GXSetArray(GX_VA_CLR0, streams[3].address, 4);
    GXSetArray(GX_VA_TEX0, streams[2].address, 4);
}

void GXScrollingDiffuseMaterialProgram::DrawIndexed(const glModelPacket* packet)
{
    unsigned short* idxPtr = packet->indexBuffer;
    unsigned short* end = idxPtr + packet->numVertices;
    GXBegin(glxGetPrimitiveType((unsigned char)packet->primType), GX_VTXFMT0, (unsigned short)packet->numVertices);

    while (idxPtr < end)
    {
        WGPIPE.us = *idxPtr;
        WGPIPE.us = *idxPtr;
        WGPIPE.us = *idxPtr;
        WGPIPE.us = *idxPtr;
        ++idxPtr;
    }
}

void GXScrollingDiffuseMaterialProgram::DrawDirect(const glModelPacket* packet)
{
    GXBegin(glxGetPrimitiveType((unsigned char)packet->primType), GX_VTXFMT0, packet->numUniqueVertices);

    for (unsigned short i = 0; i < packet->numUniqueVertices; ++i)
    {
        WGPIPE.us = i;
        WGPIPE.us = i;
        WGPIPE.us = i;
        WGPIPE.us = i;
    }
}

void GXScrollingDiffuseMaterialProgram::BindParameters(const glModelPacket* packet)
{
    glx_BindTexture(0, &static_cast<GXScrollingDiffuseParameters*>(packet->materialParameters)->diffuseTexture);
}

const GXMaterialParameter* GXScrollingDiffuseMaterialProgram::GetParameters()
{
    return Parameters;
}
