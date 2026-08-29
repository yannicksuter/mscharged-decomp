#include <revolution/gx.h>

#include "Game/TweakValue.h"
#include "NL/glx/GXMaterialProgram.h"

GXMaterialProgram_80299CA0* GXMaterialProgram_80299CA0::Instance;
bool GXMaterialProgram_80299CA0::Initialized;

GXMaterialParameter GXMaterialProgram_80299CA0::Parameters[3] = {
    { 0xEBAF55D2, 0x01010103, 0 },
    { 0x69F44DC5, 0x01010103, 8 },
    { 0x8099480F, 0x01010103, 16 },
};

TweakValue_804F4DC8 sCrystalTweak_8057B400;
TweakValue_804F4DC8 sCrystalTweak_8057B420;

GXMaterialProgram_80299CA0::GXMaterialProgram_80299CA0()
{
    Instance = this;
    programHash = 0xEB3E6061;
    parameterDataSize = sizeof(GXMaterialProgramParameters_80299CA0);
    parameterCount = 3;
    fn_802CB790(this, programHash);
}

GXMaterialProgram_80299CA0::~GXMaterialProgram_80299CA0()
{
}

void GXMaterialProgram_80299CA0::Initialize()
{
    if (Initialized)
        return;

    bool registered = sCrystalTweak_8057B400.fn_802C4FEC(
        "SilhouetteGlowMultiplier", 0.0f, "Materials/", false, 1.0f, 0.05f);
    if (!registered)
        sCrystalTweak_8057B400 = sCrystalTweak_8057B400.GetDefaultValue();
    if (!registered)
        sCrystalTweak_8057B400 = 1.0f;

    registered = sCrystalTweak_8057B420.fn_802C4FEC(
        "EdgeGlowMultiplier", 0.0f, "Materials/", false, 1.0f, 0.05f);
    if (!registered)
        sCrystalTweak_8057B420 = sCrystalTweak_8057B420.GetDefaultValue();
    if (!registered)
        sCrystalTweak_8057B420 = 1.0f;

    Initialized = true;
}

void GXMaterialProgram_80299CA0::Configure()
{
}

void GXMaterialProgram_80299CA0::ConfigureVertexFormat(bool indexed)
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

void GXMaterialProgram_80299CA0::BindVertexArrays(
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

void GXMaterialProgram_80299CA0::DrawIndexed(const glModelPacket* packet)
{
    unsigned short* idxPtr = packet->indexBuffer;
    unsigned short* end = idxPtr + packet->numVertices;
    GXBegin(lbl_80524470[(unsigned char)packet->primType], GX_VTXFMT0, (unsigned short)packet->numVertices);

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

void GXMaterialProgram_80299CA0::DrawDirect(const glModelPacket* packet)
{
    GXBegin(lbl_80524470[(unsigned char)packet->primType], GX_VTXFMT0, packet->numUniqueVertices);

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

void GXMaterialProgram_80299CA0::BindParameters(
    const glModelPacket* packet)
{
    fn_8036BE88(0, packet->unknown20);
    fn_8036BE88(1, (unsigned char*)packet->unknown20 + 8);
    fn_8036BE88(2, (unsigned char*)packet->unknown20 + 16);
}
