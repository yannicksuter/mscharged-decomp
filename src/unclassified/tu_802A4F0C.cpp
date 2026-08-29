#include <revolution/gx.h>

#include "NL/gl/glModel.h"

struct RegistryBackend_802A4F0C
{
    void* vtable;
    u32 key;
    u32 stateSize;
    u32 descriptorCount;
};

extern u8 lbl_8052A980[];
extern u8 lbl_8052A9A8[];
extern RegistryBackend_802A4F0C* lbl_806E1D08;
extern u8 lbl_806E1D0C;
extern GXPrimitive lbl_80524470[];

extern "C" void fn_802CB790(RegistryBackend_802A4F0C*, u32);
extern "C" void fn_8036BE88(int, u32);

extern "C" RegistryBackend_802A4F0C* fn_802A4F0C(
    RegistryBackend_802A4F0C* backend)
{
    backend->vtable = lbl_8052A9A8;
    lbl_806E1D08 = backend;
    backend->key = 0x46ABE398;
    backend->stateSize = 0x14;
    backend->descriptorCount = 3;
    fn_802CB790(backend, 0x46ABE398);
    return backend;
}

extern "C" void* fn_802A4F68(void* backend, int shouldDelete)
{
    if (backend != 0 && shouldDelete > 0)
    {
        ::operator delete(backend);
    }
    return backend;
}

extern "C" void fn_802A4FA8()
{
    if (lbl_806E1D0C == 0)
    {
        lbl_806E1D0C = 1;
    }
}

extern "C" void fn_802A4FC0(void*)
{
}

extern "C" void fn_802A4FC4(void*, bool indexed)
{
    GXClearVtxDesc();
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_S16, 8);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX1, GX_TEX_ST, GX_S16, 8);

    if (indexed)
    {
        GXSetVtxDesc(GX_VA_POS, GX_INDEX16);
        GXSetVtxDesc(GX_VA_TEX0, GX_INDEX16);
        GXSetVtxDesc(GX_VA_TEX1, GX_INDEX16);
    }
    else
    {
        GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
        GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);
        GXSetVtxDesc(GX_VA_TEX1, GX_DIRECT);
    }
}

extern "C" void fn_802A508C(void*, const glModelPacket* packet)
{
    glModelStream* streams = packet->streams;
    GXSetArray(GX_VA_POS, streams[0].address, 12);
    GXSetArray(GX_VA_TEX0, streams[1].address, 4);
    GXSetArray(GX_VA_TEX1, streams[2].address, 4);
}

extern "C" void fn_802A50E4(void*, const glModelPacket* packet)
{
    GXBegin(
        lbl_80524470[static_cast<u8>(packet->primType)], GX_VTXFMT0, static_cast<u16>(packet->numVertices));

    u16* index = packet->indexBuffer;
    u16* end = index + packet->numVertices;
    while (index < end)
    {
        GXPosition1x16(*index);
        GXTexCoord1x16(*index);
        GXTexCoord1x16(*index);
        ++index;
    }
}

extern "C" void fn_802A52D0(void*, const glModelPacket* packet)
{
    GXBegin(
        lbl_80524470[static_cast<u8>(packet->primType)], GX_VTXFMT0, packet->numUniqueVertices);

    for (u16 i = 0; i < packet->numUniqueVertices; ++i)
    {
        GXPosition1x16(i);
        GXTexCoord1x16(i);
        GXTexCoord1x16(i);
    }
}

extern "C" void fn_802A5344(void*, const glModelPacket* packet)
{
    fn_8036BE88(0, reinterpret_cast<u32>(packet->unknown20));
    fn_8036BE88(1, reinterpret_cast<u32>(static_cast<u8*>(packet->unknown20) + 8));
}

extern "C" void* fn_802A5388()
{
    return lbl_8052A980;
}
