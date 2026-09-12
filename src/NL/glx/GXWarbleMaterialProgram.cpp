#include <revolution/gx.h>

#include "NL/glx/GXMaterialProgram.h"
#include "NL/glx/GXMaterialProgramInternal.h"
#include "NL/gl/glLoadModel.h"
#include "Game/UnidentifiedStaticStorage.h"

GXWarbleMaterialProgram* GXWarbleMaterialProgram::Instance;
bool GXWarbleMaterialProgram::Initialized;

GXMaterialParameter GXWarbleMaterialProgram::Parameters[1] = {
    { 0x69F44DC5, 0x01010103, 0 },
};

GXWarbleMaterialProgram::GXWarbleMaterialProgram()
{
    Instance = this;
    programHash = 0xCFB7215C;
    parameterDataSize = 8;
    parameterCount = 1;
    glRegisterMaterialProgram(this, programHash);
}

GXWarbleMaterialProgram::~GXWarbleMaterialProgram()
{
}

void GXWarbleMaterialProgram::Initialize()
{
    if (Initialized)
        return;
    Initialized = true;
}

void GXWarbleMaterialProgram::Configure(glModelPacket*)
{
}

void GXWarbleMaterialProgram::ConfigureVertexFormat(bool indexed)
{
    GXClearVtxDesc();
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_S16, 10);

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

void GXWarbleMaterialProgram::BindVertexArrays(const glModelPacket* packet)
{
    glModelStream* streams = packet->streams;
    GXSetArray(GX_VA_POS, streams[0].address, 12);
    GXSetArray(GX_VA_CLR0, streams[2].address, 4);
    GXSetArray(GX_VA_TEX0, streams[1].address, 4);
}

void GXWarbleMaterialProgram::DrawIndexed(const glModelPacket* packet)
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

void GXWarbleMaterialProgram::DrawDirect(const glModelPacket* packet)
{
    GXBegin(glxGetPrimitiveType((unsigned char)packet->primType), GX_VTXFMT0, packet->numUniqueVertices);

    for (unsigned short i = 0; i < packet->numUniqueVertices; ++i)
    {
        WGPIPE.us = i;
        WGPIPE.us = i;
        WGPIPE.us = i;
    }
}

void GXWarbleMaterialProgram::BindParameters(const glModelPacket* packet)
{
    glx_BindTexture(0, (glTextureBinding*)(packet->materialParameters));
}

const GXMaterialParameter* GXWarbleMaterialProgram::GetParameters()
{
    return Parameters;
}
