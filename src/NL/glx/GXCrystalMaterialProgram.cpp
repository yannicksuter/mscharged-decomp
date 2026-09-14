#include <revolution/gx.h>

#include "Game/TweakValue.h"
#include "Game/UnidentifiedStaticStorage.h"
#include "NL/glx/GXCrystalMaterialProgram.h"
#include "NL/glx/glxTexture.h"
#include "NL/glx/GXMaterialProgramInternal.h"
#include "NL/gl/glLoadModel.h"

GXCrystalMaterialProgram* GXCrystalMaterialProgram::Instance;
bool GXCrystalMaterialProgram::Initialized;

GXMaterialParameter GXCrystalMaterialProgram::Parameters[3] = {
    { 0xEBAF55D2, 0x01010103, 0 }, // NLG_DETAIL
    { 0x69F44DC5, 0x01010103, 8 }, // NLG_DIFFUSE
    { 0x8099480F, 0x01010103, 16 }, // NLG_RAMP
};

TweakFloatBinding sCrystalSilhouetteGlowMultiplier;
TweakFloatBinding sCrystalEdgeGlowMultiplier;

GXCrystalMaterialProgram::GXCrystalMaterialProgram()
{
    Instance = this;
    programHash = 0xEB3E6061;
    parameterDataSize = sizeof(GXCrystalMaterialParameters);
    parameterCount = 3;
    glRegisterMaterialProgram(this, programHash);
}

GXCrystalMaterialProgram::~GXCrystalMaterialProgram()
{
}

void GXCrystalMaterialProgram::Initialize()
{
    if (Initialized)
        return;

    sCrystalSilhouetteGlowMultiplier.BindWithDefault("SilhouetteGlowMultiplier", 1.0f,
        "Materials/", false, 0.0f, 1.0f, 0.05f);
    sCrystalEdgeGlowMultiplier.BindWithDefault("EdgeGlowMultiplier", 1.0f,
        "Materials/", false, 0.0f, 1.0f, 0.05f);

    Initialized = true;
}

void GXCrystalMaterialProgram::Configure(glModelPacket*)
{
}

void GXCrystalMaterialProgram::ConfigureVertexFormat(bool indexed)
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

void GXCrystalMaterialProgram::BindVertexArrays(
    const glModelPacket* packet)
{
    glModelStream* streams = packet->streams;
    GXSetArray(GX_VA_POS, streams[0].address, 12);
    GXSetArray(GX_VA_NRM, streams[1].address, 3);
    GXSetArray(GX_VA_CLR0, streams[5].address, 4);
    GXSetArray(GX_VA_TEX0, streams[2].address, 4);
    GXSetArray(GX_VA_TEX1, streams[3].address, 4);
    GXSetArray(GX_VA_TEX2, streams[4].address, 4);
}

void GXCrystalMaterialProgram::DrawIndexed(const glModelPacket* packet)
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

void GXCrystalMaterialProgram::DrawDirect(const glModelPacket* packet)
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

void GXCrystalMaterialProgram::BindParameters(
    const glModelPacket* packet)
{
    glx_BindTexture(0, &static_cast<GXCrystalMaterialParameters*>(packet->materialParameters)->detailTexture);
    glx_BindTexture(1, &static_cast<GXCrystalMaterialParameters*>(packet->materialParameters)->diffuseTexture);
    glx_BindTexture(2, &static_cast<GXCrystalMaterialParameters*>(packet->materialParameters)->rampTexture);
}
