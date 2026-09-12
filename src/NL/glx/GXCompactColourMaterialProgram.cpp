#include <revolution/gx.h>

#include "NL/glx/GXCompactColourMaterialProgram.h"
#include "NL/glx/GXMaterialProgramInternal.h"
#include "NL/glx/glxTexture.h"
#include "NL/gl/glLoadModel.h"
#include "Game/UnidentifiedStaticStorage.h"

GXCompactColourMaterialProgram* GXCompactColourMaterialProgram::Instance;
bool GXCompactColourMaterialProgram::Initialized;

GXMaterialParameter GXCompactColourMaterialProgram::Parameters[1] = {
    { 0x69F44DC5, 0x01010103, 0 },
};

GXCompactColourMaterialProgram::GXCompactColourMaterialProgram()
{
    Instance = this;
    programHash = 0x4ED6C66F;
    parameterDataSize = 8;
    parameterCount = 1;
    glRegisterMaterialProgram(this, programHash);
}

GXCompactColourMaterialProgram::~GXCompactColourMaterialProgram()
{
}

void GXCompactColourMaterialProgram::Initialize()
{
    if (Initialized)
        return;
    Initialized = true;
}

void GXCompactColourMaterialProgram::Configure(glModelPacket*)
{
}

void GXCompactColourMaterialProgram::ConfigureVertexFormat(bool indexed)
{
    GXClearVtxDesc();
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_S16, 6);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_S16, 10);

    if (indexed)
    {
        GXSetVtxDesc(GX_VA_POS, GX_INDEX16);
        GXSetVtxDesc(GX_VA_CLR0, GX_INDEX16);
        GXSetVtxDesc(GX_VA_TEX0, GX_INDEX16);
    }
    else
    {
        GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
        GXSetVtxDesc(GX_VA_CLR0, GX_DIRECT);
        GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);
    }
}

void GXCompactColourMaterialProgram::BindVertexArrays(const glModelPacket* packet)
{
    glModelStream* streams = packet->streams;
    GXSetArray(GX_VA_POS, streams[0].address, 6);
    GXSetArray(GX_VA_CLR0, streams[2].address, 4);
    GXSetArray(GX_VA_TEX0, streams[1].address, 4);
}

void GXCompactColourMaterialProgram::DrawIndexed(const glModelPacket* packet)
{
    unsigned short* idxPtr = packet->indexBuffer;
    unsigned short* end = idxPtr + packet->numVertices;
    GXBegin(glxGetPrimitiveType((unsigned char)packet->primType), GX_VTXFMT0, (unsigned short)packet->numVertices);

    while (idxPtr < end)
    {
        WGPIPE.us = *idxPtr;
        WGPIPE.us = *idxPtr;
        WGPIPE.us = *idxPtr;
        ++idxPtr;
    }
}

void GXCompactColourMaterialProgram::DrawDirect(const glModelPacket* packet)
{
    GXBegin(glxGetPrimitiveType((unsigned char)packet->primType), GX_VTXFMT0, packet->numUniqueVertices);

    for (unsigned short i = 0; i < packet->numUniqueVertices; ++i)
    {
        WGPIPE.us = i;
        WGPIPE.us = i;
        WGPIPE.us = i;
    }
}

void GXCompactColourMaterialProgram::BindParameters(const glModelPacket* packet)
{
    glx_BindTexture(0, &static_cast<GXCompactColourParameters*>(packet->materialParameters)->texture);
}

const GXMaterialParameter* GXCompactColourMaterialProgram::GetParameters()
{
    return Parameters;
}
