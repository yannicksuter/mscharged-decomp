#include <revolution/gx.h>

#include "NL/glx/GXTextureBlendMaterialProgram.h"
#include "NL/glx/glxTexture.h"
#include "NL/glx/GXMaterialProgramInternal.h"
#include "NL/gl/glLoadModel.h"
#include "Game/UnidentifiedStaticStorage.h"

GXTextureBlendMaterialProgram* GXTextureBlendMaterialProgram::Instance;
bool GXTextureBlendMaterialProgram::Initialized;

GXMaterialParameter GXTextureBlendMaterialProgram::Parameters[3] = {
    { 0x69F44DC5, 0x01010103, 0 },
    { 0xEBAF55D2, 0x01010103, 8 },
    { 0x0658BB38, 0x01010101, 16 },
};

GXTextureBlendMaterialProgram::GXTextureBlendMaterialProgram()
{
    Instance = this;
    programHash = 0x46ABE398;
    parameterDataSize = 20;
    parameterCount = 3;
    glRegisterMaterialProgram(this, programHash);
}

GXTextureBlendMaterialProgram::~GXTextureBlendMaterialProgram()
{
}

void GXTextureBlendMaterialProgram::Initialize()
{
    if (Initialized)
        return;
    Initialized = true;
}

void GXTextureBlendMaterialProgram::Configure(glModelPacket*)
{
}

void GXTextureBlendMaterialProgram::ConfigureVertexFormat(bool indexed)
{
    GXClearVtxDesc();
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_S16, 8);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX1, GX_TEX_ST, GX_S16, 8);

    if (indexed)
    {
        GXSetVtxDesc(GX_VA_POS, GX_INDEX16);
        GXSetVtxDesc(GX_VA_TEX0, GX_INDEX16);
        GXSetVtxDesc(GX_VA_TEX1, GX_INDEX16);
    }
    else
    {
        GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
        GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);
        GXSetVtxDesc(GX_VA_TEX1, GX_DIRECT);
    }
}

void GXTextureBlendMaterialProgram::BindVertexArrays(const glModelPacket* packet)
{
    glModelStream* streams = packet->streams;
    GXSetArray(GX_VA_POS, streams[0].address, 12);
    GXSetArray(GX_VA_TEX0, streams[1].address, 4);
    GXSetArray(GX_VA_TEX1, streams[2].address, 4);
}

void GXTextureBlendMaterialProgram::DrawIndexed(const glModelPacket* packet)
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

void GXTextureBlendMaterialProgram::DrawDirect(const glModelPacket* packet)
{
    GXBegin(glxGetPrimitiveType((unsigned char)packet->primType), GX_VTXFMT0, packet->numUniqueVertices);

    for (unsigned short i = 0; i < packet->numUniqueVertices; ++i)
    {
        WGPIPE.us = i;
        WGPIPE.us = i;
        WGPIPE.us = i;
    }
}

void GXTextureBlendMaterialProgram::BindParameters(const glModelPacket* packet)
{
    glx_BindTexture(0, &static_cast<GXTextureBlendParameters*>(packet->materialParameters)->diffuseTexture);
    glx_BindTexture(1, &static_cast<GXTextureBlendParameters*>(packet->materialParameters)->detailTexture);
}

const GXMaterialParameter* GXTextureBlendMaterialProgram::GetParameters()
{
    return Parameters;
}
