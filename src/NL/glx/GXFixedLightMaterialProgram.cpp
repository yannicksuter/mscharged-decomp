#include <revolution/gx.h>

#include "NL/glx/GXFixedLightMaterialProgram.h"
#include "NL/glx/glxTexture.h"
#include "NL/glx/GXMaterialProgramInternal.h"
#include "NL/gl/glLoadModel.h"
#include "Game/UnidentifiedStaticStorage.h"

GXFixedLightMaterialProgram* GXFixedLightMaterialProgram::Instance;
bool GXFixedLightMaterialProgram::Initialized;

GXMaterialParameter GXFixedLightMaterialProgram::Parameters[4] = {
    { 0x69F44DC5, 0x01010103, 0 },
    { 0xFB2ECAB9, 0x01040181, 8 },
    { 0xDA88C73B, 0x01040181, 24 },
    { 0xEF0F57EB, 0x01040181, 40 },
};

GXFixedLightMaterialProgram::GXFixedLightMaterialProgram()
{
    Instance = this;
    programHash = 0xAD0DB7E9;
    parameterDataSize = 56;
    parameterCount = 4;
    glRegisterMaterialProgram(this, programHash);
}

GXFixedLightMaterialProgram::~GXFixedLightMaterialProgram()
{
}

void GXFixedLightMaterialProgram::Initialize()
{
    if (Initialized)
        return;
    Initialized = true;
}

void GXFixedLightMaterialProgram::Configure(glModelPacket*)
{
}

void GXFixedLightMaterialProgram::ConfigureVertexFormat(bool indexed)
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

void GXFixedLightMaterialProgram::BindVertexArrays(const glModelPacket* packet)
{
    glModelStream* streams = packet->streams;
    GXSetArray(GX_VA_POS, streams[0].address, 12);
    GXSetArray(GX_VA_NRM, streams[1].address, 12);
    GXSetArray(GX_VA_TEX0, streams[2].address, 8);
}

void GXFixedLightMaterialProgram::DrawIndexed(const glModelPacket* packet)
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

void GXFixedLightMaterialProgram::DrawDirect(const glModelPacket* packet)
{
    GXBegin(glxGetPrimitiveType((unsigned char)packet->primType), GX_VTXFMT0, packet->numUniqueVertices);

    for (unsigned short i = 0; i < packet->numUniqueVertices; ++i)
    {
        WGPIPE.us = i;
        WGPIPE.us = i;
        WGPIPE.us = i;
    }
}

void GXFixedLightMaterialProgram::BindParameters(const glModelPacket* packet)
{
    glx_BindTexture(0, &static_cast<GXFixedLightParameters*>(packet->materialParameters)->diffuseTexture);
}

const GXMaterialParameter* GXFixedLightMaterialProgram::GetParameters()
{
    return Parameters;
}
