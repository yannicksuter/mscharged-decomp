#include <revolution/gx.h>

#include "NL/glx/GXTextureColourAddMaterialProgram.h"
#include "NL/glx/glxTexture.h"
#include "NL/glx/GXMaterialProgramInternal.h"
#include "NL/gl/glLoadModel.h"
#include "Game/UnidentifiedStaticStorage.h"

GXTextureColourAddMaterialProgram* GXTextureColourAddMaterialProgram::Instance;
bool GXTextureColourAddMaterialProgram::Initialized;

GXMaterialParameter GXTextureColourAddMaterialProgram::Parameters[2] = {
    { 0x69F44DC5, 0x01010103, 0 },
    { 0xEE9D919D, 0x01040101, 8 },
};

GXTextureColourAddMaterialProgram::GXTextureColourAddMaterialProgram()
{
    Instance = this;
    programHash = 0x9557B266;
    parameterDataSize = 24;
    parameterCount = 2;
    glRegisterMaterialProgram(this, programHash);
}

GXTextureColourAddMaterialProgram::~GXTextureColourAddMaterialProgram()
{
}

void GXTextureColourAddMaterialProgram::Initialize()
{
    if (Initialized)
        return;
    Initialized = true;
}

void GXTextureColourAddMaterialProgram::Configure(glModelPacket*)
{
}

void GXTextureColourAddMaterialProgram::ConfigureVertexFormat(bool indexed)
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

void GXTextureColourAddMaterialProgram::BindVertexArrays(const glModelPacket* packet)
{
    glModelStream* streams = packet->streams;
    GXSetArray(GX_VA_POS, streams[0].address, 12);
    GXSetArray(GX_VA_TEX0, streams[1].address, 8);
}

void GXTextureColourAddMaterialProgram::DrawIndexed(const glModelPacket* packet)
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

void GXTextureColourAddMaterialProgram::DrawDirect(const glModelPacket* packet)
{
    GXBegin(glxGetPrimitiveType((unsigned char)packet->primType), GX_VTXFMT0, packet->numUniqueVertices);

    for (unsigned short i = 0; i < packet->numUniqueVertices; ++i)
    {
        WGPIPE.us = i;
        WGPIPE.us = i;
    }
}

void GXTextureColourAddMaterialProgram::BindParameters(const glModelPacket* packet)
{
    glx_BindTexture(0, &static_cast<GXTextureColourAddParameters*>(packet->materialParameters)->texture);
}

const GXMaterialParameter* GXTextureColourAddMaterialProgram::GetParameters()
{
    return Parameters;
}
