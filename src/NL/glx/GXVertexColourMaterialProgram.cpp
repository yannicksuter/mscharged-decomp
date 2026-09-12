#include <revolution/gx.h>

#include "NL/glx/GXVertexColourMaterialProgram.h"
#include "NL/glx/GXMaterialProgramInternal.h"
#include "NL/gl/glLoadModel.h"
#include "Game/UnidentifiedStaticStorage.h"

GXVertexColourMaterialProgram* GXVertexColourMaterialProgram::Instance;
bool GXVertexColourMaterialProgram::Initialized;

GXVertexColourMaterialProgram::GXVertexColourMaterialProgram()
{
    Instance = this;
    programHash = 0xD701656B;
    parameterDataSize = 0;
    parameterCount = 0;
    glRegisterMaterialProgram(this, programHash);
}

GXVertexColourMaterialProgram::~GXVertexColourMaterialProgram()
{
}

void GXVertexColourMaterialProgram::Initialize()
{
    if (Initialized)
        return;
    Initialized = true;
}

void GXVertexColourMaterialProgram::Configure(glModelPacket*)
{
}

void GXVertexColourMaterialProgram::ConfigureVertexFormat(bool indexed)
{
    GXClearVtxDesc();
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);

    if (indexed)
    {
        GXSetVtxDesc(GX_VA_POS, GX_INDEX16);
        GXSetVtxDesc(GX_VA_CLR0, GX_INDEX16);
    }
    else
    {
        GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
        GXSetVtxDesc(GX_VA_CLR0, GX_DIRECT);
    }
}

void GXVertexColourMaterialProgram::BindVertexArrays(const glModelPacket* packet)
{
    glModelStream* streams = packet->streams;
    GXSetArray(GX_VA_POS, streams[0].address, 12);
    GXSetArray(GX_VA_CLR0, streams[1].address, 4);
}

void GXVertexColourMaterialProgram::DrawIndexed(const glModelPacket* packet)
{
    unsigned short* index = packet->indexBuffer;
    unsigned short* end = index + packet->numVertices;
    GXBegin(glxGetPrimitiveType((unsigned char)packet->primType), GX_VTXFMT0, (unsigned short)packet->numVertices);

    while (index < end)
    {
        WGPIPE.us = *index;
        WGPIPE.us = *index;
        ++index;
    }
}

void GXVertexColourMaterialProgram::DrawDirect(const glModelPacket* packet)
{
    GXBegin(glxGetPrimitiveType((unsigned char)packet->primType), GX_VTXFMT0, packet->numUniqueVertices);

    for (unsigned short i = 0; i < packet->numUniqueVertices; ++i)
    {
        WGPIPE.us = i;
        WGPIPE.us = i;
    }
}

void GXVertexColourMaterialProgram::BindParameters(const glModelPacket* packet)
{
}

const GXMaterialParameter* GXVertexColourMaterialProgram::GetParameters()
{
    return 0;
}
