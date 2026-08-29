#include <revolution/gx.h>

#include "NL/gl/glModel.h"

struct RegistryBackend_802A6848
{
    void* vtable;
    u32 key;
    u32 stateSize;
    u32 descriptorCount;
};

extern u8 lbl_8052AB78[];
extern RegistryBackend_802A6848* lbl_806E1D38;
extern u8 lbl_806E1D3C;
extern GXPrimitive lbl_80524470[];

extern "C" void fn_802CB790(RegistryBackend_802A6848*, u32);

extern "C" RegistryBackend_802A6848* fn_802A6848(
    RegistryBackend_802A6848* backend)
{
    backend->vtable = lbl_8052AB78;
    lbl_806E1D38 = backend;
    backend->key = 0xDC56470F;
    backend->stateSize = 0;
    backend->descriptorCount = 0;
    fn_802CB790(backend, 0xDC56470F);
    return backend;
}

extern "C" void* fn_802A68A0(void* backend, int shouldDelete)
{
    if (backend != 0 && shouldDelete > 0)
    {
        ::operator delete(backend);
    }
    return backend;
}

extern "C" void fn_802A68E0()
{
    if (lbl_806E1D3C == 0)
    {
        lbl_806E1D3C = 1;
    }
}

extern "C" void fn_802A68F8(void*)
{
}

extern "C" void fn_802A68FC(void*, bool indexed)
{
    GXClearVtxDesc();
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);

    if (indexed)
    {
        GXSetVtxDesc(GX_VA_POS, GX_INDEX16);
    }
    else
    {
        GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
    }
}

extern "C" void fn_802A6964(void*, const glModelPacket* packet)
{
    glModelStream* streams = packet->streams;
    GXSetArray(GX_VA_POS, streams[0].address, 12);
}

extern "C" void fn_802A6978(void*, const glModelPacket* packet)
{
    GXBegin(
        lbl_80524470[static_cast<u8>(packet->primType)], GX_VTXFMT0, static_cast<u16>(packet->numVertices));

    u16* index = packet->indexBuffer;
    u16* end = index + packet->numVertices;
    while (index < end)
    {
        GXPosition1x16(*index);
        ++index;
    }
}

extern "C" void fn_802A6AD4(void*, const glModelPacket* packet)
{
    GXBegin(
        lbl_80524470[static_cast<u8>(packet->primType)], GX_VTXFMT0, packet->numUniqueVertices);

    for (u16 i = 0; i < packet->numUniqueVertices; ++i)
    {
        GXPosition1x16(i);
    }
}

extern "C" void fn_802A6B40(void*, const glModelPacket*)
{
}

extern "C" void* fn_802A6B44()
{
    return 0;
}
