#include <revolution/gx.h>

#include "NL/gl/glModel.h"

struct RegistryBackend_802A63D8
{
    void* vtable;
    u32 key;
    u32 stateSize;
    u32 descriptorCount;
};

extern u8 lbl_8052AB40[];
extern u8 lbl_8052AB50[];
extern RegistryBackend_802A63D8* lbl_806E1D30;
extern u8 lbl_806E1D34;
extern GXPrimitive lbl_80524470[];

extern "C" void fn_802CB790(RegistryBackend_802A63D8*, u32);
extern "C" void fn_8036BE88(int, u32);

extern "C" RegistryBackend_802A63D8* fn_802A63D8(
    RegistryBackend_802A63D8* backend)
{
    backend->vtable = lbl_8052AB50;
    lbl_806E1D30 = backend;
    backend->key = 0xD3E572DA;
    backend->stateSize = 8;
    backend->descriptorCount = 1;
    fn_802CB790(backend, 0xD3E572DA);
    return backend;
}

extern "C" void* fn_802A6434(void* backend, int shouldDelete)
{
    if (backend != 0 && shouldDelete > 0)
    {
        ::operator delete(backend);
    }
    return backend;
}

extern "C" void fn_802A6474()
{
    if (lbl_806E1D34 == 0)
    {
        lbl_806E1D34 = 1;
    }
}

extern "C" void fn_802A648C(void*)
{
}

extern "C" void fn_802A6490(void*, bool indexed)
{
    GXClearVtxDesc();
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
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

extern "C" void fn_802A6558(void*, const glModelPacket* packet)
{
    glModelStream* streams = packet->streams;
    GXSetArray(GX_VA_POS, streams[0].address, 12);
    GXSetArray(GX_VA_CLR0, streams[2].address, 4);
    GXSetArray(GX_VA_TEX0, streams[1].address, 4);
}

extern "C" void fn_802A65B0(void*, const glModelPacket* packet)
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

extern "C" void fn_802A679C(void*, const glModelPacket* packet)
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

extern "C" void fn_802A6810(void*, const glModelPacket* packet)
{
    fn_8036BE88(0, reinterpret_cast<u32>(packet->unknown20));
}

extern "C" void* fn_802A681C()
{
    return lbl_8052AB40;
}
