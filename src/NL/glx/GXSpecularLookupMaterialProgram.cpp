#include <revolution/gx.h>

#include "NL/glx/GXSpecularLookupMaterialProgram.h"
#include "NL/glx/glxTexture.h"
#include "NL/glx/GXMaterialProgramInternal.h"
#include "Game/UnidentifiedStaticStorage.h"

GXSpecularLookupMaterialProgram* GXSpecularLookupMaterialProgram::Instance;
bool GXSpecularLookupMaterialProgram::Initialized;

GXMaterialParameter GXSpecularLookupMaterialProgram::Parameters[3] = {
    { 0x69F44DC5, 0x01010103, 0 }, // NLG_DIFFUSE
    { 0x98A598DE, 0x01010103, 8 }, // NLG_SPECULAR
    { 0x93014DE7, 0x01010103, 16 }, // NLG_GLOSS
};

GXSpecularLookupMaterialProgram::GXSpecularLookupMaterialProgram()
{
    Instance = this;
    programHash = 0x966C340D;
    parameterDataSize = 24;
    parameterCount = 3;
    glRegisterMaterialProgram(this, programHash);
}

GXSpecularLookupMaterialProgram::~GXSpecularLookupMaterialProgram()
{
}

void GXSpecularLookupMaterialProgram::Initialize()
{
    if (Initialized)
        return;
    Initialized = true;
}

void GXSpecularLookupMaterialProgram::Configure(glModelPacket*)
{
}

void GXSpecularLookupMaterialProgram::ConfigureVertexFormat(bool indexed)
{
    GXClearVtxDesc();
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_NRM, GX_NRM_XYZ, GX_S8, 6);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_S16, 10);

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

void GXSpecularLookupMaterialProgram::BindVertexArrays(const glModelPacket* packet)
{
    glModelStream* streams = packet->streams;
    GXSetArray(GX_VA_POS, streams[0].address, 12);
    GXSetArray(GX_VA_NRM, streams[1].address, 3);
    GXSetArray(GX_VA_TEX0, streams[2].address, 4);
}

void GXSpecularLookupMaterialProgram::DrawIndexed(const glModelPacket* packet)
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

void GXSpecularLookupMaterialProgram::DrawDirect(const glModelPacket* packet)
{
    GXBegin(glxGetPrimitiveType((unsigned char)packet->primType), GX_VTXFMT0, packet->numUniqueVertices);

    for (unsigned short i = 0; i < packet->numUniqueVertices; ++i)
    {
        WGPIPE.us = i;
        WGPIPE.us = i;
        WGPIPE.us = i;
    }
}

void GXSpecularLookupMaterialProgram::BindParameters(const glModelPacket* packet)
{
    glx_BindTexture(0, &static_cast<GXSpecularLookupParameters*>(packet->materialParameters)->diffuseTexture);
    glx_BindTexture(1, &static_cast<GXSpecularLookupParameters*>(packet->materialParameters)->specularTexture);
    glx_BindTexture(2, &static_cast<GXSpecularLookupParameters*>(packet->materialParameters)->glossTexture);
}

const GXMaterialParameter* GXSpecularLookupMaterialProgram::GetParameters()
{
    return Parameters;
}
