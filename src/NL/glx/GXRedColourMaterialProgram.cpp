#include <revolution/gx.h>

#include "NL/glx/GXMaterialProgram.h"
#include "NL/glx/GXMaterialProgramInternal.h"
#include "Game/UnidentifiedStaticStorage.h"

GXRedColourMaterialProgram* GXRedColourMaterialProgram::Instance;
bool GXRedColourMaterialProgram::Initialized;

GXRedColourMaterialProgram::GXRedColourMaterialProgram()
{
    Instance = this;
    programHash = 0xDC56470F;
    parameterDataSize = 0;
    parameterCount = 0;
    glRegisterMaterialProgram(this, programHash);
}

GXRedColourMaterialProgram::~GXRedColourMaterialProgram()
{
}

void GXRedColourMaterialProgram::Initialize()
{
    if (Initialized)
        return;
    Initialized = true;
}

void GXRedColourMaterialProgram::Configure(glModelPacket*)
{
}

void GXRedColourMaterialProgram::ConfigureVertexFormat(bool indexed)
{
    GXClearVtxDesc();
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);

    if (indexed)
    {
        GXSetVtxDesc(GX_VA_POS, GX_INDEX16);
    }
    else
    {
        GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
    }
}

void GXRedColourMaterialProgram::BindVertexArrays(const glModelPacket* packet)
{
    GXSetArray(GX_VA_POS, packet->streams[0].address, 12);
}

void GXRedColourMaterialProgram::DrawIndexed(const glModelPacket* packet)
{
    unsigned short* idxPtr = packet->indexBuffer;
    unsigned short* end = idxPtr + packet->numVertices;
    GXBegin(glxGetPrimitiveType((unsigned char)packet->primType), GX_VTXFMT0, (unsigned short)packet->numVertices);

    while (idxPtr < end)
    {
        WGPIPE.us = *idxPtr;
        ++idxPtr;
    }
}

void GXRedColourMaterialProgram::DrawDirect(const glModelPacket* packet)
{
    GXBegin(glxGetPrimitiveType((unsigned char)packet->primType), GX_VTXFMT0, packet->numUniqueVertices);

    for (unsigned short i = 0; i < packet->numUniqueVertices; ++i)
    {
        WGPIPE.us = i;
    }
}

void GXRedColourMaterialProgram::BindParameters(const glModelPacket* packet)
{
}

const GXMaterialParameter* GXRedColourMaterialProgram::GetParameters()
{
    return 0;
}
