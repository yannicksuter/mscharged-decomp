#include <revolution/gx.h>

#include "NL/glx/GXMaskedDiffuseBlendMaterialProgram.h"
#include "NL/glx/glxTexture.h"
#include "NL/glx/GXMaterialProgramInternal.h"
#include "Game/UnidentifiedStaticStorage.h"

GXMaskedDiffuseBlendMaterialProgram* GXMaskedDiffuseBlendMaterialProgram::Instance;
bool GXMaskedDiffuseBlendMaterialProgram::Initialized;

GXMaterialParameter GXMaskedDiffuseBlendMaterialProgram::Parameters[3] = {
    { 0x69F44DC5, 0x01010103, 0 }, // NLG_DIFFUSE
    { 0x9367E87C, 0x01010103, 8 }, // blend texture
    { 0x9367E87D, 0x01010103, 16 }, // blend mask texture
};

GXMaskedDiffuseBlendMaterialProgram::GXMaskedDiffuseBlendMaterialProgram()
{
    Instance = this;
    programHash = 0x78D0AF4A;
    parameterDataSize = 24;
    parameterCount = 3;
    glRegisterMaterialProgram(this, programHash);
}

GXMaskedDiffuseBlendMaterialProgram::~GXMaskedDiffuseBlendMaterialProgram()
{
}

void GXMaskedDiffuseBlendMaterialProgram::Initialize()
{
    if (Initialized)
        return;
    Initialized = true;
}

void GXMaskedDiffuseBlendMaterialProgram::Configure(glModelPacket*)
{
}

void GXMaskedDiffuseBlendMaterialProgram::ConfigureVertexFormat(bool indexed)
{
    GXClearVtxDesc();
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX1, GX_TEX_ST, GX_F32, 0);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX2, GX_TEX_ST, GX_F32, 0);

    if (indexed)
    {
        GXSetVtxDesc(GX_VA_POS, GX_INDEX16);
        GXSetVtxDesc(GX_VA_CLR0, GX_INDEX16);
        GXSetVtxDesc(GX_VA_TEX0, GX_INDEX16);
        GXSetVtxDesc(GX_VA_TEX1, GX_INDEX16);
        GXSetVtxDesc(GX_VA_TEX2, GX_INDEX16);
    }
    else
    {
        GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
        GXSetVtxDesc(GX_VA_CLR0, GX_DIRECT);
        GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);
        GXSetVtxDesc(GX_VA_TEX1, GX_DIRECT);
        GXSetVtxDesc(GX_VA_TEX2, GX_DIRECT);
    }
}

void GXMaskedDiffuseBlendMaterialProgram::BindVertexArrays(const glModelPacket* packet)
{
    glModelStream* streams = packet->streams;
    GXSetArray(GX_VA_POS, streams[0].address, 12);
    GXSetArray(GX_VA_CLR0, streams[4].address, 4);
    GXSetArray(GX_VA_TEX0, streams[1].address, 8);
    GXSetArray(GX_VA_TEX1, streams[2].address, 8);
    GXSetArray(GX_VA_TEX2, streams[3].address, 8);
}

void GXMaskedDiffuseBlendMaterialProgram::DrawIndexed(const glModelPacket* packet)
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
        ++idxPtr;
    }
}

void GXMaskedDiffuseBlendMaterialProgram::DrawDirect(const glModelPacket* packet)
{
    GXBegin(glxGetPrimitiveType((unsigned char)packet->primType), GX_VTXFMT0, packet->numUniqueVertices);

    for (unsigned short i = 0; i < packet->numUniqueVertices; ++i)
    {
        WGPIPE.us = i;
        WGPIPE.us = i;
        WGPIPE.us = i;
        WGPIPE.us = i;
        WGPIPE.us = i;
    }
}

void GXMaskedDiffuseBlendMaterialProgram::BindParameters(const glModelPacket* packet)
{
    glx_BindTexture(0, &static_cast<GXMaskedDiffuseBlendParameters*>(packet->materialParameters)->diffuseTexture);
    glx_BindTexture(1, &static_cast<GXMaskedDiffuseBlendParameters*>(packet->materialParameters)->blendTexture);
    glx_BindTexture(2, &static_cast<GXMaskedDiffuseBlendParameters*>(packet->materialParameters)->blendMaskTexture);
}

const GXMaterialParameter* GXMaskedDiffuseBlendMaterialProgram::GetParameters()
{
    return Parameters;
}
