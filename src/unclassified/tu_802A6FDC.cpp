#include <revolution/gx.h>

#include "NL/gl/glModel.h"

struct RegistryBackend_802A6FDC
{
    void* vtable;
    u32 key;
    u32 stateSize;
    u32 descriptorCount;
};

extern u8 lbl_8052ABE0[];
extern RegistryBackend_802A6FDC* lbl_806E1D48;
extern u8 lbl_806E1D4C;
extern GXPrimitive lbl_80524470[];

extern "C" void fn_802CB790(RegistryBackend_802A6FDC*, u32);

extern "C" RegistryBackend_802A6FDC* fn_802A6FDC(
    RegistryBackend_802A6FDC* backend)
{
    backend->vtable = lbl_8052ABE0;
    lbl_806E1D48 = backend;
    backend->key = 0xD701656B;
    backend->stateSize = 0;
    backend->descriptorCount = 0;
    fn_802CB790(backend, 0xD701656B);
    return backend;
}

extern "C" void* fn_802A7034(void* backend, int shouldDelete)
{
    if (backend != 0 && shouldDelete > 0)
    {
        ::operator delete(backend);
    }
    return backend;
}

extern "C" void fn_802A7074()
{
    if (lbl_806E1D4C == 0)
    {
        lbl_806E1D4C = 1;
    }
}

extern "C" void fn_802A708C(void*)
{
}

extern "C" void fn_802A7090(void*, bool indexed)
{
    GXClearVtxDesc();
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);

    if (indexed)
    {
        GXSetVtxDesc(GX_VA_POS, GX_INDEX16);
        GXSetVtxDesc(GX_VA_CLR0, GX_INDEX16);
    }
    else
    {
        GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
        GXSetVtxDesc(GX_VA_CLR0, GX_DIRECT);
    }
}

extern "C" void fn_802A7128(void*, const glModelPacket* packet)
{
    glModelStream* streams = packet->streams;
    GXSetArray(GX_VA_POS, streams[0].address, 12);
    GXSetArray(GX_VA_CLR0, streams[1].address, 4);
}

extern "C" void fn_802A7170(void*, const glModelPacket* packet)
{
    GXBegin(
        lbl_80524470[static_cast<u8>(packet->primType)], GX_VTXFMT0, static_cast<u16>(packet->numVertices));

    u16* index = packet->indexBuffer;
    u16* end = index + packet->numVertices;
    while (index < end)
    {
        GXPosition1x16(*index);
        GXColor1x16(*index);
        ++index;
    }
}

extern "C" void fn_802A7314(void*, const glModelPacket* packet)
{
    GXBegin(
        lbl_80524470[static_cast<u8>(packet->primType)], GX_VTXFMT0, packet->numUniqueVertices);

    for (u16 i = 0; i < packet->numUniqueVertices; ++i)
    {
        GXPosition1x16(i);
        GXColor1x16(i);
    }
}

extern "C" void fn_802A7384(void*, const glModelPacket*)
{
}

extern "C" void* fn_802A7388()
{
    return 0;
}
