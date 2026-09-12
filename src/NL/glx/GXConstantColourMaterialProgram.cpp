#include <revolution/gx.h>

#include "NL/glx/GXMaterialProgram.h"
#include "NL/glx/GXMaterialProgramInternal.h"
#include "NL/gl/glLoadModel.h"
#include "Game/UnidentifiedStaticStorage.h"

GXConstantColourMaterialProgram* GXConstantColourMaterialProgram::Instance;
bool GXConstantColourMaterialProgram::Initialized;

GXMaterialParameter GXConstantColourMaterialProgram::Parameters[2] = {
    { 0x69F44DC5, 0x01010103, 0 },
    { 0xEE9D919D, 0x01040101, 8 },
};

GXConstantColourMaterialProgram::GXConstantColourMaterialProgram()
{
    Instance = this;
    programHash = 0xEE9D919D;
    parameterDataSize = 24;
    parameterCount = 2;
    glRegisterMaterialProgram(this, programHash);
}

GXConstantColourMaterialProgram::~GXConstantColourMaterialProgram()
{
}

void GXConstantColourMaterialProgram::Initialize()
{
    if (Initialized)
        return;
    Initialized = true;
}

void GXConstantColourMaterialProgram::Configure(glModelPacket*)
{
}

void GXConstantColourMaterialProgram::ConfigureVertexFormat(bool indexed)
{
    GXClearVtxDesc();
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_S16, 12);

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

void GXConstantColourMaterialProgram::BindVertexArrays(const glModelPacket* packet)
{
    glModelStream* streams = packet->streams;
    GXSetArray(GX_VA_POS, streams[0].address, 12);
    GXSetArray(GX_VA_TEX0, streams[1].address, 4);
}

void GXConstantColourMaterialProgram::DrawIndexed(const glModelPacket* packet)
{
    unsigned short* idxPtr = packet->indexBuffer;
    unsigned short* end = idxPtr + packet->numVertices;
    GXBegin(UnidentifiedGetPrimitiveType((unsigned char)packet->primType), GX_VTXFMT0, (unsigned short)packet->numVertices);

    while (idxPtr < end)
    {
        WGPIPE.us = *idxPtr;
        WGPIPE.us = *idxPtr;
        ++idxPtr;
    }
}

void GXConstantColourMaterialProgram::DrawDirect(const glModelPacket* packet)
{
    GXBegin(UnidentifiedGetPrimitiveType((unsigned char)packet->primType), GX_VTXFMT0, packet->numUniqueVertices);

    for (unsigned short i = 0; i < packet->numUniqueVertices; ++i)
    {
        WGPIPE.us = i;
        WGPIPE.us = i;
    }
}

void GXConstantColourMaterialProgram::BindParameters(const glModelPacket* packet)
{
    glx_BindTexture(0, (glTextureBinding*)(packet->materialParameters));
}

const GXMaterialParameter* GXConstantColourMaterialProgram::GetParameters()
{
    return Parameters;
}
