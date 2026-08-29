#include <revolution/gx.h>

#include "NL/gl/glModel.h"

struct RegistryBackend_802A4B28
{
    void* vtable;
    u32 key;
    u32 stateSize;
    u32 descriptorCount;
};

extern u8 lbl_8052A948[];
extern u8 lbl_8052A958[];
extern RegistryBackend_802A4B28* lbl_806E1D00;
extern u8 lbl_806E1D04;
extern GXPrimitive lbl_80524470[];

extern "C" void fn_802CB790(RegistryBackend_802A4B28*, u32);
extern "C" void fn_8036BE88(int, u32);

extern "C" RegistryBackend_802A4B28* fn_802A4B28(
    RegistryBackend_802A4B28* backend)
{
    backend->vtable = lbl_8052A958;
    lbl_806E1D00 = backend;
    backend->key = 0x21DB4385;
    backend->stateSize = 8;
    backend->descriptorCount = 1;
    fn_802CB790(backend, 0x21DB4385);
    return backend;
}

extern "C" void* fn_802A4B84(void* backend, int shouldDelete)
{
    if (backend != 0 && shouldDelete > 0)
    {
        ::operator delete(backend);
    }
    return backend;
}

extern "C" void fn_802A4BC4()
{
    if (lbl_806E1D04 == 0)
    {
        lbl_806E1D04 = 1;
    }
}

extern "C" void fn_802A4BDC(void*)
{
}

extern "C" void fn_802A4BE0(void*, bool indexed)
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

extern "C" void fn_802A4C78(void*, const glModelPacket* packet)
{
    glModelStream* streams = packet->streams;
    GXSetArray(GX_VA_POS, streams[0].address, 12);
    GXSetArray(GX_VA_TEX0, streams[1].address, 8);
}

extern "C" void fn_802A4CC0(void*, const glModelPacket* packet)
{
    GXBegin(
        lbl_80524470[static_cast<u8>(packet->primType)], GX_VTXFMT0, static_cast<u16>(packet->numVertices));

    u16* index = packet->indexBuffer;
    u16* end = index + packet->numVertices;
    while (index < end)
    {
        GXPosition1x16(*index);
        GXTexCoord1x16(*index);
        ++index;
    }
}

extern "C" void fn_802A4E64(void*, const glModelPacket* packet)
{
    GXBegin(
        lbl_80524470[static_cast<u8>(packet->primType)], GX_VTXFMT0, packet->numUniqueVertices);

    for (u16 i = 0; i < packet->numUniqueVertices; ++i)
    {
        GXPosition1x16(i);
        GXTexCoord1x16(i);
    }
}

extern "C" void fn_802A4ED4(void*, const glModelPacket* packet)
{
    fn_8036BE88(0, reinterpret_cast<u32>(packet->unknown20));
}

extern "C" void* fn_802A4EE0()
{
    return lbl_8052A948;
}
