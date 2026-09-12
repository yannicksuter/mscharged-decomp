#include <revolution/gx.h>

#include "NL/glx/GXThreeLightDiffuseMaterialProgram.h"
#include "NL/glx/glxTexture.h"
#include "NL/glx/GXMaterialProgramInternal.h"
#include "NL/gl/glLoadModel.h"
#include "Game/UnidentifiedStaticStorage.h"

GXThreeLightDiffuseMaterialProgram* GXThreeLightDiffuseMaterialProgram::Instance;
bool GXThreeLightDiffuseMaterialProgram::Initialized;

GXMaterialParameter GXThreeLightDiffuseMaterialProgram::Parameters[8] = {
    { 0x69F44DC5, 0x01010103, 0 },
    { 0xFB281C86, 0x01030181, 8 },
    { 0xFB281C87, 0x01030181, 20 },
    { 0xFB281C88, 0x01030181, 32 },
    { 0xDA88C73B, 0x01040181, 44 },
    { 0xDA88C73C, 0x01040181, 60 },
    { 0xDA88C73D, 0x01040181, 76 },
    { 0xEF0F57EB, 0x01040181, 92 },
};

GXThreeLightDiffuseMaterialProgram::GXThreeLightDiffuseMaterialProgram()
{
    Instance = this;
    programHash = 0x8D359080;
    parameterDataSize = 108;
    parameterCount = 8;
    glRegisterMaterialProgram(this, programHash);
}

GXThreeLightDiffuseMaterialProgram::~GXThreeLightDiffuseMaterialProgram()
{
}

void GXThreeLightDiffuseMaterialProgram::Initialize()
{
    if (Initialized)
        return;
    Initialized = true;
}

void GXThreeLightDiffuseMaterialProgram::Configure(glModelPacket*)
{
}

void GXThreeLightDiffuseMaterialProgram::ConfigureVertexFormat(bool indexed)
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

void GXThreeLightDiffuseMaterialProgram::BindVertexArrays(const glModelPacket* packet)
{
    glModelStream* streams = packet->streams;
    GXSetArray(GX_VA_POS, streams[0].address, 12);
    GXSetArray(GX_VA_NRM, streams[1].address, 12);
    GXSetArray(GX_VA_TEX0, streams[2].address, 8);
}

void GXThreeLightDiffuseMaterialProgram::DrawIndexed(const glModelPacket* packet)
{
    unsigned short* index = packet->indexBuffer;
    unsigned short* end = index + packet->numVertices;
    GXBegin(glxGetPrimitiveType((unsigned char)packet->primType), GX_VTXFMT0, (unsigned short)packet->numVertices);

    while (index < end)
    {
        WGPIPE.us = *index;
        WGPIPE.us = *index;
        WGPIPE.us = *index;
        ++index;
    }
}

void GXThreeLightDiffuseMaterialProgram::DrawDirect(const glModelPacket* packet)
{
    GXBegin(glxGetPrimitiveType((unsigned char)packet->primType), GX_VTXFMT0, packet->numUniqueVertices);

    for (unsigned short i = 0; i < packet->numUniqueVertices; ++i)
    {
        WGPIPE.us = i;
        WGPIPE.us = i;
        WGPIPE.us = i;
    }
}

void GXThreeLightDiffuseMaterialProgram::BindParameters(const glModelPacket* packet)
{
    glx_BindTexture(0, &static_cast<GXThreeLightDiffuseParameters*>(packet->materialParameters)->diffuseTexture);
}

const GXMaterialParameter* GXThreeLightDiffuseMaterialProgram::GetParameters()
{
    return Parameters;
}
