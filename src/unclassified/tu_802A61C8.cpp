#include <revolution/gx.h>

#include "NL/gl/glModel.h"

struct RegistryBackend_802A61C8
{
    void* vtable;
    u32 key;
    u32 stateSize;
    u32 descriptorCount;
};

extern u8 lbl_8052AB00[];
extern u8 lbl_8052AB18[];
extern RegistryBackend_802A61C8* lbl_806E1D28;
extern u8 lbl_806E1D2C;

extern "C" void fn_802CB790(RegistryBackend_802A61C8*, u32);
extern "C" void fn_8036BE88(int, u32);

extern "C" RegistryBackend_802A61C8* fn_802A61C8(
    RegistryBackend_802A61C8* backend)
{
    backend->vtable = lbl_8052AB18;
    lbl_806E1D28 = backend;
    backend->key = 0x5D6C62BA;
    backend->stateSize = 0x10;
    backend->descriptorCount = 2;
    fn_802CB790(backend, 0x5D6C62BA);
    return backend;
}

extern "C" void* fn_802A6224(void* backend, int shouldDelete)
{
    if (backend != 0 && shouldDelete > 0)
    {
        ::operator delete(backend);
    }
    return backend;
}

extern "C" void fn_802A6264()
{
    if (lbl_806E1D2C == 0)
    {
        lbl_806E1D2C = 1;
    }
}

extern "C" void fn_802A627C(void*)
{
}

extern "C" void fn_802A6280(void*, bool indexed)
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

extern "C" void fn_802A6348(void*, const glModelPacket* packet)
{
    glModelStream* streams = packet->streams;
    GXSetArray(GX_VA_POS, streams[0].address, 12);
    GXSetArray(GX_VA_NRM, streams[1].address, 12);
    GXSetArray(GX_VA_TEX0, streams[2].address, 8);
}

extern "C" void fn_802A63A0(void*, const glModelPacket* packet)
{
    fn_8036BE88(0, reinterpret_cast<u32>(packet->unknown20));
}

extern "C" void* fn_802A63AC()
{
    return lbl_8052AB00;
}
