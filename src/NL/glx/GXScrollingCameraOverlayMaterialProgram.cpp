#include <revolution/gx.h>

#include "NL/glx/GXScrollingCameraOverlayMaterialProgram.h"
#include "NL/glx/glxTexture.h"
#include "NL/glx/GXMaterialProgramInternal.h"
#include "NL/gl/glLoadModel.h"
#include "Game/UnidentifiedStaticStorage.h"

GXScrollingCameraOverlayMaterialProgram* GXScrollingCameraOverlayMaterialProgram::Instance;
bool GXScrollingCameraOverlayMaterialProgram::Initialized;

GXMaterialParameter GXScrollingCameraOverlayMaterialProgram::Parameters[12] = {
    { 0x69F44DC5, 0x01010103, 0 },
    { 0x93014DE7, 0x01010103, 8 },
    { 0x80968AEB, 0x01010103, 16 },
    { 0xEA1892E3, 0x01010101, 24 },
    { 0x1FCE8268, 0x01010101, 28 },
    { 0xE99B5893, 0x01010101, 32 },
    { 0xE516A611, 0x01010102, 36 },
    { 0x8E10B600, 0x01010102, 40 },
    { 0x608D143C, 0x01010101, 44 },
    { 0x6497A55D, 0x01010101, 48 },
    { 0xA80B4402, 0x01010102, 52 },
    { 0x26839970, 0x01010102, 56 },
};

GXScrollingCameraOverlayMaterialProgram::GXScrollingCameraOverlayMaterialProgram()
{
    Instance = this;
    programHash = 0x845CAD59;
    parameterDataSize = 60;
    parameterCount = 12;
    glRegisterMaterialProgram(this, programHash);
}

GXScrollingCameraOverlayMaterialProgram::~GXScrollingCameraOverlayMaterialProgram()
{
}

void GXScrollingCameraOverlayMaterialProgram::Initialize()
{
    if (Initialized)
        return;
    Initialized = true;
}

void GXScrollingCameraOverlayMaterialProgram::Configure(glModelPacket*)
{
}

void GXScrollingCameraOverlayMaterialProgram::ConfigureVertexFormat(bool indexed)
{
    GXClearVtxDesc();
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_NRM, GX_NRM_XYZ, GX_S8, 6);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_S16, 10);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX1, GX_TEX_ST, GX_S16, 10);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX2, GX_TEX_ST, GX_S16, 10);

    if (indexed)
    {
        GXSetVtxDesc(GX_VA_POS, GX_INDEX16);
        GXSetVtxDesc(GX_VA_NRM, GX_INDEX16);
        GXSetVtxDesc(GX_VA_CLR0, GX_INDEX16);
        GXSetVtxDesc(GX_VA_TEX0, GX_INDEX16);
        GXSetVtxDesc(GX_VA_TEX1, GX_INDEX16);
        GXSetVtxDesc(GX_VA_TEX2, GX_INDEX16);
    }
    else
    {
        GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
        GXSetVtxDesc(GX_VA_NRM, GX_DIRECT);
        GXSetVtxDesc(GX_VA_CLR0, GX_DIRECT);
        GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);
        GXSetVtxDesc(GX_VA_TEX1, GX_DIRECT);
        GXSetVtxDesc(GX_VA_TEX2, GX_DIRECT);
    }
}

void GXScrollingCameraOverlayMaterialProgram::BindVertexArrays(const glModelPacket* packet)
{
    glModelStream* streams = packet->streams;
    GXSetArray(GX_VA_POS, streams[0].address, 12);
    GXSetArray(GX_VA_NRM, streams[1].address, 3);
    GXSetArray(GX_VA_CLR0, streams[5].address, 4);
    GXSetArray(GX_VA_TEX0, streams[2].address, 4);
    GXSetArray(GX_VA_TEX1, streams[3].address, 4);
    GXSetArray(GX_VA_TEX2, streams[4].address, 4);
}

void GXScrollingCameraOverlayMaterialProgram::DrawIndexed(const glModelPacket* packet)
{
    unsigned short* idxPtr = packet->indexBuffer;
    unsigned short* end = idxPtr + packet->numVertices;
    GXBegin(glxGetPrimitiveType((unsigned char)packet->primType), GX_VTXFMT0, (unsigned short)packet->numVertices);

    while (idxPtr < end)
    {
        WGPIPE.us = *idxPtr;
        WGPIPE.us = *idxPtr;
        WGPIPE.us = *idxPtr;
        WGPIPE.us = *idxPtr;
        WGPIPE.us = *idxPtr;
        WGPIPE.us = *idxPtr;
        ++idxPtr;
    }
}

void GXScrollingCameraOverlayMaterialProgram::DrawDirect(const glModelPacket* packet)
{
    GXBegin(glxGetPrimitiveType((unsigned char)packet->primType), GX_VTXFMT0, packet->numUniqueVertices);

    for (unsigned short i = 0; i < packet->numUniqueVertices; ++i)
    {
        WGPIPE.us = i;
        WGPIPE.us = i;
        WGPIPE.us = i;
        WGPIPE.us = i;
        WGPIPE.us = i;
        WGPIPE.us = i;
    }
}

void GXScrollingCameraOverlayMaterialProgram::BindParameters(const glModelPacket* packet)
{
    glx_BindTexture(0, &static_cast<GXScrollingCameraOverlayParameters*>(packet->materialParameters)->diffuseTexture);
    glx_BindTexture(1, &static_cast<GXScrollingCameraOverlayParameters*>(packet->materialParameters)->overlayTexture);
    glx_BindTexture(2, &static_cast<GXScrollingCameraOverlayParameters*>(packet->materialParameters)->overlayMaskTexture);
}

const GXMaterialParameter* GXScrollingCameraOverlayMaterialProgram::GetParameters()
{
    return Parameters;
}
