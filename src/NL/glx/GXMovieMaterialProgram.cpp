#include <revolution/gx.h>

#include "NL/glx/GXMovieMaterialProgram.h"
#include "NL/glx/GXMaterialProgramInternal.h"
#include "NL/gl/glLoadModel.h"
#include "Game/UnidentifiedStaticStorage.h"

GXMovieMaterialProgram* GXMovieMaterialProgram::Instance;
bool GXMovieMaterialProgram::Initialized;

GXMaterialParameter GXMovieMaterialProgram::Parameters[2] = {
    { 0x69F44DC5, 0x01010103, 0 },
    { 0xEE9D919D, 0x01040101, 8 },
};

GXMovieMaterialProgram::GXMovieMaterialProgram()
{
    Instance = this;
    programHash = 0xEC35CAAB;
    parameterDataSize = 24;
    parameterCount = 2;
    glRegisterMaterialProgram(this, programHash);
}

GXMovieMaterialProgram::~GXMovieMaterialProgram()
{
}

void GXMovieMaterialProgram::Initialize()
{
    if (Initialized)
        return;
    Initialized = true;
}

void GXMovieMaterialProgram::Configure(glModelPacket*)
{
}

void GXMovieMaterialProgram::ConfigureVertexFormat(bool indexed)
{
    GXClearVtxDesc();
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);

    if (indexed)
    {
        GXSetVtxDesc(GX_VA_POS, GX_INDEX16);
        GXSetVtxDesc(GX_VA_TEX0, GX_INDEX16);
    }
    else
    {
        GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
        GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);
    }
}

void GXMovieMaterialProgram::BindVertexArrays(const glModelPacket* packet)
{
    glModelStream* streams = packet->streams;
    GXSetArray(GX_VA_POS, streams[0].address, 12);
    GXSetArray(GX_VA_TEX0, streams[1].address, 8);
}

void GXMovieMaterialProgram::DrawIndexed(const glModelPacket* packet)
{
    unsigned short* idxPtr = packet->indexBuffer;
    unsigned short* end = idxPtr + packet->numVertices;
    GXBegin(glxGetPrimitiveType((unsigned char)packet->primType), GX_VTXFMT0, (unsigned short)packet->numVertices);

    while (idxPtr < end)
    {
        WGPIPE.us = *idxPtr;
        WGPIPE.us = *idxPtr;
        ++idxPtr;
    }
}

void GXMovieMaterialProgram::DrawDirect(const glModelPacket* packet)
{
    GXBegin(glxGetPrimitiveType((unsigned char)packet->primType), GX_VTXFMT0, packet->numUniqueVertices);

    for (unsigned short i = 0; i < packet->numUniqueVertices; ++i)
    {
        WGPIPE.us = i;
        WGPIPE.us = i;
    }
}

const GXMaterialParameter* GXMovieMaterialProgram::GetParameters()
{
    return Parameters;
}
