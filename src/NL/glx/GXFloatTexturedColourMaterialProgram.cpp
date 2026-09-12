#include <revolution/gx.h>

#include "NL/glx/GXFloatTexturedColourMaterialProgram.h"
#include "NL/glx/glxTexture.h"
#include "NL/glx/GXMaterialProgramInternal.h"
#include "NL/gl/glLoadModel.h"
#include "Game/UnidentifiedStaticStorage.h"

GXFloatTexturedColourMaterialProgram* GXFloatTexturedColourMaterialProgram::Instance;
bool GXFloatTexturedColourMaterialProgram::Initialized;

GXMaterialParameter GXFloatTexturedColourMaterialProgram::Parameters[1] = {
    { 0x69F44DC5, 0x01010103, 0 }, // NLG_DIFFUSE
};

GXFloatTexturedColourMaterialProgram::GXFloatTexturedColourMaterialProgram()
{
    Instance = this;
    programHash = 0x19065BF6;
    parameterDataSize = 8;
    parameterCount = 1;
    glRegisterMaterialProgram(this, programHash);
}

GXFloatTexturedColourMaterialProgram::~GXFloatTexturedColourMaterialProgram()
{
}

void GXFloatTexturedColourMaterialProgram::Initialize()
{
    if (Initialized)
        return;
    Initialized = true;
}

void GXFloatTexturedColourMaterialProgram::Configure(glModelPacket*)
{
}

void GXFloatTexturedColourMaterialProgram::ConfigureVertexFormat(bool indexed)
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

void GXFloatTexturedColourMaterialProgram::BindVertexArrays(const glModelPacket* packet)
{
    glModelStream* streams = packet->streams;
    GXSetArray(GX_VA_POS, streams[0].address, 12);
    GXSetArray(GX_VA_CLR0, streams[2].address, 4);
    GXSetArray(GX_VA_TEX0, streams[1].address, 8);
}

void GXFloatTexturedColourMaterialProgram::DrawIndexed(const glModelPacket* packet)
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

void GXFloatTexturedColourMaterialProgram::DrawDirect(const glModelPacket* packet)
{
    GXBegin(glxGetPrimitiveType((unsigned char)packet->primType), GX_VTXFMT0, packet->numUniqueVertices);

    for (unsigned short i = 0; i < packet->numUniqueVertices; ++i)
    {
        WGPIPE.us = i;
        WGPIPE.us = i;
        WGPIPE.us = i;
    }
}

void GXFloatTexturedColourMaterialProgram::BindParameters(const glModelPacket* packet)
{
    glx_BindTexture(0, &static_cast<GXFloatTexturedColourParameters*>(packet->materialParameters)->diffuseTexture);
}

const GXMaterialParameter* GXFloatTexturedColourMaterialProgram::GetParameters()
{
    return Parameters;
}
