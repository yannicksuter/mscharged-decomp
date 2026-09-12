#include <revolution/gx.h>

#include "NL/glx/GXUnlitTextureMaterialProgram.h"
#include "NL/glx/glxTexture.h"
#include "NL/glx/GXMaterialProgramInternal.h"
#include "NL/gl/glLoadModel.h"
#include "Game/UnidentifiedStaticStorage.h"

GXUnlitTextureMaterialProgram* GXUnlitTextureMaterialProgram::Instance;
bool GXUnlitTextureMaterialProgram::Initialized;

GXMaterialParameter GXUnlitTextureMaterialProgram::Parameters[1] = {
    { 0x69F44DC5, 0x01010103, 0 },
};

GXUnlitTextureMaterialProgram::GXUnlitTextureMaterialProgram()
{
    Instance = this;
    programHash = 0x21DB4385;
    parameterDataSize = 8;
    parameterCount = 1;
    glRegisterMaterialProgram(this, programHash);
}

GXUnlitTextureMaterialProgram::~GXUnlitTextureMaterialProgram()
{
}

void GXUnlitTextureMaterialProgram::Initialize()
{
    if (Initialized)
        return;
    Initialized = true;
}

void GXUnlitTextureMaterialProgram::Configure(glModelPacket*)
{
}

void GXUnlitTextureMaterialProgram::ConfigureVertexFormat(bool indexed)
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

void GXUnlitTextureMaterialProgram::BindVertexArrays(const glModelPacket* packet)
{
    glModelStream* streams = packet->streams;
    GXSetArray(GX_VA_POS, streams[0].address, 12);
    GXSetArray(GX_VA_TEX0, streams[1].address, 8);
}

void GXUnlitTextureMaterialProgram::DrawIndexed(const glModelPacket* packet)
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

void GXUnlitTextureMaterialProgram::DrawDirect(const glModelPacket* packet)
{
    GXBegin(glxGetPrimitiveType((unsigned char)packet->primType), GX_VTXFMT0, packet->numUniqueVertices);

    for (unsigned short i = 0; i < packet->numUniqueVertices; ++i)
    {
        WGPIPE.us = i;
        WGPIPE.us = i;
    }
}

void GXUnlitTextureMaterialProgram::BindParameters(const glModelPacket* packet)
{
    glx_BindTexture(0, &static_cast<GXUnlitTextureParameters*>(packet->materialParameters)->texture);
}

const GXMaterialParameter* GXUnlitTextureMaterialProgram::GetParameters()
{
    return Parameters;
}
