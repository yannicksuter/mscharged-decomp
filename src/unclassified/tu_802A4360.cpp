#include <revolution/gx.h>

#include "NL/gl/glModel.h"

struct RegistryBackend_802A4360
{
    void* vtable;
    u32 key;
    u32 stateSize;
    u32 descriptorCount;
};

extern u8 lbl_8052A8C8[];
extern u8 lbl_8052A8E0[];
extern RegistryBackend_802A4360* lbl_806E1CF0;
extern u8 lbl_806E1CF4;
extern GXPrimitive lbl_80524470[];

extern "C" void fn_802CB790(RegistryBackend_802A4360*, u32);
extern "C" void fn_8036BE88(int, u32);

extern "C" RegistryBackend_802A4360* fn_802A4360(
    RegistryBackend_802A4360* backend)
{
    backend->vtable = lbl_8052A8E0;
    lbl_806E1CF0 = backend;
    backend->key = 0xEE9D919D;
    backend->stateSize = 0x18;
    backend->descriptorCount = 2;
    fn_802CB790(backend, 0xEE9D919D);
    return backend;
}

extern "C" void* fn_802A43BC(void* backend, int shouldDelete)
{
    if (backend != 0 && shouldDelete > 0)
    {
        ::operator delete(backend);
    }
    return backend;
}

extern "C" void fn_802A43FC()
{
    if (lbl_806E1CF4 == 0)
    {
        lbl_806E1CF4 = 1;
    }
}

extern "C" void fn_802A4414(void*)
{
}

extern "C" void fn_802A4418(void*, bool indexed)
{
    GXClearVtxDesc();
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_S16, 12);

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

extern "C" void fn_802A44B0(void*, const glModelPacket* packet)
{
    glModelStream* streams = packet->streams;
    GXSetArray(GX_VA_POS, streams[0].address, 12);
    GXSetArray(GX_VA_TEX0, streams[1].address, 4);
}

extern "C" void fn_802A44F8(void*, const glModelPacket* packet)
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

extern "C" void fn_802A469C(void*, const glModelPacket* packet)
{
    GXBegin(
        lbl_80524470[static_cast<u8>(packet->primType)], GX_VTXFMT0, packet->numUniqueVertices);

    for (u16 i = 0; i < packet->numUniqueVertices; ++i)
    {
        GXPosition1x16(i);
        GXTexCoord1x16(i);
    }
}

extern "C" void fn_802A470C(void*, const glModelPacket* packet)
{
    fn_8036BE88(0, reinterpret_cast<u32>(packet->unknown20));
}

extern "C" void* fn_802A4718()
{
    return lbl_8052A8C8;
}
