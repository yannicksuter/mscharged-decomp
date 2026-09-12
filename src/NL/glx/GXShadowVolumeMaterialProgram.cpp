#include <revolution/gx.h>

#include "NL/glx/GXShadowVolumeMaterialProgram.h"
#include "NL/glx/glxTexture.h"
#include "NL/glx/GXMaterialProgramInternal.h"
#include "NL/gl/glLoadModel.h"
#include "Game/UnidentifiedStaticStorage.h"

GXShadowVolumeMaterialProgram* GXShadowVolumeMaterialProgram::Instance;
bool GXShadowVolumeMaterialProgram::Initialized;

GXMaterialParameter GXShadowVolumeMaterialProgram::Parameters[2] = {
    { 0x69F44DC5, 0x01010103, 0 }, // NLG_DIFFUSE
    { 0x4035762A, 0x01010102, 8 },
};

GXShadowVolumeMaterialProgram::GXShadowVolumeMaterialProgram()
{
    Instance = this;
    programHash = 0x386ECBDD;
    parameterDataSize = 12;
    parameterCount = 2;
    glRegisterMaterialProgram(this, programHash);
}

GXShadowVolumeMaterialProgram::~GXShadowVolumeMaterialProgram()
{
}

void GXShadowVolumeMaterialProgram::Initialize()
{
    if (Initialized)
        return;
    Initialized = true;
}

void GXShadowVolumeMaterialProgram::Configure(glModelPacket*)
{
}

void GXShadowVolumeMaterialProgram::ConfigureVertexFormat(bool indexed)
{
    GXClearVtxDesc();
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);

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

void GXShadowVolumeMaterialProgram::BindVertexArrays(const glModelPacket* packet)
{
    glModelStream* streams = packet->streams;
    GXSetArray(GX_VA_POS, streams[0].address, 12);
    GXSetArray(GX_VA_CLR0, streams[1].address, 4);
    GXSetArray(GX_VA_TEX0, streams[2].address, 8);
}

void GXShadowVolumeMaterialProgram::DrawIndexed(const glModelPacket* packet)
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

void GXShadowVolumeMaterialProgram::DrawDirect(const glModelPacket* packet)
{
    GXBegin(glxGetPrimitiveType((unsigned char)packet->primType), GX_VTXFMT0, packet->numUniqueVertices);

    for (unsigned short i = 0; i < packet->numUniqueVertices; ++i)
    {
        WGPIPE.us = i;
        WGPIPE.us = i;
        WGPIPE.us = i;
    }
}

void GXShadowVolumeMaterialProgram::BindParameters(const glModelPacket* packet)
{
    glx_BindTexture(0, &static_cast<GXShadowVolumeParameters*>(packet->materialParameters)->diffuseTexture);
}

const GXMaterialParameter* GXShadowVolumeMaterialProgram::GetParameters()
{
    return Parameters;
}
