#include <revolution/gx.h>

#include "NL/glx/GXMaskedDetailBlendMaterialProgram.h"
#include "NL/glx/glxTexture.h"
#include "NL/glx/GXMaterialProgramInternal.h"
#include "NL/gl/glLoadModel.h"
#include "Game/UnidentifiedStaticStorage.h"

GXMaskedDetailBlendMaterialProgram* GXMaskedDetailBlendMaterialProgram::Instance;
bool GXMaskedDetailBlendMaterialProgram::Initialized;

GXMaterialParameter GXMaskedDetailBlendMaterialProgram::Parameters[6] = {
    { 0x69F44DC5, 0x01010103, 0 }, // NLG_DIFFUSE
    { 0xEBAF55D2, 0x01010103, 8 }, // NLG_DETAIL
    { 0x80968AEB, 0x01010103, 16 }, // NLG_MASK
    { 0x0658BB38, 0x01010101, 24 }, // blendAmount
    { 0x8E10B600, 0x01010102, 28 }, // lightingEnabled
    { 0x26839970, 0x01010102, 32 }, // receiveShadows
};

GXMaskedDetailBlendMaterialProgram::GXMaskedDetailBlendMaterialProgram()
{
    Instance = this;
    programHash = 0x09609A35;
    parameterDataSize = 36;
    parameterCount = 6;
    glRegisterMaterialProgram(this, programHash);
}

GXMaskedDetailBlendMaterialProgram::~GXMaskedDetailBlendMaterialProgram()
{
}

void GXMaskedDetailBlendMaterialProgram::Initialize()
{
    if (Initialized)
        return;
    Initialized = true;
}

void GXMaskedDetailBlendMaterialProgram::Configure(glModelPacket*)
{
}

void GXMaskedDetailBlendMaterialProgram::ConfigureVertexFormat(bool indexed)
{
    GXClearVtxDesc();
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_NRM, GX_NRM_XYZ, GX_S8, 6);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_S16, 10);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX1, GX_TEX_ST, GX_S16, 10);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX2, GX_TEX_ST, GX_S16, 10);

    if (indexed)
    {
        GXSetVtxDesc(GX_VA_POS, GX_INDEX16);
        GXSetVtxDesc(GX_VA_NRM, GX_INDEX16);
        GXSetVtxDesc(GX_VA_CLR0, GX_INDEX16);
        GXSetVtxDesc(GX_VA_TEX0, GX_INDEX16);
        GXSetVtxDesc(GX_VA_TEX1, GX_INDEX16);
        GXSetVtxDesc(GX_VA_TEX2, GX_INDEX16);
    }
    else
    {
        GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
        GXSetVtxDesc(GX_VA_NRM, GX_DIRECT);
        GXSetVtxDesc(GX_VA_CLR0, GX_DIRECT);
        GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);
        GXSetVtxDesc(GX_VA_TEX1, GX_DIRECT);
        GXSetVtxDesc(GX_VA_TEX2, GX_DIRECT);
    }
}

void GXMaskedDetailBlendMaterialProgram::BindVertexArrays(const glModelPacket* packet)
{
    glModelStream* streams = packet->streams;
    GXSetArray(GX_VA_POS, streams[0].address, 12);
    GXSetArray(GX_VA_NRM, streams[1].address, 3);
    GXSetArray(GX_VA_CLR0, streams[5].address, 4);
    GXSetArray(GX_VA_TEX0, streams[2].address, 4);
    GXSetArray(GX_VA_TEX1, streams[3].address, 4);
    GXSetArray(GX_VA_TEX2, streams[4].address, 4);
}

void GXMaskedDetailBlendMaterialProgram::DrawIndexed(const glModelPacket* packet)
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

void GXMaskedDetailBlendMaterialProgram::DrawDirect(const glModelPacket* packet)
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

void GXMaskedDetailBlendMaterialProgram::BindParameters(const glModelPacket* packet)
{
    glx_BindTexture(0, &static_cast<GXMaskedDetailBlendParameters*>(packet->materialParameters)->diffuseTexture);
    glx_BindTexture(1, &static_cast<GXMaskedDetailBlendParameters*>(packet->materialParameters)->detailTexture);
    glx_BindTexture(2, &static_cast<GXMaskedDetailBlendParameters*>(packet->materialParameters)->blendMaskTexture);
}

const GXMaterialParameter* GXMaskedDetailBlendMaterialProgram::GetParameters()
{
    return Parameters;
}
