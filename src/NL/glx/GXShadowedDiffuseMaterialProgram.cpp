#include <revolution/gx.h>

#include "NL/glx/GXShadowedDiffuseMaterialProgram.h"
#include "NL/glx/glxTexture.h"
#include "NL/glx/GXMaterialProgramInternal.h"
#include "Game/UnidentifiedStaticStorage.h"

GXShadowedDiffuseMaterialProgram* GXShadowedDiffuseMaterialProgram::Instance;
bool GXShadowedDiffuseMaterialProgram::Initialized;

GXMaterialParameter GXShadowedDiffuseMaterialProgram::Parameters[2] = {
    { 0x69F44DC5, 0x01010103, 0 }, // NLG_DIFFUSE
    { 0xEADBCE0A, 0x01010102, 8 }, // receiveShadows
};

GXShadowedDiffuseMaterialProgram::GXShadowedDiffuseMaterialProgram()
{
    Instance = this;
    programHash = 0x257C2FF3;
    parameterDataSize = 12;
    parameterCount = 2;
    glRegisterMaterialProgram(this, programHash);
}

GXShadowedDiffuseMaterialProgram::~GXShadowedDiffuseMaterialProgram()
{
}

void GXShadowedDiffuseMaterialProgram::Initialize()
{
    if (Initialized)
        return;
    Initialized = true;
}

void GXShadowedDiffuseMaterialProgram::Configure(glModelPacket*)
{
}

void GXShadowedDiffuseMaterialProgram::ConfigureVertexFormat(bool indexed)
{
    GXClearVtxDesc();
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
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

void GXShadowedDiffuseMaterialProgram::BindVertexArrays(const glModelPacket* packet)
{
    glModelStream* streams = packet->streams;
    GXSetArray(GX_VA_POS, streams[0].address, 12);
    GXSetArray(GX_VA_CLR0, streams[2].address, 4);
    GXSetArray(GX_VA_TEX0, streams[1].address, 4);
}

void GXShadowedDiffuseMaterialProgram::DrawIndexed(const glModelPacket* packet)
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

void GXShadowedDiffuseMaterialProgram::DrawDirect(const glModelPacket* packet)
{
    GXBegin(glxGetPrimitiveType((unsigned char)packet->primType), GX_VTXFMT0, packet->numUniqueVertices);

    for (unsigned short i = 0; i < packet->numUniqueVertices; ++i)
    {
        WGPIPE.us = i;
        WGPIPE.us = i;
        WGPIPE.us = i;
    }
}

void GXShadowedDiffuseMaterialProgram::BindParameters(const glModelPacket* packet)
{
    glx_BindTexture(0, &static_cast<GXShadowedDiffuseParameters*>(packet->materialParameters)->diffuseTexture);
}

const GXMaterialParameter* GXShadowedDiffuseMaterialProgram::GetParameters()
{
    return Parameters;
}
