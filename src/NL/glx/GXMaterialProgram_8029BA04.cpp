#include <revolution/gx.h>

#include "NL/glx/GXMaterialProgram.h"

GXMaterialProgram_8029BA04* GXMaterialProgram_8029BA04::Instance;
bool GXMaterialProgram_8029BA04::Initialized;

GXMaterialParameter GXMaterialProgram_8029BA04::Parameters[37] = {
    { 0x69F44DC5, 0x01010103, 0 },
    { 0xEBAF55D2, 0x01010103, 8 },
    { 0x8099480F, 0x01010103, 16 },
    { 0xFB3B01EC, 0x02030411, 24 },
    { 0x0658BB38, 0x01010101, 32 },
    { 0xE824FA5D, 0x01010184, 36 },
    { 0xFB281C86, 0x01030181, 40 },
    { 0xFB281C87, 0x01030181, 52 },
    { 0xFB281C88, 0x01030181, 64 },
    { 0xFB281C89, 0x01030181, 76 },
    { 0xFB2ECAB9, 0x01030181, 88 },
    { 0x602BBA7B, 0x01010181, 100 },
    { 0xFB2ECABA, 0x01030181, 104 },
    { 0x602BBA7C, 0x01010181, 116 },
    { 0xFB2ECABB, 0x01030181, 120 },
    { 0x602BBA7D, 0x01010181, 132 },
    { 0xFB2ECABC, 0x01030181, 136 },
    { 0x602BBA7E, 0x01010181, 148 },
    { 0x2B8286F5, 0x01030181, 152 },
    { 0xBF89C2A7, 0x01010101, 164 },
    { 0x2B8286F6, 0x01030181, 168 },
    { 0xBF89C2A8, 0x01010101, 180 },
    { 0x2B8286F7, 0x01030181, 184 },
    { 0xBF89C2A9, 0x01010101, 196 },
    { 0x2B8286F8, 0x01030181, 200 },
    { 0xBF89C2AA, 0x01010101, 212 },
    { 0xDA88C73B, 0x01040181, 216 },
    { 0xDA88C73C, 0x01040181, 232 },
    { 0xDA88C73D, 0x01040181, 248 },
    { 0xDA88C73E, 0x01040181, 264 },
    { 0xDA88C73F, 0x01040181, 280 },
    { 0xDA88C740, 0x01040181, 296 },
    { 0xDA88C741, 0x01040181, 312 },
    { 0xDA88C742, 0x01040181, 328 },
    { 0x19BE5B9A, 0x02010184, 344 },
    { 0xFFD78956, 0x01010182, 360 },
    { 0x135AB735, 0x01010182, 364 },
};

GXMaterialProgram_8029BA04::GXMaterialProgram_8029BA04()
{
    Instance = this;
    programHash = 0x4BA62CB4;
    parameterDataSize = 368;
    parameterCount = 37;
    fn_802CB790(this, programHash);
}

GXMaterialProgram_8029BA04::~GXMaterialProgram_8029BA04()
{
}

void GXMaterialProgram_8029BA04::Initialize()
{
    if (Initialized)
        return;
    Initialized = true;
}

void GXMaterialProgram_8029BA04::Configure()
{
}

void GXMaterialProgram_8029BA04::ConfigureVertexFormat(bool indexed)
{
    GXClearVtxDesc();
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_NRM, GX_NRM_XYZ, GX_F32, 0);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX1, GX_TEX_ST, GX_F32, 0);

    if (indexed)
    {
        GXSetVtxDesc(GX_VA_POS, GX_INDEX16);
        GXSetVtxDesc(GX_VA_NRM, GX_INDEX16);
        GXSetVtxDesc(GX_VA_TEX0, GX_INDEX16);
        GXSetVtxDesc(GX_VA_TEX1, GX_INDEX16);
    }
    else
    {
        GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
        GXSetVtxDesc(GX_VA_NRM, GX_DIRECT);
        GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);
        GXSetVtxDesc(GX_VA_TEX1, GX_DIRECT);
    }
}

void GXMaterialProgram_8029BA04::BindVertexArrays(const glModelPacket* packet)
{
    glModelStream* streams = packet->streams;
    GXSetArray(GX_VA_POS, streams[0].address, 12);
    GXSetArray(GX_VA_NRM, streams[1].address, 12);
    GXSetArray(GX_VA_TEX0, streams[2].address, 8);
    GXSetArray(GX_VA_TEX1, streams[3].address, 8);
}

void GXMaterialProgram_8029BA04::BindParameters(const glModelPacket* packet)
{
    fn_8036BE88(0, packet->unknown20);
    fn_8036BE88(1, (unsigned char*)packet->unknown20 + 8);
    fn_8036BE88(2, (unsigned char*)packet->unknown20 + 16);
}

const GXMaterialParameter* GXMaterialProgram_8029BA04::GetParameters()
{
    return Parameters;
}
