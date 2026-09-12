#include <revolution/gx.h>

#include "NL/glx/GXFourTextureAddMaterialProgram.h"
#include "NL/glx/glxTexture.h"
#include "NL/glx/GXMaterialProgramInternal.h"
#include "Game/UnidentifiedStaticStorage.h"

GXFourTextureAddMaterialProgram* GXFourTextureAddMaterialProgram::Instance;
bool GXFourTextureAddMaterialProgram::Initialized;

GXMaterialParameter GXFourTextureAddMaterialProgram::Parameters[4] = {
    { 0x69F44DC5, 0x01010103, 0 }, // NLG_DIFFUSE
    { 0xEBAF55D2, 0x01010103, 8 }, // NLG_DETAIL
    { 0x0ED8A325, 0x01010103, 16 }, // NLG_SHADOW
    { 0x93014DE7, 0x01010103, 24 }, // NLG_GLOSS
};

GXFourTextureAddMaterialProgram::GXFourTextureAddMaterialProgram()
{
    Instance = this;
    programHash = 0x2910966C;
    parameterDataSize = 32;
    parameterCount = 4;
    glRegisterMaterialProgram(this, programHash);
}

GXFourTextureAddMaterialProgram::~GXFourTextureAddMaterialProgram()
{
}

void GXFourTextureAddMaterialProgram::Initialize()
{
    if (Initialized)
        return;
    Initialized = true;
}

void GXFourTextureAddMaterialProgram::Configure(glModelPacket*)
{
}

void GXFourTextureAddMaterialProgram::ConfigureVertexFormat(bool indexed)
{
    GXClearVtxDesc();
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX1, GX_TEX_ST, GX_F32, 0);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX2, GX_TEX_ST, GX_F32, 0);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX3, GX_TEX_ST, GX_F32, 0);

    if (indexed)
    {
        GXSetVtxDesc(GX_VA_POS, GX_INDEX16);
        GXSetVtxDesc(GX_VA_CLR0, GX_INDEX16);
        GXSetVtxDesc(GX_VA_TEX0, GX_INDEX16);
        GXSetVtxDesc(GX_VA_TEX1, GX_INDEX16);
        GXSetVtxDesc(GX_VA_TEX2, GX_INDEX16);
        GXSetVtxDesc(GX_VA_TEX3, GX_INDEX16);
    }
    else
    {
        GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
        GXSetVtxDesc(GX_VA_CLR0, GX_DIRECT);
        GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);
        GXSetVtxDesc(GX_VA_TEX1, GX_DIRECT);
        GXSetVtxDesc(GX_VA_TEX2, GX_DIRECT);
        GXSetVtxDesc(GX_VA_TEX3, GX_DIRECT);
    }
}

void GXFourTextureAddMaterialProgram::BindVertexArrays(const glModelPacket* packet)
{
    glModelStream* streams = packet->streams;
    GXSetArray(GX_VA_POS, streams[0].address, 12);
    GXSetArray(GX_VA_CLR0, streams[5].address, 4);
    GXSetArray(GX_VA_TEX0, streams[1].address, 8);
    GXSetArray(GX_VA_TEX1, streams[2].address, 8);
    GXSetArray(GX_VA_TEX2, streams[3].address, 8);
    GXSetArray(GX_VA_TEX3, streams[4].address, 8);
}

void GXFourTextureAddMaterialProgram::DrawIndexed(const glModelPacket* packet)
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
        WGPIPE.us = *idxPtr;
        WGPIPE.us = *idxPtr;
        ++idxPtr;
    }
}

void GXFourTextureAddMaterialProgram::DrawDirect(const glModelPacket* packet)
{
    GXBegin(glxGetPrimitiveType((unsigned char)packet->primType), GX_VTXFMT0, packet->numUniqueVertices);

    for (unsigned short i = 0; i < packet->numUniqueVertices; ++i)
    {
        WGPIPE.us = i;
        WGPIPE.us = i;
        WGPIPE.us = i;
        WGPIPE.us = i;
        WGPIPE.us = i;
        WGPIPE.us = i;
    }
}

void GXFourTextureAddMaterialProgram::BindParameters(const glModelPacket* packet)
{
    glx_BindTexture(0, &static_cast<GXFourTextureAddParameters*>(packet->materialParameters)->diffuseTexture);
    glx_BindTexture(1, &static_cast<GXFourTextureAddParameters*>(packet->materialParameters)->detailTexture);
    glx_BindTexture(2, &static_cast<GXFourTextureAddParameters*>(packet->materialParameters)->shadowTexture);
    glx_BindTexture(3, &static_cast<GXFourTextureAddParameters*>(packet->materialParameters)->glossTexture);
}

const GXMaterialParameter* GXFourTextureAddMaterialProgram::GetParameters()
{
    return Parameters;
}
