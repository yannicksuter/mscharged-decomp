#include <revolution/gx.h>

#include "NL/gl/glModel.h"

struct RegistryBackend_802A58E8
{
    void* vtable;
    u32 key;
    u32 stateSize;
    u32 descriptorCount;
};

extern u8 lbl_8052AA20[];
extern u8 lbl_8052AA50[];
extern RegistryBackend_802A58E8* lbl_806E1D18;
extern u8 lbl_806E1D1C;
extern GXPrimitive lbl_80524470[];

extern "C" void fn_802CB790(RegistryBackend_802A58E8*, u32);
extern "C" void fn_8036BE88(int, u32);

extern "C" RegistryBackend_802A58E8* fn_802A58E8(
    RegistryBackend_802A58E8* backend)
{
    backend->vtable = lbl_8052AA50;
    lbl_806E1D18 = backend;
    backend->key = 0xAD0DB7E9;
    backend->stateSize = 0x38;
    backend->descriptorCount = 4;
    fn_802CB790(backend, 0xAD0DB7E9);
    return backend;
}

extern "C" void* fn_802A5944(void* backend, int shouldDelete)
{
    if (backend != 0 && shouldDelete > 0)
    {
        ::operator delete(backend);
    }
    return backend;
}

extern "C" void fn_802A5984()
{
    if (lbl_806E1D1C == 0)
    {
        lbl_806E1D1C = 1;
    }
}

extern "C" void fn_802A599C(void*)
{
}

extern "C" void fn_802A59A0(void*, bool indexed)
{
    GXClearVtxDesc();
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_NRM, GX_NRM_XYZ, GX_F32, 0);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);

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

extern "C" void fn_802A5A68(void*, const glModelPacket* packet)
{
    glModelStream* streams = packet->streams;
    GXSetArray(GX_VA_POS, streams[0].address, 12);
    GXSetArray(GX_VA_NRM, streams[1].address, 12);
    GXSetArray(GX_VA_TEX0, streams[2].address, 8);
}

extern "C" void fn_802A5AC0(void*, const glModelPacket* packet)
{
    GXBegin(
        lbl_80524470[static_cast<u8>(packet->primType)], GX_VTXFMT0, static_cast<u16>(packet->numVertices));

    u16* index = packet->indexBuffer;
    u16* end = index + packet->numVertices;
    while (index < end)
    {
        GXPosition1x16(*index);
        GXNormal1x16(*index);
        GXTexCoord1x16(*index);
        ++index;
    }
}

extern "C" void fn_802A5CAC(void*, const glModelPacket* packet)
{
    GXBegin(
        lbl_80524470[static_cast<u8>(packet->primType)], GX_VTXFMT0, packet->numUniqueVertices);

    for (u16 i = 0; i < packet->numUniqueVertices; ++i)
    {
        GXPosition1x16(i);
        GXNormal1x16(i);
        GXTexCoord1x16(i);
    }
}

extern "C" void fn_802A5D20(void*, const glModelPacket* packet)
{
    fn_8036BE88(0, reinterpret_cast<u32>(packet->unknown20));
}

extern "C" void* fn_802A5D2C()
{
    return lbl_8052AA20;
}
