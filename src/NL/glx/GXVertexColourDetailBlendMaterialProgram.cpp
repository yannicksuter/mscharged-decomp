#include <revolution/gx.h>

#include "NL/glx/GXVertexColourDetailBlendMaterialProgram.h"
#include "NL/glx/glxTexture.h"
#include "NL/glx/GXMaterialProgramInternal.h"
#include "NL/gl/glLoadModel.h"
#include "Game/UnidentifiedStaticStorage.h"

GXVertexColourDetailBlendMaterialProgram* GXVertexColourDetailBlendMaterialProgram::Instance;
bool GXVertexColourDetailBlendMaterialProgram::Initialized;

GXMaterialParameter GXVertexColourDetailBlendMaterialProgram::Parameters[3] = {
    { 0x69F44DC5, 0x01010103, 0 },
    { 0xEBAF55D2, 0x01010103, 8 },
    { 0x0658BB38, 0x01010101, 16 },
};

GXVertexColourDetailBlendMaterialProgram::GXVertexColourDetailBlendMaterialProgram()
{
    Instance = this;
    programHash = 0x13DF86AD;
    parameterDataSize = 20;
    parameterCount = 3;
    glRegisterMaterialProgram(this, programHash);
}

GXVertexColourDetailBlendMaterialProgram::~GXVertexColourDetailBlendMaterialProgram()
{
}

void GXVertexColourDetailBlendMaterialProgram::Initialize()
{
    if (Initialized)
        return;
    Initialized = true;
}

void GXVertexColourDetailBlendMaterialProgram::Configure(glModelPacket*)
{
}

void GXVertexColourDetailBlendMaterialProgram::ConfigureVertexFormat(bool indexed)
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

void GXVertexColourDetailBlendMaterialProgram::BindVertexArrays(const glModelPacket* packet)
{
    glModelStream* streams = packet->streams;
    GXSetArray(GX_VA_POS, streams[0].address, 12);
    GXSetArray(GX_VA_CLR0, streams[3].address, 4);
    GXSetArray(GX_VA_TEX0, streams[1].address, 4);
    GXSetArray(GX_VA_TEX1, streams[2].address, 4);
}

void GXVertexColourDetailBlendMaterialProgram::DrawIndexed(
    const glModelPacket* packet)
{
    unsigned short* index = packet->indexBuffer;
    unsigned short* end = index + packet->numVertices;
    GXBegin(glxGetPrimitiveType((unsigned char)packet->primType), GX_VTXFMT0, (unsigned short)packet->numVertices);

    while (index < end)
    {
        WGPIPE.us = *index;
        WGPIPE.us = *index;
        WGPIPE.us = *index;
        WGPIPE.us = *index;
        ++index;
    }
}

void GXVertexColourDetailBlendMaterialProgram::DrawDirect(
    const glModelPacket* packet)
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

void GXVertexColourDetailBlendMaterialProgram::BindParameters(const glModelPacket* packet)
{
    glx_BindTexture(0, &static_cast<GXVertexColourDetailBlendParameters*>(packet->materialParameters)->diffuseTexture);
    glx_BindTexture(1, &static_cast<GXVertexColourDetailBlendParameters*>(packet->materialParameters)->detailTexture);
}

const GXMaterialParameter* GXVertexColourDetailBlendMaterialProgram::GetParameters()
{
    return Parameters;
}
