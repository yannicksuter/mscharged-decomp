#include <revolution/gx.h>

#include "NL/glx/GXMaterialProgram.h"
#include "NL/gl/glLoadModel.h"
#include "Game/UnidentifiedStaticStorage.h"

GXMaterialProgram_8029AB0C* GXMaterialProgram_8029AB0C::Instance;
bool GXMaterialProgram_8029AB0C::Initialized;

GXMaterialParameter GXMaterialProgram_8029AB0C::Parameters[3] = {
    { 0x69F44DC5, 0x01010103, 0 },
    { 0x98A598DE, 0x01010103, 8 },
    { 0x93014DE7, 0x01010103, 16 },
};

GXMaterialProgram_8029AB0C::GXMaterialProgram_8029AB0C()
{
    Instance = this;
    programHash = 0x966C340D;
    parameterDataSize = 24;
    parameterCount = 3;
    glRegisterMaterialProgram(this, programHash);
}

GXMaterialProgram_8029AB0C::~GXMaterialProgram_8029AB0C()
{
}

void GXMaterialProgram_8029AB0C::Initialize()
{
    if (Initialized)
        return;
    Initialized = true;
}

void GXMaterialProgram_8029AB0C::Configure(glModelPacket*)
{
}

void GXMaterialProgram_8029AB0C::ConfigureVertexFormat(bool indexed)
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

void GXMaterialProgram_8029AB0C::BindVertexArrays(const glModelPacket* packet)
{
    glModelStream* streams = packet->streams;
    GXSetArray(GX_VA_POS, streams[0].address, 12);
    GXSetArray(GX_VA_NRM, streams[1].address, 3);
    GXSetArray(GX_VA_TEX0, streams[2].address, 4);
}

void GXMaterialProgram_8029AB0C::DrawIndexed(const glModelPacket* packet)
{
    unsigned short* idxPtr = packet->indexBuffer;
    unsigned short* end = idxPtr + packet->numVertices;
    GXBegin(glx_PrimitiveTypes[(unsigned char)packet->primType], GX_VTXFMT0, (unsigned short)packet->numVertices);

    while (idxPtr < end)
    {
        WGPIPE.us = *idxPtr;
        WGPIPE.us = *idxPtr;
        WGPIPE.us = *idxPtr;
        ++idxPtr;
    }
}

void GXMaterialProgram_8029AB0C::DrawDirect(const glModelPacket* packet)
{
    GXBegin(glx_PrimitiveTypes[(unsigned char)packet->primType], GX_VTXFMT0, packet->numUniqueVertices);

    for (unsigned short i = 0; i < packet->numUniqueVertices; ++i)
    {
        WGPIPE.us = i;
        WGPIPE.us = i;
        WGPIPE.us = i;
    }
}

void GXMaterialProgram_8029AB0C::BindParameters(const glModelPacket* packet)
{
    glx_BindTexture(0, (glTextureBinding*)(packet->materialParameters));
    glx_BindTexture(1, (glTextureBinding*)((unsigned char*)packet->materialParameters + 8));
    glx_BindTexture(2, (glTextureBinding*)((unsigned char*)packet->materialParameters + 16));
}

const GXMaterialParameter* GXMaterialProgram_8029AB0C::GetParameters()
{
    return Parameters;
}
