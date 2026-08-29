#include <revolution/gx.h>

#include "NL/gl/glModel.h"

struct RegistryBackend_802A5D58
{
    void* vtable;
    u32 key;
    u32 stateSize;
    u32 descriptorCount;
};

extern u8 lbl_8052AA78[];
extern u8 lbl_8052AAD8[];
extern RegistryBackend_802A5D58* lbl_806E1D20;
extern u8 lbl_806E1D24;
extern GXPrimitive lbl_80524470[];

extern "C" void fn_802CB790(RegistryBackend_802A5D58*, u32);
extern "C" void fn_8036BE88(int, u32);

extern "C" RegistryBackend_802A5D58* fn_802A5D58(
    RegistryBackend_802A5D58* backend)
{
    backend->vtable = lbl_8052AAD8;
    lbl_806E1D20 = backend;
    backend->key = 0x8D359080;
    backend->stateSize = 0x6C;
    backend->descriptorCount = 8;
    fn_802CB790(backend, 0x8D359080);
    return backend;
}

extern "C" void* fn_802A5DB4(void* backend, int shouldDelete)
{
    if (backend != 0 && shouldDelete > 0)
    {
        ::operator delete(backend);
    }
    return backend;
}

extern "C" void fn_802A5DF4()
{
    if (lbl_806E1D24 == 0)
    {
        lbl_806E1D24 = 1;
    }
}

extern "C" void fn_802A5E0C(void*)
{
}

extern "C" void fn_802A5E10(void*, bool indexed)
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

extern "C" void fn_802A5ED8(void*, const glModelPacket* packet)
{
    glModelStream* streams = packet->streams;
    GXSetArray(GX_VA_POS, streams[0].address, 12);
    GXSetArray(GX_VA_NRM, streams[1].address, 12);
    GXSetArray(GX_VA_TEX0, streams[2].address, 8);
}

extern "C" void fn_802A5F30(void*, const glModelPacket* packet)
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

extern "C" void fn_802A611C(void*, const glModelPacket* packet)
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

extern "C" void fn_802A6190(void*, const glModelPacket* packet)
{
    fn_8036BE88(0, reinterpret_cast<u32>(packet->unknown20));
}

extern "C" void* fn_802A619C()
{
    return lbl_8052AA78;
}
