#include <revolution/gx.h>

#include "NL/gl/glModel.h"

struct RegistryBackend_802A3EF0
{
    void* vtable;
    u32 key;
    u32 stateSize;
    u32 descriptorCount;
};

extern u8 lbl_8052A890[];
extern u8 lbl_8052A8A0[];
extern RegistryBackend_802A3EF0* lbl_806E1CE8;
extern u8 lbl_806E1CEC;
extern GXPrimitive lbl_80524470[];

extern "C" void fn_802CB790(RegistryBackend_802A3EF0*, u32);
extern "C" void fn_8036BE88(int, u32);

extern "C" RegistryBackend_802A3EF0* fn_802A3EF0(
    RegistryBackend_802A3EF0* backend)
{
    backend->vtable = lbl_8052A8A0;
    lbl_806E1CE8 = backend;
    backend->key = 0x4ED6C66F;
    backend->stateSize = 8;
    backend->descriptorCount = 1;
    fn_802CB790(backend, 0x4ED6C66F);
    return backend;
}

extern "C" void* fn_802A3F4C(void* backend, int shouldDelete)
{
    if (backend != 0 && shouldDelete > 0)
    {
        ::operator delete(backend);
    }
    return backend;
}

extern "C" void fn_802A3F8C()
{
    if (lbl_806E1CEC == 0)
    {
        lbl_806E1CEC = 1;
    }
}

extern "C" void fn_802A3FA4(void*)
{
}

extern "C" void fn_802A3FA8(void*, bool indexed)
{
    GXClearVtxDesc();
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_S16, 6);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_S16, 10);

    if (indexed)
    {
        GXSetVtxDesc(GX_VA_POS, GX_INDEX16);
        GXSetVtxDesc(GX_VA_CLR0, GX_INDEX16);
        GXSetVtxDesc(GX_VA_TEX0, GX_INDEX16);
    }
    else
    {
        GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
        GXSetVtxDesc(GX_VA_CLR0, GX_DIRECT);
        GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);
    }
}

extern "C" void fn_802A4070(void*, const glModelPacket* packet)
{
    glModelStream* streams = packet->streams;
    GXSetArray(GX_VA_POS, streams[0].address, 6);
    GXSetArray(GX_VA_CLR0, streams[2].address, 4);
    GXSetArray(GX_VA_TEX0, streams[1].address, 4);
}

extern "C" void fn_802A40C8(void*, const glModelPacket* packet)
{
    GXBegin(
        lbl_80524470[static_cast<u8>(packet->primType)], GX_VTXFMT0, static_cast<u16>(packet->numVertices));

    u16* index = packet->indexBuffer;
    u16* end = index + packet->numVertices;
    while (index < end)
    {
        GXPosition1x16(*index);
        GXColor1x16(*index);
        GXTexCoord1x16(*index);
        ++index;
    }
}

extern "C" void fn_802A42B4(void*, const glModelPacket* packet)
{
    GXBegin(
        lbl_80524470[static_cast<u8>(packet->primType)], GX_VTXFMT0, packet->numUniqueVertices);

    for (u16 i = 0; i < packet->numUniqueVertices; ++i)
    {
        GXPosition1x16(i);
        GXColor1x16(i);
        GXTexCoord1x16(i);
    }
}

extern "C" void fn_802A4328(void*, const glModelPacket* packet)
{
    fn_8036BE88(0, reinterpret_cast<u32>(packet->unknown20));
}

extern "C" void* fn_802A4334()
{
    return lbl_8052A890;
}
