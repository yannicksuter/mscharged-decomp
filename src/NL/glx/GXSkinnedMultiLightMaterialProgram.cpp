#include <revolution/gx.h>

#include "NL/glx/GXSkinnedMultiLightMaterialProgram.h"
#include "NL/glx/glxTexture.h"
#include "NL/glx/GXMaterialProgramInternal.h"
#include "Game/UnidentifiedStaticStorage.h"

GXSkinnedMultiLightMaterialProgram* GXSkinnedMultiLightMaterialProgram::Instance;
bool GXSkinnedMultiLightMaterialProgram::Initialized;

GXMaterialParameter GXSkinnedMultiLightMaterialProgram::Parameters[37] = {
    { 0x69F44DC5, 0x01010103, 0 }, // NLG_DIFFUSE
    { 0xEBAF55D2, 0x01010103, 8 }, // NLG_DETAIL
    { 0x8099480F, 0x01010103, 16 }, // NLG_RAMP
    { 0xFB3B01EC, 0x02030411, 24 }, // SkinMatrices
    { 0x0658BB38, 0x01010101, 32 }, // blendAmount
    { 0xE824FA5D, 0x01010184, 36 }, // animated normal texture coordinates
    { 0xFB281C86, 0x01030181, 40 }, // lightDir0
    { 0xFB281C87, 0x01030181, 52 }, // lightDir1
    { 0xFB281C88, 0x01030181, 64 }, // lightDir2
    { 0xFB281C89, 0x01030181, 76 }, // lightDir3
    { 0xFB2ECAB9, 0x01030181, 88 }, // lightPos0
    { 0x602BBA7B, 0x01010181, 100 }, // lightDist0
    { 0xFB2ECABA, 0x01030181, 104 }, // lightPos1
    { 0x602BBA7C, 0x01010181, 116 }, // lightDist1
    { 0xFB2ECABB, 0x01030181, 120 }, // lightPos2
    { 0x602BBA7D, 0x01010181, 132 }, // lightDist2
    { 0xFB2ECABC, 0x01030181, 136 }, // lightPos3
    { 0x602BBA7E, 0x01010181, 148 }, // lightDist3
    { 0x2B8286F5, 0x01030181, 152 }, // specular direction 0
    { 0xBF89C2A7, 0x01010101, 164 }, // specular exponent 0
    { 0x2B8286F6, 0x01030181, 168 }, // specular direction 1
    { 0xBF89C2A8, 0x01010101, 180 }, // specular exponent 1
    { 0x2B8286F7, 0x01030181, 184 }, // specular direction 2
    { 0xBF89C2A9, 0x01010101, 196 }, // specular exponent 2
    { 0x2B8286F8, 0x01030181, 200 }, // specular direction 3
    { 0xBF89C2AA, 0x01010101, 212 }, // specular exponent 3
    { 0xDA88C73B, 0x01040181, 216 }, // lightColour0
    { 0xDA88C73C, 0x01040181, 232 }, // lightColour1
    { 0xDA88C73D, 0x01040181, 248 }, // lightColour2
    { 0xDA88C73E, 0x01040181, 264 }, // lightColour3
    { 0xDA88C73F, 0x01040181, 280 }, // lightColour4
    { 0xDA88C740, 0x01040181, 296 }, // lightColour5
    { 0xDA88C741, 0x01040181, 312 }, // lightColour6
    { 0xDA88C742, 0x01040181, 328 }, // lightColour7
    { 0x19BE5B9A, 0x02010184, 344 }, // directional light selection
    { 0xFFD78956, 0x01010182, 360 }, // diffuse light count
    { 0x135AB735, 0x01010182, 364 }, // specular light count
};

GXSkinnedMultiLightMaterialProgram::GXSkinnedMultiLightMaterialProgram()
{
    Instance = this;
    programHash = 0x4BA62CB4;
    parameterDataSize = 368;
    parameterCount = 37;
    glRegisterMaterialProgram(this, programHash);
}

GXSkinnedMultiLightMaterialProgram::~GXSkinnedMultiLightMaterialProgram()
{
}

void GXSkinnedMultiLightMaterialProgram::Initialize()
{
    if (Initialized)
        return;
    Initialized = true;
}

void GXSkinnedMultiLightMaterialProgram::Configure(glModelPacket*)
{
}

void GXSkinnedMultiLightMaterialProgram::ConfigureVertexFormat(bool indexed)
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

void GXSkinnedMultiLightMaterialProgram::BindVertexArrays(const glModelPacket* packet)
{
    glModelStream* streams = packet->streams;
    GXSetArray(GX_VA_POS, streams[0].address, 12);
    GXSetArray(GX_VA_NRM, streams[1].address, 12);
    GXSetArray(GX_VA_TEX0, streams[2].address, 8);
    GXSetArray(GX_VA_TEX1, streams[3].address, 8);
}

void GXSkinnedMultiLightMaterialProgram::BindParameters(const glModelPacket* packet)
{
    glx_BindTexture(0, &static_cast<GXSkinnedMultiLightParameters*>(packet->materialParameters)->diffuseTexture);
    glx_BindTexture(1, &static_cast<GXSkinnedMultiLightParameters*>(packet->materialParameters)->detailTexture);
    glx_BindTexture(2, &static_cast<GXSkinnedMultiLightParameters*>(packet->materialParameters)->rampTexture);
}

const GXMaterialParameter* GXSkinnedMultiLightMaterialProgram::GetParameters()
{
    return Parameters;
}
